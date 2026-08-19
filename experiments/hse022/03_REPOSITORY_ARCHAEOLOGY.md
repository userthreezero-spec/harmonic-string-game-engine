# HSE-022 — Repository Archaeology

## Summary
The Harmonic String Engine (HSE) is currently at the HSE-021 baseline, supporting a scene graph with primitives, materials, lighting, persistence, and basic semantic world transformation via external orchestrators.

## Subsystems (PROVEN)

### 1. Scene & Primitives
- `hse::Scene`: Manages objects (`m_primitives`), lights (`m_lights`), and cameras.
- `hse::Primitive`: Supports `position`, `rotation`, `scale`, and `rotation_speed`.
- Hierarchy: Objects can have a parent-child relationship.

### 2. Renderer
- OpenGL 3.3.
- Blinn-Phong lighting with single point light support.
- Viewport and camera aspect ratio synchronization (HSE-020 fix).

### 3. Persistence
- `.hsc` (JSON) format.
- `hse::SceneBuilder`: Handles export/import of full scene state.

### 4. Interaction & Bridge
- `hse_workspace_app`: Interactive viewer with mouse/keyboard control.
- `hse::Bridge`: Named pipe NDJSON protocol for remote scene mutation.
- Commands: `set_position`, `set_rotation`, `set_scale`, `set_rotation_speed`, `add_primitive`, `save_project`, etc.

### 5. Math
- `hse::Vec3`, `hse::Mat4`: Standard 3D math operations.
- `hse::Ray`, `hse::Picker`: Ray-casting selection logic (HSE-020).

## Capability Gaps for HSE-022 (PLANNED)

1. **Spatial Reasoning / Collision Query**: No built-in geometric collision tests between primitives other than ray-casting. 
2. **Path Clearance**: No explicit pathfinding or clearance volume checking.
3. **Multi-Constraint Planning**: Planning logic currently resides in WebOS or external Python orchestrators.

## Architecture Map (Verified)
- `include/scene/scene.h`, `src/scene/scene.cpp`
- `include/scene/primitive.h`, `src/scene/primitive.cpp`
- `include/scene/scene_builder.h`, `src/scene/scene_builder.cpp`
- `include/bridge/bridge.h`, `src/bridge/bridge.cpp`
- `src/workspace_main.cpp`
