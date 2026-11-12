cmake_minimum_required(VERSION 3.16)
project(MakingGamesWithBen_GameEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_NINJA_PLATFORMER "Build NinjaPlatformer (requires CEGUI-0 and Box2D 2.3.1)" ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm QUIET)
find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL2 REQUIRED sdl2)
pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

# The codebase uses #include <SDL/SDL.h>, <SDL/SDL_ttf.h>, <SDL/SDL_mixer.h>.
# On Linux distros (e.g. Fedora / Bazzite), SDL2 headers are installed under
# /usr/include/SDL2. Create an include/SDL symlink in the build directory so
# both <SDL/SDL.h> and <SDL.h> resolve cleanly against SDL2.
set(SDL_COMPAT_INCLUDE_DIR "${CMAKE_BINARY_DIR}/sdl2_compat_include")
file(MAKE_DIRECTORY "${SDL_COMPAT_INCLUDE_DIR}")
list(GET SDL2_INCLUDE_DIRS 0 PRIMARY_SDL2_INC_DIR)
if(NOT EXISTS "${SDL_COMPAT_INCLUDE_DIR}/SDL")
    file(CREATE_LINK "${PRIMARY_SDL2_INC_DIR}" "${SDL_COMPAT_INCLUDE_DIR}/SDL" SYMBOLIC)
endif()

# Optional CEGUI detection for GUI.cpp and NinjaPlatformer
# Supports both Ubuntu/Debian (libcegui-mk2-dev -> CEGUI-0 / cegui-0.8.7)
# and Fedora (cegui-devel -> CEGUI-0 / cegui-0)
pkg_check_modules(CEGUI QUIET CEGUI-0 CEGUI-0-OPENGL)
if(NOT CEGUI_FOUND)
    pkg_check_modules(CEGUI QUIET cegui-0 cegui-0-opengl)
endif()

# Also search multiarch and versioned header/library paths directly (Ubuntu/Debian)
find_path(CEGUI_MAIN_INCLUDE_DIR
    NAMES CEGUI/CEGUI.h
    PATH_SUFFIXES cegui-0 cegui-0.8.7
)
find_path(CEGUI_CONFIG_INCLUDE_DIR
    NAMES CEGUI/Config.h
    PATH_SUFFIXES cegui-0 cegui-0.8.7
)
find_library(CEGUI_BASE_LIBRARY NAMES CEGUIBase-0)
find_library(CEGUI_OPENGL_LIBRARY NAMES CEGUIOpenGLRenderer-0)

if(NOT CEGUI_FOUND AND CEGUI_MAIN_INCLUDE_DIR AND CEGUI_BASE_LIBRARY AND CEGUI_OPENGL_LIBRARY)
    set(CEGUI_FOUND TRUE)
    set(CEGUI_INCLUDE_DIRS "${CEGUI_MAIN_INCLUDE_DIR}")
    set(CEGUI_LIBRARIES "${CEGUI_BASE_LIBRARY}" "${CEGUI_OPENGL_LIBRARY}")
endif()
if(CEGUI_CONFIG_INCLUDE_DIR)
    list(APPEND CEGUI_INCLUDE_DIRS "${CEGUI_CONFIG_INCLUDE_DIR}")
    list(REMOVE_DUPLICATES CEGUI_INCLUDE_DIRS)
endif()

