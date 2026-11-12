#!/usr/bin/env bash
# =============================================================================
# Automated, idempotent setup & build script for Bazzite Linux (Distrobox)
# Uses ubuntu-toolbox by default and can be safely re-run at any time.
#
# Usage:
#   ./setup-and-build-bazzite.sh
#
# Optional environment overrides:
#   CONTAINER_NAME=gamedev-ubuntu
#   CONTAINER_IMAGE=quay.io/toolbx/ubuntu-toolbox:22.04
#   REPO_URL=https://github.com/fridobansho/GameEngine.git
#   REPO_BRANCH=linux-port
#   REPO_DIR=$PWD/GameEngine
#   BUILD_TYPE=Release
# =============================================================================
set -euo pipefail

CONTAINER_NAME="${CONTAINER_NAME:-gamedev-ubuntu}"
# Default to ubuntu-toolbox:22.04 because it includes libcegui-mk2-dev in universe
# (required for NinjaPlatformer's level editor GUI).
CONTAINER_IMAGE="${CONTAINER_IMAGE:-quay.io/toolbx/ubuntu-toolbox:22.04}"
REPO_URL="${REPO_URL:-https://github.com/fridobansho/GameEngine.git}"
REPO_BRANCH="${REPO_BRANCH:-linux-port}"
BUILD_TYPE="${BUILD_TYPE:-Release}"

# -----------------------------------------------------------------------------
# Step 1: Detect or clone/update the repository (idempotent)
# -----------------------------------------------------------------------------
SCRIPT_SOURCE="${BASH_SOURCE[0]:-}"
if [[ -n "${SCRIPT_SOURCE}" ]]; then
    SCRIPT_DIR="$(cd "$(dirname "${SCRIPT_SOURCE}")" && pwd)"
else
    SCRIPT_DIR="${PWD}"
fi

if [[ -f "${SCRIPT_DIR}/CMakeLists.txt" && -d "${SCRIPT_DIR}/GameEngine" ]]; then
    REPO_DIR="${SCRIPT_DIR}"
    echo "==> Using existing repository checkout at: ${REPO_DIR}"
else
    REPO_DIR="${REPO_DIR:-${PWD}/GameEngine}"
    if [[ -d "${REPO_DIR}/.git" ]]; then
        echo "==> Repository already cloned at ${REPO_DIR}. Fetching latest changes..."
        git -C "${REPO_DIR}" fetch origin
        git -C "${REPO_DIR}" checkout "${REPO_BRANCH}"
        git -C "${REPO_DIR}" pull --ff-only origin "${REPO_BRANCH}" || true
    else
        echo "==> Cloning ${REPO_URL} (branch: ${REPO_BRANCH}) into ${REPO_DIR}..."
        git clone -b "${REPO_BRANCH}" "${REPO_URL}" "${REPO_DIR}"
    fi
fi

