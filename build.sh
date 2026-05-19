#!/bin/bash
set -e

# Change to the directory where this script is located
cd "$(dirname "$0")"

# Build configuration (Debug by default, override with first arg)
BUILD_TYPE="${1:-Debug}"

# Allow caller to pre-set CMAKE_PREFIX_PATH; otherwise auto-detect.
if [ -z "${CMAKE_PREFIX_PATH}" ]; then
    if [ "$(uname)" == "Darwin" ]; then
        if command -v brew &> /dev/null && [ -d "$(brew --prefix qt 2>/dev/null)" ]; then
            export CMAKE_PREFIX_PATH="$(brew --prefix qt)"
        elif [ -d "$HOME/Qt" ]; then
            # Pick the latest Qt 6.x install under ~/Qt/<version>/macos
            CANDIDATE="$(ls -d "$HOME"/Qt/6.*/macos 2>/dev/null | sort -V | tail -n1)"
            if [ -n "$CANDIDATE" ]; then
                export CMAKE_PREFIX_PATH="$CANDIDATE"
            fi
        fi
    elif [ "$(uname)" == "Linux" ]; then
        if command -v dpkg-architecture &> /dev/null; then
            export CMAKE_PREFIX_PATH="/usr/lib/$(dpkg-architecture -qDEB_HOST_MULTIARCH)/cmake/Qt6"
        else
            export CMAKE_PREFIX_PATH="/usr/lib/aarch64-linux-gnu/cmake/Qt6:/usr/lib/x86_64-linux-gnu/cmake/Qt6"
        fi
    fi
fi

if [ -z "${CMAKE_PREFIX_PATH}" ]; then
    echo "Error: could not locate Qt6. Set CMAKE_PREFIX_PATH and re-run." >&2
    exit 1
fi

# Light sanity check on the prefix; CMAKE_PREFIX_PATH may be a list, so we only
# verify it isn't empty rather than that each entry exists.
echo "Using CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}"
echo "Using CMAKE_BUILD_TYPE: ${BUILD_TYPE}"

mkdir -p build
cd build

cmake .. \
    -DCMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH}" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"

CORES=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
cmake --build . -j${CORES}
