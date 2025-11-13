# Simple wrapper Makefile for CMake-based build

.PHONY: all configure build run test clean

# Default: configure & build everything
all: build

# Step 1: run CMake configure into ./build
configure:
	mkdir -p build
	cmake -S . -B build

# Step 2: build via CMake
build: configure
	cmake --build build

# Optional: run the ncurses app
run: build
	./build/minesweeper

# Optional: run tests (uses CTest from the CMake build)
test: build
	cd build && ctest --output-on-failure

# Cleanup: remove CMake build dir
clean:
	rm -rf build