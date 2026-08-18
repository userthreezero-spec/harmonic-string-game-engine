# HSE-002 ENGINEERING PLAN

**Date:** 2026-08-17
**Cognitive Engine:** opencode/big-pickle
**Status:** EXECUTING

---

## Task A: Mechanical Accessor (Low Risk)

**Objective:** Add type/name accessors to Primitive, Scene, Camera

**Changes:**

| # | target_file | target_symbol | change | risk |
|---|------------|---------------|--------|------|
| A1 | `include/scene/primitive.h` | `Primitive` | Add `getType()` method | LOW |
| A2 | `include/scene/scene.h` | `Scene` | Add `getPrimitiveCount()` method | LOW |
| A3 | `include/scene/camera.h` | `Camera` | Add `getProjectionType()` method | LOW |

**Evidence:** All symbols verified in repository. No new dependencies.

---

## Task B: Architectural — Color Property (Medium Risk)

**Objective:** Add per-primitive color instead of hardcoded orange

**Changes:**

| # | target_file | target_symbol | change | risk |
|---|------------|---------------|--------|------|
| B1 | `include/scene/primitive.h` | `Primitive` | Add `m_color` member + accessors | LOW |
| B2 | `src/scene/primitive.cpp` | `Primitive` | Initialize `m_color` in constructor | LOW |
| B3 | `src/renderer/renderer.cpp` | `Renderer::renderScene` | Use `prim->getColor()` instead of hardcoded | MEDIUM |
| B4 | `src/main.cpp` | `main()` | Set triangle color explicitly | LOW |

**Existing Dependencies:** Vec3 (already used for position/rotation/scale). Color stored as Vec3 (RGB).

**Architectural Relationship:** Color is a property of Primitive, consumed by Renderer. This introduces a data flow change: Renderer now reads color from Primitive instead of using a constant.

---

## Task C: Rendering Subsystem — Colored Primitives (Medium Risk)

**Objective:** Extend shader to use per-vertex color or uniform color from primitive

**Changes:**

| # | target_file | target_symbol | change | risk |
|---|------------|---------------|--------|------|
| C1 | `src/renderer/renderer.cpp` | vertexShaderSource | No change (position-only) | N/A |
| C2 | `src/renderer/renderer.cpp` | fragmentShaderSource | Already uses `uColor` uniform | N/A |
| C3 | `src/renderer/renderer.cpp` | `renderScene` | Set uColor per primitive from `prim->getColor()` | LOW |

**Verification:** Color uniform already exists in fragment shader. Task B already passes color. Task C verifies the pipeline works end-to-end.

---

## Task D: Full 3D Scene — Rotation Animation (High Risk)

**Objective:** Add time-based rotation animation to primitives

**Changes:**

| # | target_file | target_symbol | change | risk |
|---|------------|---------------|--------|------|
| D1 | `include/scene/primitive.h` | `Primitive` | Add `m_rotationSpeed` + `setRotationSpeed()` | LOW |
| D2 | `src/scene/primitive.cpp` | `Primitive` | Initialize `m_rotationSpeed` to zero | LOW |
| D3 | `include/scene/scene.h` | `Scene` | Add `update(float deltaTime)` method | MEDIUM |
| D4 | `src/scene/scene.cpp` | `Scene` | Implement `update()` — rotate each primitive | MEDIUM |
| D5 | `src/main.cpp` | `main()` | Add delta time tracking, call `scene.update(dt)` | MEDIUM |
| D6 | `include/core/window.h` | `Window` | Add `getDeltaTime()` using GLFW time | LOW |
| D7 | `src/core/window.cpp` | `Window` | Implement delta time tracking | MEDIUM |

**Existing Dependencies:** GLFw `glfwGetTime()` for delta time. Already linked via window.cpp.

**Architectural Relationship:** Animation requires a time source (Window/GLFW), a scene update method, and per-primitive rotation speed. This introduces a temporal dimension to the scene graph.

---

## Build Verification Strategy

| Attempt | Method | Expected Result |
|---------|--------|-----------------|
| 1 | CMake configure | FAIL (no cmake) |
| 2 | Syntax check | Manual review |
| 3 | Compilation | BLOCKED (no compiler) |

**Classification:** PROCEED_WITHOUT_BUILD — changes are syntactically verifiable, grounded in existing patterns.