set(ENGINE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/GameEngine")

set(ENGINE_SOURCES
    "${ENGINE_DIR}/AudioEngine.cpp"
    "${ENGINE_DIR}/Camera2D.cpp"
    "${ENGINE_DIR}/DebugRenderer.cpp"
    "${ENGINE_DIR}/GLSLProgram.cpp"
    "${ENGINE_DIR}/GameEngine.cpp"
    "${ENGINE_DIR}/GameEngineErrors.cpp"
    "${ENGINE_DIR}/IMainGame.cpp"
    "${ENGINE_DIR}/IOManager.cpp"
    "${ENGINE_DIR}/ImageLoader.cpp"
    "${ENGINE_DIR}/InputManager.cpp"
    "${ENGINE_DIR}/ParticleBatch2D.cpp"
    "${ENGINE_DIR}/ParticleEngine2D.cpp"
    "${ENGINE_DIR}/ResourceManager.cpp"
    "${ENGINE_DIR}/ScreenList.cpp"
    "${ENGINE_DIR}/Sprite.cpp"
    "${ENGINE_DIR}/SpriteBatch.cpp"
    "${ENGINE_DIR}/TextureCache.cpp"
    "${ENGINE_DIR}/Timing.cpp"
    "${ENGINE_DIR}/Window.cpp"
    "${ENGINE_DIR}/picoPNG.cpp"
    "${ENGINE_DIR}/SoA/SpriteFont.cpp"
)

if(CEGUI_FOUND)
    list(APPEND ENGINE_SOURCES "${ENGINE_DIR}/GUI.cpp")
endif()

add_library(GameEngine STATIC ${ENGINE_SOURCES})

target_compile_definitions(GameEngine PUBLIC GLM_ENABLE_EXPERIMENTAL)

target_include_directories(GameEngine PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine"
    "${ENGINE_DIR}"
    "${SDL_COMPAT_INCLUDE_DIR}"
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
)

if(CEGUI_FOUND)
    target_include_directories(GameEngine PUBLIC ${CEGUI_INCLUDE_DIRS})
    if(CEGUI_LIBRARY_DIRS)
        target_link_directories(GameEngine PUBLIC ${CEGUI_LIBRARY_DIRS})
    endif()
    target_link_libraries(GameEngine PUBLIC ${CEGUI_LIBRARIES})
endif()

target_link_directories(GameEngine PUBLIC
    ${SDL2_LIBRARY_DIRS}
    ${SDL2_TTF_LIBRARY_DIRS}
    ${SDL2_MIXER_LIBRARY_DIRS}
)

target_link_libraries(GameEngine PUBLIC
    OpenGL::GL
    GLEW::GLEW
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)

# Helper to symlink game asset directories into the target's build folder
function(link_game_assets TARGET_NAME SOURCE_GAME_DIR)
    set(DEST_DIR "${CMAKE_BINARY_DIR}/bin/${TARGET_NAME}")
    file(MAKE_DIRECTORY "${DEST_DIR}")
    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${DEST_DIR}"
    )
    foreach(ASSET_DIR ${ARGN})
        if(EXISTS "${SOURCE_GAME_DIR}/${ASSET_DIR}" AND NOT EXISTS "${DEST_DIR}/${ASSET_DIR}")
            file(CREATE_LINK "${SOURCE_GAME_DIR}/${ASSET_DIR}" "${DEST_DIR}/${ASSET_DIR}" SYMBOLIC)
        endif()
    endforeach()
endfunction()

