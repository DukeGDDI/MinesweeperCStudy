#!/usr/bin/env bash

set -e

# Ensure build directory exists
if [ ! -d "build" ]; then
    echo "No build directory found. Building first..."
    mkdir -p build
    cmake -S . -B build
    cmake --build build
fi

# Prefer ctest if available (runs all discovered tests)
if command -v ctest >/dev/null 2>&1; then
    echo "Running tests via ctest..."
    (cd build && ctest --output-on-failure)
else
    # Fallback to direct binary execution
    if [ -x "build/minesweeper_tests" ]; then
        echo "Running tests via minesweeper_tests binary..."
        ./build/minesweeper_tests
    else
        echo "Error: test binary not found!"
        exit 1
    fi
fi
