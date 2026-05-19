#!/bin/bash
set -e

# Change to the directory where this script is located
cd "$(dirname "$0")"

BUILD_TYPE="${1:-Debug}"
case "${BUILD_TYPE}" in
    Debug|Release|RelWithDebInfo|MinSizeRel) ;;
    *)
        echo "Error: invalid build type '${BUILD_TYPE}'."
        echo "Usage: $0 [Debug|Release|RelWithDebInfo|MinSizeRel]"
        exit 1
        ;;
esac

# Detect OS to set CMAKE_PREFIX_PATH
if [ "$(uname)" == "Darwin" ]; then
    if command -v brew &> /dev/null; then
        export CMAKE_PREFIX_PATH="$(brew --prefix qt)"
    else
        echo "Error: Homebrew not found. Ensure Qt6 is installed and CMAKE_PREFIX_PATH is set."
        exit 1
    fi
elif [ "$(uname)" == "Linux" ]; then
    if command -v dpkg-architecture &> /dev/null; then
        export CMAKE_PREFIX_PATH="/usr/lib/$(dpkg-architecture -qDEB_HOST_MULTIARCH)/cmake/Qt6"
    else
        # Fallback paths for Debian/Ubuntu
        export CMAKE_PREFIX_PATH="/usr/lib/aarch64-linux-gnu/cmake/Qt6:/usr/lib/x86_64-linux-gnu/cmake/Qt6"
    fi
fi

echo "Using CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}"

IFS=':' read -ra _qt_paths <<< "${CMAKE_PREFIX_PATH}"
_qt_found=0
for _path in "${_qt_paths[@]}"; do
    if [ -d "${_path}" ]; then
        _qt_found=1
        break
    fi
done

if [ "${_qt_found}" -ne 1 ]; then
    echo "Error: none of the CMAKE_PREFIX_PATH entries exist."
    echo "Value: ${CMAKE_PREFIX_PATH}"
    exit 1
fi

mkdir -p build
cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -DCMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH}"

# Build
CORES=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
cmake --build . -j${CORES}