# -----------------------------------------------------------------------------
# 1. TutorialGame (Tutorials 1 - 17)
# -----------------------------------------------------------------------------
set(TUTORIAL_GAME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/TutorialGame")
add_executable(TutorialGame
    "${TUTORIAL_GAME_DIR}/MainGame.cpp"
    "${TUTORIAL_GAME_DIR}/Bullet.cpp"
    "${TUTORIAL_GAME_DIR}/main.cpp"
)
target_include_directories(TutorialGame PRIVATE "${TUTORIAL_GAME_DIR}")
target_link_libraries(TutorialGame PRIVATE GameEngine)
link_game_assets(TutorialGame "${TUTORIAL_GAME_DIR}" Shaders Textures)

# -----------------------------------------------------------------------------
# 2. ZombieGame (Tutorials 18 - 38)
# -----------------------------------------------------------------------------
set(ZOMBIE_GAME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/ZombieGame")
add_executable(ZombieGame
    "${ZOMBIE_GAME_DIR}/Agent.cpp"
    "${ZOMBIE_GAME_DIR}/Bullet.cpp"
    "${ZOMBIE_GAME_DIR}/Gun.cpp"
    "${ZOMBIE_GAME_DIR}/Human.cpp"
    "${ZOMBIE_GAME_DIR}/Level.cpp"
    "${ZOMBIE_GAME_DIR}/MainGame.cpp"
    "${ZOMBIE_GAME_DIR}/Player.cpp"
    "${ZOMBIE_GAME_DIR}/Zombie.cpp"
    "${ZOMBIE_GAME_DIR}/main.cpp"
)
target_include_directories(ZombieGame PRIVATE "${ZOMBIE_GAME_DIR}")
target_link_libraries(ZombieGame PRIVATE GameEngine)
link_game_assets(ZombieGame "${ZOMBIE_GAME_DIR}" Shaders Textures Audio Fonts Levels)

# -----------------------------------------------------------------------------
# 3. BallGame (Tutorials 39 - 43)
# -----------------------------------------------------------------------------
set(BALL_GAME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/BallGame")
add_executable(BallGame
    "${BALL_GAME_DIR}/Ball.cpp"
    "${BALL_GAME_DIR}/BallController.cpp"
    "${BALL_GAME_DIR}/BallRenderer.cpp"
    "${BALL_GAME_DIR}/Grid.cpp"
    "${BALL_GAME_DIR}/MainGame.cpp"
    "${BALL_GAME_DIR}/main.cpp"
)
target_include_directories(BallGame PRIVATE "${BALL_GAME_DIR}")
target_link_libraries(BallGame PRIVATE GameEngine)
link_game_assets(BallGame "${BALL_GAME_DIR}" Shaders Textures Fonts)

# -----------------------------------------------------------------------------
# 4. NinjaPlatformer (Tutorials 44 - 58)
# -----------------------------------------------------------------------------
if(BUILD_NINJA_PLATFORMER AND CEGUI_FOUND)
    include(FetchContent)
    # NinjaPlatformer uses Box2D v2.3.1 API (<Box2D/Box2D.h>)
    FetchContent_Declare(
        box2d_v231
        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        GIT_TAG v2.3.1
        SOURCE_SUBDIR __nonexistent_subdir_to_skip_legacy_cmake__
    )
    FetchContent_MakeAvailable(box2d_v231)

    file(GLOB_RECURSE BOX2D_V231_SOURCES
        "${box2d_v231_SOURCE_DIR}/Box2D/Box2D/*.cpp"
    )
    add_library(Box2D_v231 STATIC ${BOX2D_V231_SOURCES})
    target_include_directories(Box2D_v231 PUBLIC "${box2d_v231_SOURCE_DIR}/Box2D")

    set(NINJA_GAME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/NinjaPlatformer")
    add_executable(NinjaPlatformer
        "${NINJA_GAME_DIR}/App.cpp"
        "${NINJA_GAME_DIR}/Box.cpp"
        "${NINJA_GAME_DIR}/Capsule.cpp"
        "${NINJA_GAME_DIR}/EditorScreen.cpp"
        "${NINJA_GAME_DIR}/GameplayScreen.cpp"
        "${NINJA_GAME_DIR}/LevelReaderWriter.cpp"
        "${NINJA_GAME_DIR}/MainMenuScreen.cpp"
        "${NINJA_GAME_DIR}/Player.cpp"
        "${NINJA_GAME_DIR}/main.cpp"
    )
    target_include_directories(NinjaPlatformer PRIVATE "${NINJA_GAME_DIR}")
    target_link_libraries(NinjaPlatformer PRIVATE GameEngine Box2D_v231)
    link_game_assets(NinjaPlatformer "${NINJA_GAME_DIR}" Shaders Assets Fonts GUI Levels)
elseif(BUILD_NINJA_PLATFORMER AND NOT CEGUI_FOUND)
    message(STATUS "Skipping NinjaPlatformer: CEGUI-0 (cegui-0, cegui-0-opengl) was not found via pkg-config.")
endif()

# -----------------------------------------------------------------------------
# 5. Automated Engine Tests (CTest)
# -----------------------------------------------------------------------------
enable_testing()

set(TEST_SOURCES
    "tests/main.cpp"
    "tests/test_camera.cpp"
    "tests/test_grid.cpp"
    "tests/test_input.cpp"
    "tests/test_screen_list.cpp"
    "tests/test_iomanager.cpp"
    "${BALL_GAME_DIR}/Grid.cpp"
    "${BALL_GAME_DIR}/Ball.cpp"
)

if(TARGET Box2D_v231)
    list(APPEND TEST_SOURCES
        "tests/test_level_rw.cpp"
        "tests/test_shaders.cpp"
        "tests/test_spritefont.cpp"
        "${NINJA_GAME_DIR}/LevelReaderWriter.cpp"
        "${NINJA_GAME_DIR}/Box.cpp"
        "${NINJA_GAME_DIR}/Capsule.cpp"
        "${NINJA_GAME_DIR}/Player.cpp"
    )
endif()

add_executable(EngineTests ${TEST_SOURCES})
target_include_directories(EngineTests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/tests"
    "${BALL_GAME_DIR}"
    "${NINJA_GAME_DIR}"
)

target_link_libraries(EngineTests PRIVATE GameEngine)
if(TARGET Box2D_v231)
    target_link_libraries(EngineTests PRIVATE Box2D_v231)
    link_game_assets(EngineTests "${NINJA_GAME_DIR}" Shaders Assets Fonts Levels)
endif()

set(TEST_CASES
    "Camera2D_BasicInitAndMatrix"
    "Camera2D_FrustumCulling"
    "Camera2D_ScreenToWorld"
    "Grid_BoundaryClamping"
    "Grid_AddAndRemoveBall"
    "InputManager_KeyPressAndRelease"
    "InputManager_MouseCoords"
    "ScreenList_Navigation"
    "IOManager_ReadFileToBuffer"
)

if(TARGET Box2D_v231)
    list(APPEND TEST_CASES
        "LevelReaderWriter_RoundTripText"
        "LevelReaderWriter_LoadShippedLevel"
        "Shader_TextureShadingCompileAndLink"
        "Shader_LightShadingCompileAndLink"
        "Shader_ColorCompileAndLink"
        "SpriteFont_InitAndMeasure"
        "SpriteFont_TextureAtlasNoSkewRegression"
    )
endif()

foreach(TC ${TEST_CASES})
    add_test(NAME "Engine.${TC}" COMMAND EngineTests "${TC}")
    set_tests_properties("Engine.${TC}" PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/EngineTests")
endforeach()
