# HSE-020 — Symbol Plan

## 1. Input & Window

#### [MODIFY] `include/core/window.h` / `src/core/window.cpp`
-   `Window::isMouseButtonPressed(int button) const`
-   `Window::isKeyPressed(int key) const`
-   `Window::getMousePosition(double& x, double& y) const`
-   `Window::getScrollOffset() const`
-   Internal: Add GLFW callbacks for mouse button and scroll.

## 2. Camera Navigation

#### [MODIFY] `include/scene/camera.h` / `src/scene/camera.cpp`
-   `Camera::offsetOrbit(float yawDeg, float pitchDeg)`
-   `Camera::pan(float x, float y)`
-   `Camera::zoom(float amount)`
-   `Camera::m_yaw`, `m_pitch`, `m_orbitRadius` (Internal state)

## 3. Picking & Math

#### [NEW] `include/math/ray.h`
-   `hse::Ray` (struct: origin, direction)
-   `hse::Intersection` (struct: hit, distance, point)

#### [NEW] `include/scene/picker.h` / `src/scene/picker.cpp`
-   `hse::Picker::screenToRay(double x, double y, const Camera& camera, int width, int height)`
-   `hse::Picker::pick(const Ray& ray, const Scene& scene)`
-   Intersection tests for Cube, Quad, Triangle.

## 4. Visuals

#### [MODIFY] `src/renderer/renderer.cpp`
-   `fragmentShaderSource`: Add `uniform bool uSelected`.
-   `Renderer::renderScene`: Add logic to set `uSelected` based on a new `selectedID` parameter.

#### [MODIFY] `include/renderer/renderer.h`
-   Update `renderScene` signature to include `uint64_t selectedID = 0`.

## 5. Workspace Application

#### [MODIFY] `src/workspace_main.cpp`
-   Integrate input handling to call Camera nav methods.
-   Implement Mouse Click -> Picker -> Update `m_selectedID`.
-   Implement Keyboard -> Selected Object -> `setPosition/Rotation/Scale`.
-   Implement Key 'S' -> `SceneBuilder::exportHSC`.

## 6. Persistence

#### [MODIFY] `src/scene/scene_builder.cpp`
-   Ensure `importState` and `exportHSC` handle the full set of transform data (verified in HSE-019, but will re-verify).
