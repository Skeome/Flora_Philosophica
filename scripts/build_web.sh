#!/bin/bash
# Flora Philosophia - Web Build Helper Script
# Compiles the C++ application to WebAssembly using Emscripten.

set -e

# Configuration
BUILD_DIR="build-web"
CMAKE_TOOLCHAIN_FILE="${EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"

echo "========================================================="
echo "Building Flora Philosophia for Web Assembly (Emscripten)"
echo "========================================================="

# Check for EMSDK
if [ -z "$EMSDK" ]; then
    echo "Error: EMSDK environment variable is not set."
    echo "Please install Emscripten and source its environment first (e.g. source emsdk_env.sh)."
    exit 1
fi

if [ ! -f "$CMAKE_TOOLCHAIN_FILE" ]; then
    echo "Error: Emscripten CMake toolchain file not found at: $CMAKE_TOOLCHAIN_FILE"
    exit 1
fi

# Run CMake configure
echo "Configuring with Emscripten toolchain..."
cmake -B "$BUILD_DIR" -S . \
    -DCMAKE_TOOLCHAIN_FILE="$CMAKE_TOOLCHAIN_FILE" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=OFF

# Compile
echo "Compiling..."
cmake --build "$BUILD_DIR" --parallel $(nproc)

echo "========================================================="
echo "Build complete! Output located in: $BUILD_DIR"
echo "========================================================="
