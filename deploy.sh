#!/usr/bin/env bash
set -euo pipefail

usage() {
    echo "Usage: $0 <ssh_host> <remote_path> [build_dir]"
    echo ""
    echo "  ssh_host    SSH destination (e.g. user@192.168.1.100)"
    echo "  remote_path Remote mods directory (e.g. D:/SteamLibrary/steamapps/common/Okami/mods)"
    echo "  build_dir   Build directory (default: build/llvm-mingw-cross-debug)"
    exit 1
}

[[ $# -lt 2 ]] && usage

SSH_HOST="$1"
REMOTE_PATH="$2"
BUILD_DIR="${3:-build/llvm-mingw-cross-debug}"

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

# Build first
echo "Building..."
cmake --build "$BUILD_DIR" --target okami-apclient

# Install to a staging directory
STAGING="$(mktemp -d)"
trap 'rm -rf "$STAGING"' EXIT

cmake --install "$BUILD_DIR" --prefix "$STAGING"

# Deploy to remote
echo "Deploying to ${SSH_HOST}:${REMOTE_PATH}/ ..."
scp -r "$STAGING/apclient/" "${SSH_HOST}:${REMOTE_PATH}/"

echo "Done."
