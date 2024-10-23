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

EXECUTABLE_NAME="checkers_game"

if [[ "$ARCHITECTURE" == "x86_64" ]]; then
    executable="out/build/x64-$build_type/$EXECUTABLE_NAME"
elif [[ "$ARCHITECTURE" == "i386" || "$ARCHITECTURE" == "i686" ]]; then
    executable="out/build/x86-$build_type/$EXECUTABLE_NAME"
else
    echo "Unsupported architecture: $ARCHITECTURE"
    exit 1
fi


if [[ ! -x "$executable" ]]; then
    echo "Error: Executable not found or not executable at $executable"
    exit 1
fi


"$executable"