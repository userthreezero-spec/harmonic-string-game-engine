# HSE-002 DESKTOP INTEGRATION

**Date:** 2026-08-17
**Status:** COMPLETE

---

## Launcher

**File:** `C:\Users\Rico\HarmonicStringEngine\launch_hse.bat`

### Menu Options

```
[1] Open project folder    → Opens Explorer at HSE root
[2] Build (Release)        → cmake configure + build Release
[3] Build (Debug)          → cmake configure + build Debug
[4] Run tests              → ctest with output on failure
[5] Launch renderer        → Starts hse_main.exe
[6] Open documentation     → Opens README.md
[7] Git status             → Shows git status and recent commits
[8] Exit                   → Closes launcher
```

### Technical Details

- Sets PATH for MSYS2 and CMake
- Changes to HSE root directory
- Uses `start ""` for non-blocking launcher
- Color scheme: green on black (0A)

## Desktop Shortcut

**File:** `C:\Users\Rico\Desktop\Harmonic String Engine.lnk`

| Property | Value |
|----------|-------|
| Target | `C:\Users\Rico\HarmonicStringEngine\launch_hse.bat` |
| Working Directory | `C:\Users\Rico\HarmonicStringEngine` |
| Icon | shell32.dll,13 (folder icon) |
| Description | Harmonic String Engine - Rendering Foundation |

## User Workflow

1. Double-click "Harmonic String Engine" on desktop
2. Terminal window opens with menu
3. Select option (e.g., [5] to launch renderer)
4. OpenGL window appears with green rotating triangle
5. Press ESC or close window to exit

## Project Navigation

```
C:\Users\Rico\HarmonicStringEngine\
├── src\                    ← Source code (edit here)
│   ├── main.cpp
│   ├── core\window.cpp
│   ├── renderer\renderer.cpp
│   ├── scene\camera.cpp, primitive.cpp, scene.cpp
│   └── math\vec3.cpp, mat4.cpp
├── include\                ← Headers (edit here)
│   ├── core\window.h
│   ├── renderer\renderer.h
│   ├── scene\camera.h, primitive.h, scene.h
│   └── math\vec3.h, mat4.h
├── tests\                  ← Unit tests
├── assets\                 ← (empty, for future use)
├── build\                  ← Build output (auto-generated)
├── docs\                   ← (empty, for future use)
├── experiments\            ← HSE-001/HSE-002 artifacts
├── knowledge\              ← Project knowledge model
├── webos\                  ← WebOS integration files
├── launch_hse.bat          ← Desktop launcher
├── CMakeLists.txt          ← Build configuration
├── README.md               ← Project overview
├── ARCHITECTURE.md         ← Architecture docs
└── WEBOS_INTEGRATION.md    ← WebOS integration docs
```
