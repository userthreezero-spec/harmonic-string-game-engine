# HSE-020 — Implementation Results

## Summary
HSE has been evolved from a passive renderer into an interactive 3D workspace. The engine now supports mouse/keyboard input queries, manual camera navigation (Orbit/Pan/Zoom), transform-aware object picking, and interactive saving.

## Subsystem Changes

### 1. Window & Input
- **Extended `hse::Window`**: Added `isMouseButtonPressed`, `isKeyPressed`, `getMousePosition`, and `getScrollY`.
- **GLFW Integration**: Implemented scroll and framebuffer callbacks.

### 2. Camera Navigation
- **Unified Orbit Model**: Extended HSE-015 orbit system with `offsetOrbit`, `pan`, and `zoom`.
- **Manual Control**: `Camera` now computes its position from yaw, pitch, and radius, allowing both automated and manual updates.

### 3. Picking & Selection
- **Ray-Casting**: Implemented `hse::Picker` and `hse::Ray`.
- **Transform Awareness**: Rays are transformed into local object space (inverse world matrix) for accurate intersection with rotated/scaled primitives.
- **Visual Feedback**: Added `uSelected` uniform to shaders to highlight the active object.

### 4. Workspace Application
- **Interaction Loop**: Implemented in `workspace_main.cpp`.
- **Manipulation**: Mapped arrow keys, PageUp/Down, Home/End, and +/- to object transforms.
- **Persistence**: Mapped 'S' key to project save.

## Files Modified/Created
- `include/core/window.h`, `src/core/window.cpp`
- `include/scene/camera.h`, `src/scene/camera.cpp`
- `include/math/vec3.h`, `src/math/vec3.cpp` (Added operators)
- `include/math/mat4.h`, `src/math/mat4.cpp` (Added inverse)
- `include/math/ray.h` (New)
- `include/scene/picker.h`, `src/scene/picker.cpp` (New)
- `include/renderer/renderer.h`, `src/renderer/renderer.cpp`
- `src/workspace_main.cpp`
- `CMakeLists.txt`
- `tests/test_input.cpp` (New)
- `tests/test_picking.cpp` (New)