# -----------------------------------------------------------------------------
# Helper function executed INSIDE the container
# -----------------------------------------------------------------------------
run_inside_container() {
    cd "${REPO_DIR}"

    if command -v apt-get >/dev/null 2>&1; then
        APT_PKGS=(
            build-essential
            cmake
            ninja-build
            git
            pkg-config
            libsdl2-dev
            libsdl2-ttf-dev
            libsdl2-mixer-dev
            libglew-dev
            libglm-dev
            libgl1-mesa-dev
        )

        # Check if all base packages are already installed to make re-runs fast
        MISSING_PKGS=()
        for pkg in "${APT_PKGS[@]}"; do
            if ! dpkg -s "${pkg}" >/dev/null 2>&1; then
                MISSING_PKGS+=("${pkg}")
            fi
        done

        # Check if libcegui-mk2-dev is available and installed
        if ! dpkg -s libcegui-mk2-dev >/dev/null 2>&1; then
            MISSING_PKGS+=("libcegui-mk2-dev")
        fi

        if [[ ${#MISSING_PKGS[@]} -gt 0 ]]; then
            echo "==> Installing build dependencies via apt (${MISSING_PKGS[*]})..."
            sudo apt-get update
            # Ensure universe repository is enabled on Ubuntu for libcegui-mk2-dev & libglm-dev
            if ! dpkg -s software-properties-common >/dev/null 2>&1; then
                sudo apt-get install -y software-properties-common || true
            fi
            sudo add-apt-repository -y universe >/dev/null 2>&1 || true
            sudo apt-get update

            # Install core dependencies
            sudo apt-get install -y "${APT_PKGS[@]}"

            # Install CEGUI 0.8 dev package if available in this Ubuntu release
            if apt-cache show libcegui-mk2-dev >/dev/null 2>&1; then
                sudo apt-get install -y libcegui-mk2-dev
            else
                echo "==> Note: libcegui-mk2-dev is not in apt for this Ubuntu release; NinjaPlatformer will be skipped unless CEGUI-0 is installed."
            fi
        else
            echo "==> All Ubuntu apt build dependencies are already installed."
        fi
    elif command -v dnf >/dev/null 2>&1; then
        DNF_PKGS=(
            gcc-c++ cmake ninja-build git pkgconf-pkg-config
            SDL2-devel SDL2_ttf-devel SDL2_mixer-devel
            glew-devel glm-devel mesa-libGL-devel cegui-devel
        )
        if ! rpm -q "${DNF_PKGS[@]}" >/dev/null 2>&1; then
            echo "==> Installing build dependencies via dnf..."
            sudo dnf install -y "${DNF_PKGS[@]}"
        else
            echo "==> All Fedora dnf build dependencies are already installed."
        fi
    fi

    echo "==> Configuring CMake (${BUILD_TYPE})..."
    cmake -B build -G Ninja -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"

    echo "==> Building all targets..."
    cmake --build build

    echo "==> Running automated tests..."
    (cd build && ctest --output-on-failure)

    echo ""
    echo "============================================================================="
    echo "Build complete! Executables and linked assets are in: ${REPO_DIR}/build/bin/"
    echo ""
    echo "Quick run commands:"
    echo "  ./run-ninja.sh      (or ./run.sh ninja)"
    echo "  ./run-zombie.sh     (or ./run.sh zombie)"
    echo "  ./run-ball.sh       (or ./run.sh ball)"
    echo "  ./run-tutorial.sh   (or ./run.sh tutorial)"
    echo "  ./run-tests.sh      (or ./run.sh tests)"
    echo "============================================================================="
}

# -----------------------------------------------------------------------------
# Step 2: Check if we are already inside a container or on the Bazzite host
# -----------------------------------------------------------------------------
if [[ "${1:-}" == "--inside-container" ]] || [[ -n "${CONTAINER_ID:-}" ]]; then
    run_inside_container
    exit 0
fi

if ! command -v distrobox >/dev/null 2>&1; then
    echo "Error: 'distrobox' was not found on PATH." >&2
    exit 1
fi

# Create the Distrobox container only if it does not already exist
if distrobox list --no-color 2>/dev/null | awk -F'|' 'NR>1 {gsub(/^[ \t]+|[ \t]+$/, "", $2); print $2}' | grep -qx "${CONTAINER_NAME}"; then
    echo "==> Distrobox container '${CONTAINER_NAME}' already exists."
else
    echo "==> Creating Distrobox container '${CONTAINER_NAME}' using image '${CONTAINER_IMAGE}'..."
    distrobox create --yes --name "${CONTAINER_NAME}" --image "${CONTAINER_IMAGE}"
fi

# Re-invoke this script inside the Distrobox container
echo "==> Entering '${CONTAINER_NAME}' to install dependencies and build..."
distrobox enter "${CONTAINER_NAME}" -- bash "${REPO_DIR}/setup-and-build-bazzite.sh" --inside-container
