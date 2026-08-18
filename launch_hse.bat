@echo off
title Harmonic String Engine
color 0A
cls
echo.
echo  ========================================
echo   HARMONIC STRING ENGINE v0.1
echo   Rendering Foundation
echo  ========================================
echo.
echo  [1] Open project folder
echo  [2] Build (Release)
echo  [3] Build (Debug)
echo  [4] Run tests
echo  [5] Launch renderer
echo  [6] Open documentation
echo  [7] Git status
echo  [8] Exit
echo.
set /p choice="Select: "

if "%choice%"=="1" goto open_project
if "%choice%"=="2" goto build_release
if "%choice%"=="3" goto build_debug
if "%choice%"=="4" goto test
if "%choice%"=="5" goto launch
if "%choice%"=="6" goto docs
if "%choice%"=="7" goto git_status
if "%choice%"=="8" goto exit

echo Invalid selection.
pause
goto :eof

:open_project
explorer "C:\Users\Rico\HarmonicStringEngine"
goto :eof

:build_release
echo.
echo Building Release...
set PATH=C:\msys64\mingw64\bin;C:\Program Files\CMake\bin;%PATH%
cd /d C:\Users\Rico\HarmonicStringEngine
cmake -S . -B build -G "MinGW Makefiles" 2>&1
cmake --build build --config Release 2>&1
echo.
echo Build complete.
pause
goto :eof

:build_debug
echo.
echo Building Debug...
set PATH=C:\msys64\mingw64\bin;C:\Program Files\CMake\bin;%PATH%
cd /d C:\Users\Rico\HarmonicStringEngine
cmake -S . -B build -G "MinGW Makefiles" 2>&1
cmake --build build --config Debug 2>&1
echo.
echo Build complete.
pause
goto :eof

:test
echo.
echo Running tests...
set PATH=C:\msys64\mingw64\bin;C:\Program Files\CMake\bin;%PATH%
cd /d C:\Users\Rico\HarmonicStringEngine
ctest --test-dir build --output-on-failure
echo.
pause
goto :eof

:launch
echo.
echo Launching renderer...
echo Close the window or press ESC to exit.
start "" "C:\Users\Rico\HarmonicStringEngine\build\hse_main.exe"
goto :eof

:docs
start "" "C:\Users\Rico\HarmonicStringEngine\README.md"
goto :eof

:git_status
echo.
set PATH=C:\msys64\mingw64\bin;%PATH%
cd /d C:\Users\Rico\HarmonicStringEngine
git status
echo.
git log --oneline -5
echo.
pause
goto :eof

:exit
exit
