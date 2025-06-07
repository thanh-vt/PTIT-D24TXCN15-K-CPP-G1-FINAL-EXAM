@echo off
REM ===================================================================
REM E-Wallet Management System - Clean Script
REM ===================================================================

echo ========================================
echo E-Wallet Management System Clean Script
echo ========================================

echo Cleaning build directory...
if exist build (
    rmdir /s /q build
    echo Build directory removed.
) else (
    echo Build directory does not exist.
)

echo Cleaning temporary files...
if exist *.o (
    del *.o
    echo Object files removed.
)

if exist *.exe (
    del *.exe
    echo Executable files removed.
)

if exist test_simple.cpp (
    del test_simple.cpp
    echo Test files removed.
)

if exist compile_output.txt (
    del compile_output.txt
    echo Compile output files removed.
)

echo.
echo ========================================
echo Clean completed successfully!
echo ========================================
echo.
pause
