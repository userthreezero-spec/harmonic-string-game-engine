# HSE-002 BUILD REPORT

**Date:** 2026-08-17
**Status:** SUCCESS

---

## Environment

| Component | Version | Source |
|-----------|---------|--------|
| OS | Windows (win32) | System |
| CMake | 4.4.2 | C:\Program Files\CMake\bin\cmake.exe |
| GCC/G++ | 16.1.0 | MSYS2 mingw-w64-x86_64-gcc |
| Make | 4.4.1 | MSYS2 mingw-w64-x86_64-make |
| Git | 2.53.0 | System PATH |
| GLFW | 3.4 | MSYS2 mingw-w64-x86_64-glfw |
| GLEW | 2.2.0 | MSYS2 mingw-w64-x86_64-glew |
| OpenGL | 1.3.301 (Vulkan) / opengl32.dll | System |

## Compiler Installation

1. MSYS2 installed via winget to `C:\msys64\`
2. MinGW toolchain installed via pacman: `mingw-w64-x86_64-gcc`, `mingw-w64-x86_64-make`
3. GLFW and GLEW installed via pacman: `mingw-w64-x86_64-glfw`, `mingw-w64-x86_64-glew`
4. PATH configured: `C:\msys64\mingw64\bin;C:\Program Files\CMake\bin`

## CMake Configuration

```
cmake -S . -B build -G "MinGW Makefiles"
```

**Result:** SUCCESS
- CXX compiler: GNU 16.1.0
- Found OpenGL: opengl32
- Found GLEW: C:/msys64/mingw64/lib/cmake/glew/glew-config.cmake
- Found Threads: TRUE

## Build

```
cmake --build build --config Release
```

**Result:** SUCCESS — all 4 targets built

| Target | Type | Size | Status |
|--------|------|------|--------|
| libhse_core.a | Static library | 304 KB | BUILT |
| hse_main.exe | Executable | 390 KB | BUILT |
| hse_test_math.exe | Test executable | 157 KB | BUILT |
| hse_test_scene.exe | Test executable | 352 KB | BUILT |

## Build Error Repaired

**Error:** `'void hse::framebufferSizeCallback(GLFWwindow*, int, int)' was declared 'extern' and later 'static'`

**Cause:** `window.h` declares the function as `friend` (extern linkage), but `window.cpp` defines it as `static` (internal linkage). GCC enforces this; MSVC does not.

**Fix:** Removed `static` keyword from `framebufferSizeCallback` in `src/core/window.cpp:8`.

**File:** `src/core/window.cpp`
**Change:** `static void framebufferSizeCallback(...)` → `void framebufferSizeCallback(...)`
**Lines changed:** 1

## Build Artifacts

```
build/
├── hse_main.exe          (390 KB)
├── hse_test_math.exe     (157 KB)
├── hse_test_scene.exe    (352 KB)
├── libhse_core.a         (304 KB)
├── CMakeCache.txt
├── CMakeFiles/
├── Makefile
└── cmake_install.cmake
```
