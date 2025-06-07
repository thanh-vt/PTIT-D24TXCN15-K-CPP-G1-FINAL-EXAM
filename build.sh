#!/bin/bash
# ===================================================================
# E-Wallet Management System - Build Script (Linux/macOS)
# ===================================================================

echo "========================================"
echo "E-Wallet Management System Build Script"
echo "========================================"

# Check if CMake is installed
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake is not installed"
    echo "Please install CMake:"
    echo "  Ubuntu/Debian: sudo apt-get install cmake"
    echo "  macOS: brew install cmake"
    exit 1
fi

# Check if g++ is installed
if ! command -v g++ &> /dev/null; then
    echo "ERROR: g++ is not installed"
    echo "Please install g++:"
    echo "  Ubuntu/Debian: sudo apt-get install build-essential"
    echo "  macOS: xcode-select --install"
    exit 1
fi

echo
echo "Step 1: Cleaning previous build..."
rm -rf build
mkdir build

echo
echo "Step 2: Configuring project with CMake..."
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed"
    cd ..
    exit 1
fi

echo
echo "Step 3: Building project..."
cmake --build . --config Debug
if [ $? -ne 0 ]; then
    echo "ERROR: Build failed"
    cd ..
    exit 1
fi

echo
echo "Step 4: Checking build output..."
if [ -f "ewallet-manager" ]; then
    echo "SUCCESS: Build completed successfully!"
    echo
    echo "Executable location: build/ewallet-manager"
    echo "File size: $(ls -lh ewallet-manager | awk '{print $5}')"
    echo
    echo "Testing executable..."
    ./ewallet-manager --help
    echo
    echo "========================================"
    echo "Build completed successfully!"
    echo "========================================"
    echo
    echo "To run the application:"
    echo "  cd build"
    echo "  ./ewallet-manager"
    echo
    echo "To run with custom database path:"
    echo "  ./ewallet-manager --db-path ../test"
else
    echo "ERROR: Executable not found after build"
    cd ..
    exit 1
fi

cd ..
echo
echo "Build script completed."
