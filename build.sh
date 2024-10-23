#!/bin/bash

cd "$(dirname "$0")"

build_type=$(echo "$1" | tr -d '[:space:]' | tr '[:upper:]' '[:lower:]')

if [[ "$build_type" == "" || "$build_type" == "r" || "$build_type" == "release" ]]; then
    build_type="release"
elif [[ "$build_type" == "d" || "$build_type" == "debug" ]]; then
    build_type="debug"
else
    echo "Error: Invalid build type argument."
    echo "Accepted values: 'r' or 'release' for Release mode, 'd' or 'debug' for Debug mode."
    echo "Usage: bash [script_name].sh [build_type]"
    exit 1
fi

ARCHITECTURE=$(uname -m)

if [[ "$ARCHITECTURE" == "x86_64" ]]; then
    echo "Building x64-$build_type"
    cmake --build out/build/x64-$build_type || { echo "CMake build failed!"; exit 1; }
elif [[ "$ARCHITECTURE" == "i386" || "$ARCHITECTURE" == "i686" ]]; then
    echo "Building x86-$build_type"
    cmake --build out/build/x86-$build_type || { echo "CMake build failed!"; exit 1; }
else
    echo "Unsupported architecture: $ARCHITECTURE"
    exit 1
fi

echo "CMake build completed successfully!"