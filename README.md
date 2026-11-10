# GameEngine
Following tutorials from https://www.youtube.com/user/makinggameswithben

## Projects Included
- **`GameEngine`** — Shared 2D OpenGL/SDL2 game engine static library
- **`TutorialGame`** — Tutorials 1–17 (Sprites, Shaders, Camera2D, SpriteBatch, Input)
- **`ZombieGame`** — Tutorials 18–38 (Top-down 2D zombie shooter with AI, levels, audio, fonts, particles)
- **`BallGame`** — Tutorials 39–43 (Spatial grid physics and multiple procedural ball renderers)
- **`NinjaPlatformer`** — Tutorials 44–58 (`IMainGame`/`IGameScreen` architecture, Box2D physics, 2D lights, CEGUI level editor)

## Building Natively on Bazzite Linux

Bazzite is an immutable distribution and includes **Distrobox** out of the box. You can build and run all games natively with full GPU and audio acceleration using `ubuntu-toolbox` (or `fedora-toolbox`) without modifying your host image.

### Option A: Automated One-Step Script (`ubuntu-toolbox`, Re-runnable)

The included [`setup-and-build-bazzite.sh`](setup-and-build-bazzite.sh) script automates:
1. Cloning the repository (or updating/using your existing checkout if already cloned).
2. Creating the `gamedev-ubuntu` Distrobox container (`quay.io/toolbx/ubuntu-toolbox:22.04` by default, which includes `libcegui-mk2-dev` in `universe` for `NinjaPlatformer`) if it doesn't already exist.
3. Installing any missing `apt` dependencies inside the container (skipping installation on subsequent runs).
4. Configuring and building all targets via CMake + Ninja.

```bash
chmod +x setup-and-build-bazzite.sh
./setup-and-build-bazzite.sh
```

You can also run the script standalone before cloning to let it clone `linux-port`, create the container, and build in one command:
```bash
curl -fsSL https://raw.githubusercontent.com/fridobansho/GameEngine/linux-port/setup-and-build-bazzite.sh | bash
```

### Option B: Manual Steps (`ubuntu-toolbox`)

```bash
# 1. Create and enter an Ubuntu toolbox container
distrobox create -n gamedev-ubuntu -i quay.io/toolbx/ubuntu-toolbox:22.04
distrobox enter gamedev-ubuntu

# 2. Install build dependencies inside the container
sudo apt-get update
sudo apt-get install -y software-properties-common
sudo add-apt-repository -y universe
sudo apt-get update
sudo apt-get install -y \
  build-essential cmake ninja-build git pkg-config \
  libsdl2-dev libsdl2-ttf-dev libsdl2-mixer-dev \
  libglew-dev libglm-dev libgl1-mesa-dev libcegui-mk2-dev

# 3. Configure and build with CMake
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Running the Games
From your host terminal (or inside `distrobox enter gamedev-ubuntu`):
```bash
distrobox enter gamedev-ubuntu -- bash -c 'cd build/bin/TutorialGame && ./TutorialGame'
distrobox enter gamedev-ubuntu -- bash -c 'cd build/bin/ZombieGame && ./ZombieGame'
distrobox enter gamedev-ubuntu -- bash -c 'cd build/bin/BallGame && ./BallGame'
distrobox enter gamedev-ubuntu -- bash -c 'cd build/bin/NinjaPlatformer && ./NinjaPlatformer'
```

