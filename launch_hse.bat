@echo off
title Harmonic String Game Engine
color 0A
set PATH=C:\msys64\mingw64\bin;C:\Program Files\CMake\bin;%PATH%
cd /d C:\Users\Rico\HarmonicStringGameEngine

if not exist build\hse_app.exe (
    echo Building Harmonic String Game Engine...
    cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
    cmake --build build --target hse_app -j8
)

echo Starting Harmonic String Game Engine...
start "" "C:\Users\Rico\HarmonicStringGameEngine\build\hse_app.exe" %*
