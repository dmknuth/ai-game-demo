#!/bin/bash
# Build test script for Ubuntu Linux
# This script tests that the project compiles successfully on Ubuntu

set -e  # Exit on error

echo "=== Space Wars Build Test for Ubuntu Linux ==="
echo ""

# Check if we're on Linux
if [[ "$OSTYPE" != "linux-gnu"* ]]; then
    echo "Warning: This script is intended for Linux systems"
    echo "Current OS: $OSTYPE"
fi

# Check for required tools
echo "Checking for required tools..."
command -v cmake >/dev/null 2>&1 || { echo "Error: cmake is not installed. Install with: sudo apt install cmake"; exit 1; }
command -v pkg-config >/dev/null 2>&1 || { echo "Error: pkg-config is not installed. Install with: sudo apt install pkg-config"; exit 1; }
command -v g++ >/dev/null 2>&1 || { echo "Error: g++ is not installed. Install with: sudo apt install build-essential"; exit 1; }

# Check compiler version (need C++23 support)
GCC_VERSION=$(g++ -dumpversion | cut -d. -f1)
if [ "$GCC_VERSION" -lt 13 ]; then
    echo "Warning: GCC version $GCC_VERSION may not fully support C++23"
    echo "GCC 13+ is recommended for full C++23 support"
fi

echo "✓ All required tools found"
echo ""

# Check for SFML
echo "Checking for SFML..."
if pkg-config --exists sfml-all 2>/dev/null || pkg-config --exists sfml-system 2>/dev/null; then
    SFML_VERSION=$(pkg-config --modversion sfml-all 2>/dev/null || pkg-config --modversion sfml-system 2>/dev/null)
    echo "✓ SFML found: version $SFML_VERSION"
else
    echo "Error: SFML not found. Install with: sudo apt install libsfml-dev"
    exit 1
fi

# Check for ZeroMQ
echo "Checking for ZeroMQ..."
if pkg-config --exists libzmq; then
    ZMQ_VERSION=$(pkg-config --modversion libzmq)
    echo "✓ ZeroMQ found: version $ZMQ_VERSION"
else
    echo "Error: ZeroMQ not found. Install with: sudo apt install libzmq3-dev"
    exit 1
fi

echo ""
echo "=== Starting Build Test ==="
echo ""

# Clean previous build if it exists
if [ -d "build" ]; then
    echo "Cleaning previous build..."
    rm -rf build
fi

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. || { echo "Error: CMake configuration failed"; exit 1; }

# Build the project
echo ""
echo "Building project..."
cmake --build . || { echo "Error: Build failed"; exit 1; }

# Check if executable was created
if [ -f "bin/SpaceWars" ]; then
    echo ""
    echo "✓ Build successful!"
    echo "✓ Executable created: build/bin/SpaceWars"
    echo ""
    echo "Build test completed successfully on Ubuntu Linux"
    exit 0
else
    echo ""
    echo "Error: Executable not found after build"
    exit 1
fi

