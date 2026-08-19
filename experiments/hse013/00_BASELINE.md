# HSE-013 Phase 0: Clean Baseline

## HEAD
- Commit: `af44c78` (HSE-012)
- Branch: `harmonic-string-game-engine`

## Working Tree State (Uncommitted)
Files modified (M):
- `CMakeLists.txt` — added `hse_material_demo` target
- `experiments/hse009/verification_results.json`
- `include/scene/primitive.h` — added `m_material` field, `setMaterial()`, `getMaterial()`
- `include/scene/scene.h` — added `m_materials` map, `addMaterial()`, `getMaterial()`, `getMaterials()`
- `src/main.cpp`
- `src/renderer/renderer.cpp` — added `#include "renderer/material.h"`, GLSL Material struct in fragment shader, `material->apply()` in render loop
- `src/scene/primitive.cpp`
- `src/scene/scene.cpp`
- `src/scene/scene_builder.cpp`

Files untracked (??):
- `HSE-013_MATERIAL_WIRING_MAP.json`
- `include/renderer/material.h` — NEW: Material class (5 fields, 11 methods)
- `src/renderer/material.cpp` — NEW: Material::apply() implementation
- `src/material_demo.cpp` — NEW: material demo

## Build Environment
- GCC 16.1.0 (MSYS2)
- CMake 4.4.2
- GLFW 3.4
- GLEW 2.2.0
- OpenGL 3.3

## Committed Targets (from HSE-012)
- `hse_core` (static library) — window, renderer, texture, material, scene, camera, primitive, scene_builder, vec3, mat4
- `hse_main` — main executable
- `hse_room_demo` — room demo
- `hse_bridge_app` — bridge app
- `hse_workspace_app` — workspace app
- `hse_texture_demo` — texture demo
- `hse_test_math` — math tests
- `hse_test_scene` — scene tests

## Uncommitted New Target
- `hse_material_demo` — material demo (in working tree CMakeLists.txt)

## Existing Tests
- `tests/test_math.cpp` → `hse_test_math`
- `tests/test_scene.cpp` → `hse_test_scene`

## Current Material Implementation (Working Tree)
Material class exists at `include/renderer/material.h`:
- Fields: m_name (string), m_albedo (Vec3), m_albedoMap (shared_ptr<Texture>), m_roughness (float), m_metallic (float)
- Methods: getName(), setAlbedo()/getAlbedo(), setAlbedoMap()/getAlbedoMap()/hasAlbedoMap(), setRoughness()/getRoughness(), setMetallic()/getMetallic(), apply(unsigned int shaderProgram)
- apply() calls: glUniform3f for albedo, glUniform1f for roughness/metallic, Texture::bind(0) + glUniform1i for albedo map
- Fragment shader: GLSL struct Material { albedo, roughness, metallic, hasAlbedoMap, albedoMap } with uMaterial uniform
- Primitive integration: m_material shared_ptr, setMaterial()/getMaterial()
- Renderer integration: material->apply() called per-primitive, fallback "Default" material if none assigned
- Scene integration: m_materials unordered_map<string, shared_ptr<Material>>, addMaterial()/getMaterial()/getMaterials()

## Classification
- Material system: EXISTS (working tree, uncommitted)
- Architecture: Needs verification against authoritative knowledge
- Tests for material: NONE (only demo exists)
- Integration: Primitive, Renderer, Scene, Material, Fragment Shader
