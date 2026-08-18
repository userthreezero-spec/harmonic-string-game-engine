@echo off
title HSE Live Bridge
echo ============================================
echo    HSE Live Bridge
echo ============================================
echo.
echo Pipe: \\.\pipe\hse_bridge
echo.
echo 1. Start HSE Bridge (this window)
echo 2. In another terminal: python bridge_client.py
echo 3. In another terminal: python bridge_client.py demo
echo.
echo Press any key to start the bridge...
pause >nul

set PATH=C:\msys64\mingw64\bin;C:\Program Files\CMake\bin;%PATH%
cd /d C:\Users\Rico\HarmonicStringEngine

if not exist build\hse_bridge_app.exe (
    echo Building bridge...
    cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
    cmake --build build --target hse_bridge_app -j8
)

build\hse_bridge_app.exe
