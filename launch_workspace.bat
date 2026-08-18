@echo off
title HSE Workspace
echo ============================================
echo    HSE Workspace Launcher
echo ============================================
echo.
echo Usage:
echo   Double-click .hsc file (after file association)
echo   Or run: launch_workspace.bat project.hsc
echo.

set PATH=C:\msys64\mingw64\bin;C:\Program Files\CMake\bin;%PATH%
cd /d C:\Users\Rico\HarmonicStringEngine

if not exist build\hse_workspace_app.exe (
    echo Building workspace...
    cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
    cmake --build build --target hse_workspace_app -j8
)

if "%~1"=="" (
    echo No project specified.
    echo.
    echo Available projects:
    if exist projects\ (
        dir /b projects\*.hsc 2>nul
    )
    echo.
    echo Create a project: .\build\hse_workspace_app.exe --create MyProject
    echo Open a project:   .\build\hse_workspace_app.exe projects\MyProject\MyProject.hsc
    echo.
    echo Starting with default scene...
    build\hse_workspace_app.exe
) else (
    echo Opening project: %~1
    build\hse_workspace_app.exe "%~1"
)
