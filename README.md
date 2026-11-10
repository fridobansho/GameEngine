# GameEngine
Following tutorials from https://www.youtube.com/user/makinggameswithben

## Projects Included
- **`GameEngine`** — Shared 2D OpenGL/SDL2 game engine static library
- **`TutorialGame`** — Tutorials 1–17 (Sprites, Shaders, Camera2D, SpriteBatch, Input)
- **`ZombieGame`** — Tutorials 18–38 (Top-down 2D zombie shooter with AI, levels, audio, fonts, particles)
- **`BallGame`** — Tutorials 39–43 (Spatial grid physics and multiple procedural ball renderers)
- **`NinjaPlatformer`** — Tutorials 44–58 (`IMainGame`/`IGameScreen` architecture, Box2D physics, 2D lights, CEGUI level editor)

## Building Natively on Bazzite Linux

Bazzite is an immutable Fedora Atomic distribution and includes **Distrobox** out of the box. You can build and run all games natively with full GPU and audio acceleration using a Fedora toolbox container without modifying your host image:

### 1. Create and enter a Fedora dev container
```bash
distrobox create -n gamedev -i registry.fedoraproject.org/fedora-toolbox:latest
distrobox enter gamedev
```

### 2. Install build dependencies inside the container
```bash
sudo dnf install -y \
  gcc-c++ cmake ninja-build git pkgconf-pkg-config \
  SDL2-devel SDL2_ttf-devel SDL2_mixer-devel \
  glew-devel glm-devel mesa-libGL-devel cegui-devel
```

### 3. Configure and build with CMake
```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
*(Note: CMake automatically fetches Box2D v2.3.1 for `NinjaPlatformer` and symlinks each game's asset folders into `build/bin/<GameName>`.)*

### 4. Run the games
Run each executable from its directory in `build/bin/` so relative asset paths resolve properly:
```bash
(cd build/bin/TutorialGame && ./TutorialGame)
(cd build/bin/ZombieGame && ./ZombieGame)
(cd build/bin/BallGame && ./BallGame)
(cd build/bin/NinjaPlatformer && ./NinjaPlatformer)
```
