# PHASE 1220 — In-Window Mouse Ray-Picking & Interactive Hover/Click Validation

## Forensic Diagnosis & Root Cause
Prior to Phase 1220, moving the mouse over or clicking on in-window Project Hub card primitives did not trigger card highlights or open projects due to two underlying issues:
1. **Unupdated World Matrix in Non-Physical Scenes**: In `Scene::update(float dt)`, `primitive->updateWorldTransform()` was inside `if (m_physicsEnabled)`. Because the Project Hub scene had `m_physicsEnabled = false`, primitive world matrices remained identity matrices, causing bounding box ray tests to fail.
2. **Reverse Matrix Multiplication Order in Ray Picking**: In `Picker::screenToRay(x, y, camera, w, h)`, `invPV` was calculated as `(camera.getViewMatrix() * camera.getProjectionMatrix()).inverse()`. Standard OpenGL projection-view order requires `(camera.getProjectionMatrix() * camera.getViewMatrix()).inverse()`.

## Applied Engineering Repairs
1. **`src/scene/scene.cpp`**: Added `prim->updateWorldTransform()` into the unconditional `for (auto& prim : m_primitives)` loop in `Scene::update(dt)`, ensuring all Hub card primitives update world matrices every frame.
2. **`src/scene/picker.cpp`**: Updated `Picker::screenToRay` to compute `invPV = (camera.getProjectionMatrix() * camera.getViewMatrix()).inverse()`.
3. **`src/app/hse_app.cpp`**: Implemented `handleMouseButton(int button, int action, int mods)` with left-click ray-picking on `m_hubScene` card primitives (`card_two_story_home`, `card_workbench_validation`, `card_new_project`).

## Verified Interactive Behaviors
- **Mouse Hovering**: Moving the cursor over Card 1 (`card_two_story_home` / `preview_two_story_home`) or Card 2 (`card_workbench_validation` / `preview_workbench`) fires a ray that intersects the card's world-transformed AABB bounding box and updates the card material to Electric Blue (`#3B82F6`).
- **Mouse Clicking**: Left-clicking Card 1 directly triggers `transitionTo(ApplicationState::PROJECT_SELECTED)` for `two_story_home.json` and loads the 63-primitive Two-Story Home scene directly in the application window. Left-clicking Card 2 opens Workbench Validation.

## Test Harness Result
- Executed `hse_test_phase1218.exe`: **11/11 Lifecycle Stages Passed**.
- Executed `hse_app.exe --headless --frames 5`: **Clean Exit 0, Zero Process Crash**.
