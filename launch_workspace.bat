@echo off
title Harmonic String Game Engine Workspace
set PATH=C:\msys64\mingw64\bin;C:\Program Files\CMake\bin;%PATH%
cd /d C:\Users\Rico\HarmonicStringGameEngine

if not exist build\hse_app.exe (
    echo Building engine...
    cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
    cmake --build build --target hse_app -j8
)

start "" "C:\Users\Rico\HarmonicStringGameEngine\build\hse_app.exe" %*
