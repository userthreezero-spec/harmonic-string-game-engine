# HSE-019 — Implementation Report

## Summary
The implementation was performed in two phases: Engine Extensions and World Construction.

## Engine Extensions
The following capabilities were added to HSE:
1. **Lighting System**: A new `hse::Light` class was created, and `hse::Scene` was updated to manage point lights.
2. **Vertex Normals**: All primitives (Triangle, Quad, Cube) were updated to generate and store normal vectors in an interleaved vertex buffer (Pos+UV+Normal).
3. **Lighting Shaders**: The renderer's GLSL shaders were updated to implement a Blinn-Phong lighting model (Ambient + Diffuse + Specular).
4. **Persistence Fixes**: `SceneBuilder` was updated to correctly export and import rotation, scale, and lighting data in the `.hsc` format.

## World Construction
The 3D room was constructed autonomously using a Python script (`hse019_build_room.py`). The resulting scene contains:
- 5 base structural surfaces (Floor, Ceiling, Back/Left/Right Walls).
- 3 front wall sections forming a doorway.
- 1 window frame.
- A table (5 cubes).
- Two chairs (4 cubes total).
- 1 warm point light source.
- Animated spinning chair (`chair_1`).

## Files Modified
- `include/scene/light.h` (New)
- `include/scene/scene.h`
- `src/scene/scene.cpp`
- `include/scene/primitive.h`
- `src/scene/primitive.cpp`
- `src/renderer/renderer.cpp`
- `src/scene/scene_builder.cpp`
- `CMakeLists.txt`
- `tests/test_scene.cpp`
- `tests/test_hsc_persistence.cpp` (New)
