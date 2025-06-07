#!/bin/bash
# ===================================================================
# E-Wallet Management System - Clean Script (Linux/macOS)
# ===================================================================

echo "========================================"
echo "E-Wallet Management System Clean Script"
echo "========================================"

echo
echo "Cleaning build directory..."
if [ -d "build" ]; then
    rm -rf build
    echo "Build directory removed."
else
    echo "Build directory does not exist."
fi

echo
echo "Cleaning temporary files..."

# Clean object files
if ls *.o 1> /dev/null 2>&1; then
    rm -f *.o
    echo "Object files removed."
fi

# Clean executable files
if ls ewallet-manager 1> /dev/null 2>&1; then
    rm -f ewallet-manager
    echo "Executable files removed."
fi

# Clean test files
if [ -f "test_simple.cpp" ]; then
    rm -f test_simple.cpp
    echo "Test files removed."
fi

# Clean compile output files
if [ -f "compile_output.txt" ]; then
    rm -f compile_output.txt
    echo "Compile output files removed."
fi

# Clean core dumps
if ls core.* 1> /dev/null 2>&1; then
    rm -f core.*
    echo "Core dump files removed."
fi

# Clean backup files
if ls *~ 1> /dev/null 2>&1; then
    rm -f *~
    echo "Backup files removed."
fi

echo
echo "========================================"
echo "Clean completed successfully!"
echo "========================================"
echo
