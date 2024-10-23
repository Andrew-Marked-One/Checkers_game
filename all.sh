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

bash configure.sh "$build_type"

bash build.sh "$build_type"

bash run.sh "$build_type"