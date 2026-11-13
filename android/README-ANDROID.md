# Building for Android

This directory wraps the root `CMakeLists.txt` in a Gradle/NDK Android project using SDL2's `SDLActivity` and OpenGL ES 3.0.

All four games are configured as Gradle product flavors (`ninjaPlatformer`, `zombieGame`, `ballGame`, `tutorialGame`). Each flavor compiles the selected game as `libmain.so` via `-DANDROID_GAME_TARGET=<GameName>` and bundles only that game's runtime assets (`Shaders`, `Textures`, `Assets`, `Fonts`, `Audio`, `Levels`) into the APK.

## Prerequisites

1. **Android SDK & NDK** (API 34 SDK, NDK r25+ / CMake 3.22.1+, JDK 17).
2. **SDL2, SDL2_ttf, and SDL2_mixer sources** (with vendored/downloaded external deps such as FreeType for `SDL_ttf`).
3. **GLM headers** (`glm/glm.hpp`) — either installed in your sysroot/toolchain or pointed to via `GLM_INCLUDE_DIR`.
4. **Box2D v2.3.1** (required only for `ninjaPlatformer`) — fetched automatically by CMake `FetchContent` when online, or supplied offline via `BOX2D_SOURCE_DIR`.

## Supplying SDL2 Sources

You can supply the dependency paths in any of three ways:

1. **Place or symlink sources under `android/app/jni/`:**
   ```text
   android/app/jni/
     SDL/        # SDL2 source tree (must contain android-project/app/src/main/java)
     SDL_ttf/    # SDL2_ttf source tree
     SDL_mixer/  # SDL2_mixer source tree
     glm/        # (optional) directory containing glm/glm.hpp
     box2d/      # (optional) offline Box2D v2.3.1 checkout
   ```
2. **Or pass Gradle properties on the command line:**
   `-PSDL2_SOURCE_DIR=/path/to/SDL -PSDL2_TTF_SOURCE_DIR=/path/to/SDL_ttf -PSDL2_MIXER_SOURCE_DIR=/path/to/SDL_mixer -PGLM_INCLUDE_DIR=/path/to/glm`
3. **Or export environment variables** with the same names (`SDL2_SOURCE_DIR`, `SDL2_TTF_SOURCE_DIR`, `SDL2_MIXER_SOURCE_DIR`, `GLM_INCLUDE_DIR`, `BOX2D_SOURCE_DIR`).

Note: `android/app/build.gradle` automatically adds `${SDL2_SOURCE_DIR}/android-project/app/src/main/java` to the Java source sets so `org.libsdl.app.GameActivity` can extend `SDLActivity`.

## Building an APK

From the `android/` directory (using Gradle or a Gradle wrapper):

```bash
# NinjaPlatformer (default / platformer + virtual D-pad/jump/punch)
gradle assembleNinjaPlatformerDebug

# ZombieGame (twin-stick virtual controls)
gradle assembleZombieGameDebug

# BallGame (touch drag + pinch-to-zoom + HUD buttons)
gradle assembleBallGameDebug

# TutorialGame (touch-to-shoot + pinch-to-zoom + virtual D-pad)
gradle assembleTutorialGameDebug
```

If using `-P` paths explicitly:

```bash
gradle assembleNinjaPlatformerDebug \
  -PSDL2_SOURCE_DIR=/opt/android/SDL \
  -PSDL2_TTF_SOURCE_DIR=/opt/android/SDL_ttf \
  -PSDL2_MIXER_SOURCE_DIR=/opt/android/SDL_mixer \
  -PGLM_INCLUDE_DIR=/opt/android/glm/include
```

To install directly onto a connected device or emulator:

```bash
gradle installNinjaPlatformerDebug
```

## Notes

- `CEGUI` is automatically disabled on Android; `NinjaPlatformer` uses its built-in `SpriteBatch`/`SpriteFont` menu, editor toolbar, and exit button fallback.
- On Android, `IOManager::ReadFileToBuffer` and `LevelReaderWriter::loadAsText` / `loadAsBinary` read via `SDL_RWops`, which reads transparently from the APK's `assets/` folder.
- Desktop Linux builds (`cmake -B build && cmake --build build`) remain completely unaffected.
