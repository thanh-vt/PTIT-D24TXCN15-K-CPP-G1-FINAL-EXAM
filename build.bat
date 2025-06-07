@echo off
REM ===================================================================
REM E-Wallet Management System - Build Script
REM ===================================================================

echo ========================================
echo E-Wallet Management System Build Script
echo ========================================

REM Check if CMake is installed
cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: CMake is not installed or not in PATH
    echo Please install CMake from https://cmake.org/download/
    pause
    exit /b 1
)

REM Check if Visual Studio is available
where cl.exe >nul 2>&1
if %errorlevel% neq 0 (
    echo WARNING: Visual Studio compiler not found in PATH
    echo Trying to setup Visual Studio environment...
    
    REM Try to find and setup Visual Studio
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
    ) else (
        echo ERROR: Visual Studio not found
        echo Please install Visual Studio 2019 or 2022 with C++ support
        pause
        exit /b 1
    )
)

echo.
echo Step 1: Cleaning previous build...
if exist build (
    rmdir /s /q build
)
mkdir build

echo.
echo Step 2: Configuring project with CMake...
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
if %errorlevel% neq 0 (
    echo ERROR: CMake configuration failed
    cd ..
    pause
    exit /b 1
)

echo.
echo Step 3: Building project...
cmake --build . --config Debug
if %errorlevel% neq 0 (
    echo ERROR: Build failed
    cd ..
    pause
    exit /b 1
)

echo.
echo Step 4: Checking build output...
if exist "Debug\ewallet-manager.exe" (
    echo SUCCESS: Build completed successfully!
    echo.
    echo Executable location: build\Debug\ewallet-manager.exe
    echo File size: 
    dir "Debug\ewallet-manager.exe" | find "ewallet-manager.exe"
    echo.
    echo Testing executable...
    "Debug\ewallet-manager.exe" --help
    echo.
    echo ========================================
    echo Build completed successfully!
    echo ========================================
    echo.
    echo To run the application:
    echo   cd build
    echo   Debug\ewallet-manager.exe
    echo.
    echo To run with custom database path:
    echo   Debug\ewallet-manager.exe --db-path ..\test
) else (
    echo ERROR: Executable not found after build
    cd ..
    pause
    exit /b 1
)

cd ..
echo.
echo Press any key to exit...
pause >nul
