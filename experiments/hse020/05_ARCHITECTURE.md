# HSE-020 — Architecture

## 1. Camera Navigation

### Orbit, Pan, and Zoom
- **Orbit**: Controlled by Mouse Left-Drag. Increments yaw/pitch offsets.
- **Pan**: Controlled by Mouse Middle-Drag or Shift+Left-Drag. Translates the camera target along the camera's Right and Up vectors.
- **Zoom**: Controlled by Mouse Scroll. Adjusts the `orbitRadius`.

### Logic Placement
- `hse::Camera` will store `m_yaw`, `m_pitch`, and `m_orbitRadius` as the primary state.
- `update()` will compute `m_position` from these polar coordinates relative to `m_target`.

## 2. Object Selection (Picking)

### Mechanism: Ray-Casting
1.  **Screen-to-World**: Convert normalized mouse coordinates [-1, 1] to a 3D ray using the inverse View and Projection matrices.
2.  **Intersection**: Iterate through all `hse::Primitive` objects in the scene.
    -   **Cube**: Ray-AABB intersection.
    -   **Quad/Triangle**: Ray-Plane intersection followed by boundary check.
3.  **Result**: The primitive with the nearest intersection point is selected.

## 3. Selection State

### Workspace Controller
-   The selection state (`uint64_t m_selectedID`) will be managed by the `hse_workspace_app` (Workspace main loop).
-   This avoids polluting the base `hse::Scene` with editor-specific state unless a "Tag" system is added later.

## 4. Visual Selection

### Shader Tinting
-   Add a `uniform bool uSelected` to the fragment shader.
-   When `true`, the fragment shader will blend the final color with a highlight color (e.g., bright yellow or white tint).
-   The `Renderer` will set this uniform per-primitive during the `renderScene` loop.

## 5. Manipulation

### Direct Transform Update
-   **Position**: Arrows (X/Z) + PageUp/Down (Y).
-   **Rotation**: NUMPAD keys.
-   **Scale**: +/- keys.
-   **Bridge**: Bridge commands `set_position`, etc., will continue to work and immediately update the selected object.

## 6. Persistence

### Interactive Save
-   A keyboard shortcut (e.g., CTRL+S) in the Workspace app will trigger `hse::SceneBuilder::exportHSC`.
-   The existing HSE-019 persistence system is sufficient.

## 7. Mathematical Symbols

### Ray Class
-   `struct Ray { Vec3 origin; Vec3 direction; }`
-   Added to `math/vec3.h` or as a utility.
