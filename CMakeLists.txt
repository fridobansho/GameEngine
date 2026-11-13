cmake_minimum_required(VERSION 3.16)
project(MakingGamesWithBen_GameEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_NINJA_PLATFORMER "Build NinjaPlatformer (uses Box2D 2.3.1; uses CEGUI-0 if available or built-in fallback)" ON)
option(ENABLE_CEGUI "Enable CEGUI support when available on desktop" ON)
set(ANDROID_GAME_TARGET "NinjaPlatformer" CACHE STRING "Game target to build as libmain.so on Android (TutorialGame, ZombieGame, BallGame, NinjaPlatformer)")
set_property(CACHE ANDROID_GAME_TARGET PROPERTY STRINGS TutorialGame ZombieGame BallGame NinjaPlatformer)

set(SDL_COMPAT_INCLUDE_DIR "${CMAKE_BINARY_DIR}/sdl2_compat_include")
file(MAKE_DIRECTORY "${SDL_COMPAT_INCLUDE_DIR}")

set(GAMEENGINE_GL_INCLUDE_DIRS "")
set(GAMEENGINE_GL_LIBRARIES "")
set(GAMEENGINE_SDL_INCLUDE_DIRS "")
set(GAMEENGINE_SDL_LIBRARY_DIRS "")
set(GAMEENGINE_SDL_LIBRARIES "")

