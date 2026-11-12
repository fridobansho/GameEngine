#!/usr/bin/env bash
# =============================================================================
# Helper script to run games inside Distrobox with a simple, short command
#
# Usage:
#   ./run.sh [game] [args...]
#
# Examples:
#   ./run.sh ninja
#   ./run.sh zombie
#   ./run.sh ball
#   ./run.sh tutorial
#   ./run.sh tests
#
# You can also use the direct wrappers:
#   ./run-ninja.sh
#   ./run-zombie.sh
#   ./run-ball.sh
#   ./run-tutorial.sh
#   ./run-tests.sh
# =============================================================================
set -euo pipefail

CONTAINER_NAME="${CONTAINER_NAME:-gamedev-ubuntu}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN_BASE="${SCRIPT_DIR}/build/bin"

# Determine target game from script name or first argument
INVOKED_AS="$(basename "$0")"
TARGET=""

if [[ "${INVOKED_AS}" =~ run-(ninja|zombie|ball|tutorial|tests?)\.sh ]]; then
    TARGET="${BASH_REMATCH[1]}"
elif [[ $# -gt 0 ]]; then
    TARGET="$1"
    shift
fi

# Normalize target name
TARGET_LOWER="$(echo "${TARGET}" | tr '[:upper:]' '[:lower:]')"

case "${TARGET_LOWER}" in
    1|tut|tutorial|tutorialgame)
        GAME_DIR="${BIN_BASE}/TutorialGame"
        GAME_BIN="TutorialGame"
        ;;
    2|zomb|zombie|zombiegame)
        GAME_DIR="${BIN_BASE}/ZombieGame"
        GAME_BIN="ZombieGame"
        ;;
    3|ball|ballgame)
        GAME_DIR="${BIN_BASE}/BallGame"
        GAME_BIN="BallGame"
        ;;
    4|ninja|ninjaplatformer)
        GAME_DIR="${BIN_BASE}/NinjaPlatformer"
        GAME_BIN="NinjaPlatformer"
        ;;
    5|test|tests|enginetests)
        GAME_DIR="${BIN_BASE}/EngineTests"
        GAME_BIN="EngineTests"
        ;;
    "")
        echo "=========================================="
        echo "         GameEngine Launcher              "
        echo "=========================================="
        echo "  1) TutorialGame"
        echo "  2) ZombieGame"
        echo "  3) BallGame"
        echo "  4) NinjaPlatformer"
        echo "  5) Automated Tests (EngineTests)"
        echo "  q) Quit"
        echo "=========================================="
        read -r -p "Enter choice [1-5]: " CHOICE
        case "${CHOICE}" in
            1) GAME_DIR="${BIN_BASE}/TutorialGame"; GAME_BIN="TutorialGame" ;;
            2) GAME_DIR="${BIN_BASE}/ZombieGame"; GAME_BIN="ZombieGame" ;;
            3) GAME_DIR="${BIN_BASE}/BallGame"; GAME_BIN="BallGame" ;;
            4) GAME_DIR="${BIN_BASE}/NinjaPlatformer"; GAME_BIN="NinjaPlatformer" ;;
            5) GAME_DIR="${BIN_BASE}/EngineTests"; GAME_BIN="EngineTests" ;;
            q|Q|*) exit 0 ;;
        esac
        ;;
    *)
        echo "Error: Unknown game '${TARGET}'." >&2
        echo "Available targets: ninja, zombie, ball, tutorial, tests" >&2
        exit 1
        ;;
esac

# Check if binary exists
if [[ ! -f "${GAME_DIR}/${GAME_BIN}" ]]; then
    echo "Error: Executable not found at: ${GAME_DIR}/${GAME_BIN}" >&2
    echo "Please build the project first by running: ./setup-and-build-bazzite.sh" >&2
    exit 1
fi

# Run directly if already inside a container, otherwise enter Distrobox
if [[ -n "${CONTAINER_ID:-}" ]] || [[ -f /run/.containerenv ]]; then
    cd "${GAME_DIR}"
    exec "./${GAME_BIN}" "$@"
else
    if ! command -v distrobox >/dev/null 2>&1; then
        echo "Error: 'distrobox' command not found." >&2
        exit 1
    fi
    exec distrobox enter "${CONTAINER_NAME}" -- bash -c 'cd "$1" && exec ./"$2" "${@:3}"' bash "${GAME_DIR}" "${GAME_BIN}" "$@"
fi
