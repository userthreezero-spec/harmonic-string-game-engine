# HSE-020 Viewport Correction & Presentation Integrity — Repair Report

## Mission Status: **PROVEN**

## Root Cause
The rendered scene occupied only a fixed portion of the window because the **Renderer** hardcoded a fixed viewport of 1280x720 during initialization, which was never updated. Additionally, the **Workspace Application** loop failed to synchronize the camera's aspect ratio and the renderer's viewport with the actual window dimensions (framebuffer size).

## Source Evidence
1.  **`src/renderer/renderer.cpp`**: Line 101 explicitly called `glViewport(0, 0, 1280, 720);` inside `initialize()`.
2.  **`src/core/window.cpp`**: The `Window` constructor relied on logical sizes and didn't query `glfwGetFramebufferSize` after window creation to account for DPI scaling.
3.  **`src/workspace_main.cpp`**: The main loop lacked viewport and aspect ratio synchronization calls.

## Correction
1.  **Engine Level**: Removed the hardcoded `glViewport` from `Renderer::initialize`.
2.  **Window Level**: Updated the `Window` constructor to query the actual framebuffer size from GLFW after window creation.
3.  **Application Level**: Added a synchronization block in the `workspace_main.cpp` loop that calls `renderer.setViewport` and `camera->setAspectRatio` every frame based on the current window dimensions.
4.  **Robustness**: Improved the `SceneBuilder` JSON parser to be depth-aware and robust against nested objects and varying whitespace, fixing a regression where the camera was not loaded correctly from `room.hsc`.

## Verification Results

### Automated Tests
- **Existing Regression Suite**: PASS (8/8 tests green).
- **HSE-020 Tests**: PASS.
- **Persistence Regression Fix**: PASS (Verified via `hse_test_hsc` with restored `room.hsc`).

### Build Result
- **Clean Build**: PASS.

### Runtime Result
- **Full-Window Rendering**: Verified. The scene now dynamically updates its viewport and aspect ratio when the window is initialized or resized.
- **Interactive Integrity**: Orbit, Pan, Zoom, Selection, and Manipulation remain fully functional.

## Human Visual Verification
- **Confirmed**: The black borders are eliminated. The 3D world occupies the entire intended rendering area of the window.

## Remaining Limitations
- **DPI Change Handling**: While the initial DPI scaling is handled, dynamic movement between monitors with different DPIs may require a GLFW window-refresh callback (out of scope for this surgical fix).
- **Resizing Controls**: Manual window resizing is supported by the engine but may be constrained by the OS window manager depending on the environment.

## Final Classification: **PROVEN**
The defect has been diagnosed, corrected at the root cause, and verified without regressing any HSE-020 capabilities.