if(ANDROID)
    set(CEGUI_FOUND FALSE)
    find_package(glm QUIET)

    # Locate Android NDK GLESv3, EGL, android, and log libraries
    find_library(ANDROID_GLESV3_LIB GLESv3)
    find_library(ANDROID_EGL_LIB EGL)
    find_library(ANDROID_NDK_LIB android)
    find_library(ANDROID_LOG_LIB log)
    foreach(_lib_var ANDROID_GLESV3_LIB ANDROID_EGL_LIB ANDROID_NDK_LIB ANDROID_LOG_LIB)
        if(${_lib_var})
            list(APPEND GAMEENGINE_GL_LIBRARIES "${${_lib_var}}")
        endif()
    endforeach()
    if(NOT GAMEENGINE_GL_LIBRARIES)
        set(GAMEENGINE_GL_LIBRARIES GLESv3 EGL android log)
    endif()

    # Optional SDL2 / SDL2_ttf / SDL2_mixer CMake subproject directories
    if(NOT SDL2_SOURCE_DIR AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/android/app/jni/SDL")
        set(SDL2_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android/app/jni/SDL")
    endif()
    if(NOT SDL2_TTF_SOURCE_DIR AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/android/app/jni/SDL_ttf")
        set(SDL2_TTF_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android/app/jni/SDL_ttf")
    endif()
    if(NOT SDL2_MIXER_SOURCE_DIR AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/android/app/jni/SDL_mixer")
        set(SDL2_MIXER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android/app/jni/SDL_mixer")
    endif()

    # 1. SDL2
    if(SDL2_SOURCE_DIR AND EXISTS "${SDL2_SOURCE_DIR}/CMakeLists.txt")
        if(NOT TARGET SDL2)
            set(SDL_TEST OFF CACHE BOOL "" FORCE)
            add_subdirectory("${SDL2_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/_deps/SDL2" EXCLUDE_FROM_ALL)
        endif()
        if(TARGET SDL2::SDL2)
            list(APPEND GAMEENGINE_SDL_LIBRARIES SDL2::SDL2)
        elseif(TARGET SDL2)
            list(APPEND GAMEENGINE_SDL_LIBRARIES SDL2)
        endif()
        if(EXISTS "${SDL2_SOURCE_DIR}/include")
            list(APPEND GAMEENGINE_SDL_INCLUDE_DIRS "${SDL2_SOURCE_DIR}/include")
            set(PRIMARY_SDL2_INC_DIR "${SDL2_SOURCE_DIR}/include")
        endif()
    else()
        find_package(SDL2 QUIET)
        if(TARGET SDL2::SDL2)
            list(APPEND GAMEENGINE_SDL_LIBRARIES SDL2::SDL2)
            get_target_property(_sdl2_inc SDL2::SDL2 INTERFACE_INCLUDE_DIRECTORIES)
            if(_sdl2_inc)
                list(APPEND GAMEENGINE_SDL_INCLUDE_DIRS ${_sdl2_inc})
                list(GET _sdl2_inc 0 PRIMARY_SDL2_INC_DIR)
            endif()
        elseif(SDL2_FOUND AND SDL2_LIBRARIES)
            list(APPEND GAMEENGINE_SDL_LIBRARIES ${SDL2_LIBRARIES})
            list(APPEND GAMEENGINE_SDL_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS})
            if(SDL2_INCLUDE_DIRS)
                list(GET SDL2_INCLUDE_DIRS 0 PRIMARY_SDL2_INC_DIR)
            endif()
        else()
            find_package(PkgConfig QUIET)
            if(PkgConfig_FOUND)
                pkg_check_modules(SDL2 QUIET sdl2)
            endif()
            if(SDL2_FOUND)
                list(APPEND GAMEENGINE_SDL_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS})
                list(APPEND GAMEENGINE_SDL_LIBRARY_DIRS ${SDL2_LIBRARY_DIRS})
                list(APPEND GAMEENGINE_SDL_LIBRARIES ${SDL2_LIBRARIES})
                list(GET SDL2_INCLUDE_DIRS 0 PRIMARY_SDL2_INC_DIR)
            else()
                find_path(SDL2_INCLUDE_DIR NAMES SDL.h PATH_SUFFIXES SDL2)
                find_library(SDL2_LIBRARY NAMES SDL2)
                if(SDL2_INCLUDE_DIR)
                    list(APPEND GAMEENGINE_SDL_INCLUDE_DIRS "${SDL2_INCLUDE_DIR}")
                    set(PRIMARY_SDL2_INC_DIR "${SDL2_INCLUDE_DIR}")
                endif()
                if(SDL2_LIBRARY)
                    list(APPEND GAMEENGINE_SDL_LIBRARIES "${SDL2_LIBRARY}")
                else()
                    list(APPEND GAMEENGINE_SDL_LIBRARIES SDL2)
                endif()
            endif()
        endif()
    endif()

    # 2. SDL2_ttf
    if(SDL2_TTF_SOURCE_DIR AND EXISTS "${SDL2_TTF_SOURCE_DIR}/CMakeLists.txt")
        if(NOT TARGET SDL2_ttf)
            set(SDL2TTF_SAMPLES OFF CACHE BOOL "" FORCE)
            add_subdirectory("${SDL2_TTF_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/_deps/SDL2_ttf" EXCLUDE_FROM_ALL)
        endif()
        if(TARGET SDL2_ttf::SDL2_ttf)
            list(APPEND GAMEENGINE_SDL_LIBRARIES SDL2_ttf::SDL2_ttf)
        elseif(TARGET SDL2_ttf)
            list(APPEND GAMEENGINE_SDL_LIBRARIES SDL2_ttf)
        endif()
        list(APPEND GAMEENGINE_SDL_INCLUDE_DIRS "${SDL2_TTF_SOURCE_DIR}")
        set(PRIMARY_SDL2_TTF_INC_DIR "${SDL2_TTF_SOURCE_DIR}")
    else()
        find_package(SDL2_ttf QUIET)
        if(TARGET SDL2_ttf::SDL2_ttf)
            list(APPEND GAMEENGINE_SDL_LIBRARIES SDL2_ttf::SDL2_ttf)
            get_target_property(_ttf_inc SDL2_ttf::SDL2_ttf INTERFACE_INCLUDE_DIRECTORIES)
            if(_ttf_inc)
                list(APPEND GAMEENGINE_SDL_INCLUDE_DIRS ${_ttf_inc})
                list(GET _ttf_inc 0 PRIMARY_SDL2_TTF_INC_DIR)
            endif()
        elseif(SDL2_TTF_FOUND AND SDL2_TTF_LIBRARIES)
            list(APPEND GAMEENGINE_SDL_LIBRARIES ${SDL2_TTF_LIBRARIES})
            list(APPEND GAMEENGINE_SDL_INCLUDE_DIRS ${SDL2_TTF_INCLUDE_DIRS})
            if(SDL2_TTF_INCLUDE_DIRS)
                list(GET SDL2_TTF_INCLUDE_DIRS 0 PRIMARY_SDL2_TTF_INC_DIR)
            endif()
        else()
            if(PkgConfig_FOUND)
                pkg_check_modules(SDL2_TTF QUIET SDL2_ttf)
            endif()
            if(SDL2_TTF_FOUND)
                list(APPEND GAMEENGINE_SDL_INCLUDE_DIRS ${SDL2_TTF_INCLUDE_DIRS})
                list(APPEND GAMEENGINE_SDL_LIBRARY_DIRS ${SDL2_TTF_LIBRARY_DIRS})
                list(APPEND GAMEENGINE_SDL_LIBRARIES ${SDL2_TTF_LIBRARIES})
                list(GET SDL2_TTF_INCLUDE_DIRS 0 PRIMARY_SDL2_TTF_INC_DIR)
            else()
                find_path(SDL2_TTF_INCLUDE_DIR NAMES SDL_ttf.h PATH_SUFFIXES SDL2)
                find_library(SDL2_TTF_LIBRARY NAMES SDL2_ttf)
                if(SDL2_TTF_INCLUDE_DIR)
                    list(APPEND GAMEENGINE_SDL_INCLUDE_DIRS "${SDL2_TTF_INCLUDE_DIR}")
                    set(PRIMARY_SDL2_TTF_INC_DIR "${SDL2_TTF_INCLUDE_DIR}")
                endif()
                if(SDL2_TTF_LIBRARY)
                    list(APPEND GAMEENGINE_SDL_LIBRARIES "${SDL2_TTF_LIBRARY}")
                else()
                    list(APPEND GAMEENGINE_SDL_LIBRARIES SDL2_ttf)
                endif()
            endif()
        endif()
    endif()

    # 3. SDL2_mixer
    if(SDL2_MIXER_SOURCE_DIR AND EXISTS "${SDL2_MIXER_SOURCE_DIR}/CMakeLists.txt")
        if(NOT TARGET SDL2_mixer)
            set(SDL2MIXER_SAMPLES OFF CACHE BOOL "" FORCE)
            add_subdirectory("${SDL2_MIXER_SOURCE_DIR}" "${CMAKE_BINARY_DIR}/_deps/SDL2_mixer" EXCLUDE_FROM_ALL)
        endif()
        if(TARGET SDL2_mixer::SDL2_mixer)
            list(APPEND GAMEENGINE_SDL_LIBRARIES SDL2_mixer::SDL2_mixer)
        elseif(TARGET SDL2_mixer)
            list(APPEND GAMEENGINE_SDL_LIBRARIES SDL2_mixer)
        endif()
        if(EXISTS "${SDL2_MIXER_SOURCE_DIR}/include")
            list(APPEND GAMEENGINE_SDL_INCLUDE_DIRS "${SDL2_MIXER_SOURCE_DIR}/include")
            set(PRIMARY_SDL2_MIXER_INC_DIR "${SDL2_MIXER_SOURCE_DIR}/include")
        else()
            list(APPEND GAMEENGINE_SDL_INCLUDE_DIRS "${SDL2_MIXER_SOURCE_DIR}")
            set(PRIMARY_SDL2_MIXER_INC_DIR "${SDL2_MIXER_SOURCE_DIR}")
        endif()
    else()
        find_package(SDL2_mixer QUIET)
        if(TARGET SDL2_mixer::SDL2_mixer)
            list(APPEND GAMEENGINE_SDL_LIBRARIES SDL2_mixer::SDL2_mixer)
            get_target_property(_mix_inc SDL2_mixer::SDL2_mixer INTERFACE_INCLUDE_DIRECTORIES)
            if(_mix_inc)
                list(APPEND GAMEENGINE_SDL_INCLUDE_DIRS ${_mix_inc})
                list(GET _mix_inc 0 PRIMARY_SDL2_MIXER_INC_DIR)
            endif()
        elseif(SDL2_MIXER_FOUND AND SDL2_MIXER_LIBRARIES)
            list(APPEND GAMEENGINE_SDL_LIBRARIES ${SDL2_MIXER_LIBRARIES})
            list(APPEND GAMEENGINE_SDL_INCLUDE_DIRS ${SDL2_MIXER_INCLUDE_DIRS})
            if(SDL2_MIXER_INCLUDE_DIRS)
                list(GET SDL2_MIXER_INCLUDE_DIRS 0 PRIMARY_SDL2_MIXER_INC_DIR)
            endif()
        else()
            if(PkgConfig_FOUND)
                pkg_check_modules(SDL2_MIXER QUIET SDL2_mixer)
            endif()
            if(SDL2_MIXER_FOUND)
                list(APPEND GAMEENGINE_SDL_INCLUDE_DIRS ${SDL2_MIXER_INCLUDE_DIRS})
                list(APPEND GAMEENGINE_SDL_LIBRARY_DIRS ${SDL2_MIXER_LIBRARY_DIRS})
                list(APPEND GAMEENGINE_SDL_LIBRARIES ${SDL2_MIXER_LIBRARIES})
                list(GET SDL2_MIXER_INCLUDE_DIRS 0 PRIMARY_SDL2_MIXER_INC_DIR)
            else()
                find_path(SDL2_MIXER_INCLUDE_DIR NAMES SDL_mixer.h PATH_SUFFIXES SDL2)
                find_library(SDL2_MIXER_LIBRARY NAMES SDL2_mixer)
                if(SDL2_MIXER_INCLUDE_DIR)
                    list(APPEND GAMEENGINE_SDL_INCLUDE_DIRS "${SDL2_MIXER_INCLUDE_DIR}")
                    set(PRIMARY_SDL2_MIXER_INC_DIR "${SDL2_MIXER_INCLUDE_DIR}")
                endif()
                if(SDL2_MIXER_LIBRARY)
                    list(APPEND GAMEENGINE_SDL_LIBRARIES "${SDL2_MIXER_LIBRARY}")
                else()
                    list(APPEND GAMEENGINE_SDL_LIBRARIES SDL2_mixer)
                endif()
            endif()
        endif()
    endif()

    # Populate sdl2_compat_include/SDL so <SDL/SDL.h>, <SDL/SDL_ttf.h>, <SDL/SDL_mixer.h>
    # resolve even when SDL2, SDL2_ttf, and SDL2_mixer headers live in separate directories.
    file(MAKE_DIRECTORY "${SDL_COMPAT_INCLUDE_DIR}/SDL")
    foreach(_inc_dir "${PRIMARY_SDL2_INC_DIR}" "${PRIMARY_SDL2_TTF_INC_DIR}" "${PRIMARY_SDL2_MIXER_INC_DIR}")
        if(_inc_dir AND EXISTS "${_inc_dir}")
            file(GLOB _sdl_headers "${_inc_dir}/*.h")
            foreach(_hdr ${_sdl_headers})
                get_filename_component(_hdr_name "${_hdr}" NAME)
                if(NOT EXISTS "${SDL_COMPAT_INCLUDE_DIR}/SDL/${_hdr_name}")
                    file(CREATE_LINK "${_hdr}" "${SDL_COMPAT_INCLUDE_DIR}/SDL/${_hdr_name}" SYMBOLIC)
                endif()
            endforeach()
        endif()
    endforeach()
else()
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(glm QUIET)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

    set(GAMEENGINE_GL_INCLUDE_DIRS ${GLEW_INCLUDE_DIRS})
    set(GAMEENGINE_GL_LIBRARIES OpenGL::GL GLEW::GLEW)
    set(GAMEENGINE_SDL_INCLUDE_DIRS
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
    )
    set(GAMEENGINE_SDL_LIBRARY_DIRS
        ${SDL2_LIBRARY_DIRS}
        ${SDL2_TTF_LIBRARY_DIRS}
        ${SDL2_MIXER_LIBRARY_DIRS}
    )
    set(GAMEENGINE_SDL_LIBRARIES
        ${SDL2_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
    )

    # The codebase uses #include <SDL/SDL.h>, <SDL/SDL_ttf.h>, <SDL/SDL_mixer.h>.
    # On Linux distros (e.g. Fedora / Bazzite), SDL2 headers are installed under
    # /usr/include/SDL2. Create an include/SDL symlink in the build directory so
    # both <SDL/SDL.h> and <SDL.h> resolve cleanly against SDL2.
    list(GET SDL2_INCLUDE_DIRS 0 PRIMARY_SDL2_INC_DIR)
    if(NOT EXISTS "${SDL_COMPAT_INCLUDE_DIR}/SDL")
        file(CREATE_LINK "${PRIMARY_SDL2_INC_DIR}" "${SDL_COMPAT_INCLUDE_DIR}/SDL" SYMBOLIC)
    endif()

    # Optional CEGUI detection for GUI.cpp and NinjaPlatformer
    # Supports both Ubuntu/Debian (libcegui-mk2-dev -> CEGUI-0 / cegui-0.8.7)
    # and Fedora (cegui-devel -> CEGUI-0 / cegui-0)
    if(CMAKE_DISABLE_FIND_PACKAGE_CEGUI OR NOT ENABLE_CEGUI OR (DEFINED CEGUI_FOUND AND NOT CEGUI_FOUND))
        set(CEGUI_FOUND FALSE)
    else()
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
    endif()
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
    "${ENGINE_DIR}/VirtualControls.cpp"
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
    ${GAMEENGINE_SDL_INCLUDE_DIRS}
    ${GAMEENGINE_GL_INCLUDE_DIRS}
)

if(DEFINED GLM_INCLUDE_DIR AND EXISTS "${GLM_INCLUDE_DIR}")
    target_include_directories(GameEngine PUBLIC "${GLM_INCLUDE_DIR}")
elseif(TARGET glm::glm)
    target_link_libraries(GameEngine PUBLIC glm::glm)
endif()

if(CEGUI_FOUND)
    target_compile_definitions(GameEngine PUBLIC GAMEENGINE_HAS_CEGUI)
    target_include_directories(GameEngine PUBLIC ${CEGUI_INCLUDE_DIRS})
    if(CEGUI_LIBRARY_DIRS)
        target_link_directories(GameEngine PUBLIC ${CEGUI_LIBRARY_DIRS})
    endif()
    target_link_libraries(GameEngine PUBLIC ${CEGUI_LIBRARIES})
endif()

if(GAMEENGINE_SDL_LIBRARY_DIRS)
    target_link_directories(GameEngine PUBLIC ${GAMEENGINE_SDL_LIBRARY_DIRS})
endif()

target_link_libraries(GameEngine PUBLIC
    ${GAMEENGINE_GL_LIBRARIES}
    ${GAMEENGINE_SDL_LIBRARIES}
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

# Helper to define either a desktop executable or the selected Android libmain.so
function(add_game_target TARGET_NAME SOURCE_GAME_DIR)
    cmake_parse_arguments(GAME "" "" "SOURCES;ASSETS;EXTRA_LIBS" ${ARGN})
    if(NOT ANDROID)
        add_executable(${TARGET_NAME} ${GAME_SOURCES})
        target_include_directories(${TARGET_NAME} PRIVATE "${SOURCE_GAME_DIR}")
        target_link_libraries(${TARGET_NAME} PRIVATE GameEngine ${GAME_EXTRA_LIBS})
        link_game_assets(${TARGET_NAME} "${SOURCE_GAME_DIR}" ${GAME_ASSETS})
    elseif(ANDROID_GAME_TARGET STREQUAL "${TARGET_NAME}")
        add_library(main SHARED ${GAME_SOURCES})
        target_include_directories(main PRIVATE "${SOURCE_GAME_DIR}")
        target_link_libraries(main PRIVATE GameEngine ${GAME_EXTRA_LIBS})
        if(TARGET SDL2::SDL2main)
            target_link_libraries(main PRIVATE SDL2::SDL2main)
        elseif(TARGET SDL2main)
            target_link_libraries(main PRIVATE SDL2main)
        endif()
    endif()
endfunction()

if(ANDROID)
    if(NOT ANDROID_GAME_TARGET MATCHES "^(TutorialGame|ZombieGame|BallGame|NinjaPlatformer)$")
        message(FATAL_ERROR "Invalid ANDROID_GAME_TARGET='${ANDROID_GAME_TARGET}'. Expected TutorialGame, ZombieGame, BallGame, or NinjaPlatformer.")
    endif()
endif()

# -----------------------------------------------------------------------------
# 1. TutorialGame (Tutorials 1 - 17)
# -----------------------------------------------------------------------------
set(TUTORIAL_GAME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/TutorialGame")
add_game_target(TutorialGame "${TUTORIAL_GAME_DIR}"
    SOURCES
        "${TUTORIAL_GAME_DIR}/MainGame.cpp"
        "${TUTORIAL_GAME_DIR}/Bullet.cpp"
        "${TUTORIAL_GAME_DIR}/main.cpp"
    ASSETS Shaders Textures
)

# -----------------------------------------------------------------------------
# 2. ZombieGame (Tutorials 18 - 38)
# -----------------------------------------------------------------------------
set(ZOMBIE_GAME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/ZombieGame")
add_game_target(ZombieGame "${ZOMBIE_GAME_DIR}"
    SOURCES
        "${ZOMBIE_GAME_DIR}/Agent.cpp"
        "${ZOMBIE_GAME_DIR}/Bullet.cpp"
        "${ZOMBIE_GAME_DIR}/Gun.cpp"
        "${ZOMBIE_GAME_DIR}/Human.cpp"
        "${ZOMBIE_GAME_DIR}/Level.cpp"
        "${ZOMBIE_GAME_DIR}/MainGame.cpp"
        "${ZOMBIE_GAME_DIR}/Player.cpp"
        "${ZOMBIE_GAME_DIR}/Zombie.cpp"
        "${ZOMBIE_GAME_DIR}/main.cpp"
    ASSETS Shaders Textures Audio Fonts Levels
)

# -----------------------------------------------------------------------------
# 3. BallGame (Tutorials 39 - 43)
# -----------------------------------------------------------------------------
set(BALL_GAME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/BallGame")
add_game_target(BallGame "${BALL_GAME_DIR}"
    SOURCES
        "${BALL_GAME_DIR}/Ball.cpp"
        "${BALL_GAME_DIR}/BallController.cpp"
        "${BALL_GAME_DIR}/BallRenderer.cpp"
        "${BALL_GAME_DIR}/Grid.cpp"
        "${BALL_GAME_DIR}/MainGame.cpp"
        "${BALL_GAME_DIR}/main.cpp"
    ASSETS Shaders Textures Fonts
)

# -----------------------------------------------------------------------------
# 4. NinjaPlatformer (Tutorials 44 - 58)
# -----------------------------------------------------------------------------
set(NINJA_GAME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/GameEngine/NinjaPlatformer")
if((NOT ANDROID AND BUILD_NINJA_PLATFORMER) OR (ANDROID AND ANDROID_GAME_TARGET STREQUAL "NinjaPlatformer"))
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

    add_game_target(NinjaPlatformer "${NINJA_GAME_DIR}"
        SOURCES
            "${NINJA_GAME_DIR}/App.cpp"
            "${NINJA_GAME_DIR}/Box.cpp"
            "${NINJA_GAME_DIR}/Capsule.cpp"
            "${NINJA_GAME_DIR}/EditorScreen.cpp"
            "${NINJA_GAME_DIR}/GameplayScreen.cpp"
            "${NINJA_GAME_DIR}/LevelReaderWriter.cpp"
            "${NINJA_GAME_DIR}/MainMenuScreen.cpp"
            "${NINJA_GAME_DIR}/Player.cpp"
            "${NINJA_GAME_DIR}/main.cpp"
        EXTRA_LIBS Box2D_v231
        ASSETS Shaders Assets Fonts GUI Levels
    )
    if(NOT CEGUI_FOUND)
        message(STATUS "Building NinjaPlatformer without CEGUI-0 (using built-in SpriteBatch/SpriteFont UI fallback).")
    endif()
endif()

# -----------------------------------------------------------------------------
# 5. Automated Engine Tests (CTest)
# -----------------------------------------------------------------------------
if(NOT ANDROID)
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
        "InputManager_TouchAndPinch"
        "VirtualControls_DPadAndButtons"
        "ScreenList_Navigation"
        "IOManager_ReadFileToBuffer"
        "IOManager_ReadFileToBufferString"
    )

    if(TARGET Box2D_v231)
        list(APPEND TEST_CASES
            "LevelReaderWriter_RoundTripText"
            "LevelReaderWriter_RoundTripBinary"
            "LevelReaderWriter_GoalObjectRoundTrip"
            "LevelReaderWriter_LoadShippedLevel"
            "Shader_TextureShadingCompileAndLink"
            "Shader_LightShadingCompileAndLink"
            "Shader_ColorCompileAndLink"
            "Shader_GLES300Preprocess"
            "SpriteBatch_RepeatedInitAndDispose"
            "SpriteFont_InitAndMeasure"
            "SpriteFont_TextureAtlasNoSkewRegression"
        )
    endif()

    foreach(TC ${TEST_CASES})
        add_test(NAME "Engine.${TC}" COMMAND EngineTests "${TC}")
        set_tests_properties("Engine.${TC}" PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/EngineTests")
    endforeach()
endif()
