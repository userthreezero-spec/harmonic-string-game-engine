# HSE-012: Baseline Reproduction

## Git State

| Property | Value |
|----------|-------|
| Branch | `harmonic-string-game-engine` |
| Commit | `53b7653` (HSE-011 complete) |
| Remote | `origin` (up to date) |
| Working tree | Clean for experiment (uncommitted material changes exist but are baseline) |

## Verifier Baseline (HSE-009 v2)

Run: `python tools/architecture_verifier/verify.py --repo . --benchmark`

```
Total:     15
VERIFIED:      10  (66%)
CONTRADICTED:  5  (33%)
UNVERIFIED:    0
INFERRED:      0
```

### Verified Claims (10)

| ID | Claim | Evidence |
|----|-------|----------|
| C100 | File `include/scene/camera.h` exists | Found |
| C101 | Class `Camera` in `include/scene/camera.h:9` | Found |
| C102 | Class `Bridge` in `include/bridge/bridge.h:42` | Found |
| C103 | Member `Primitive::m_position` (type: `Vec3`) | Found |
| C104 | Member `Camera::m_fovDeg` (type: `float`) | Found |
| C105 | Member `Bridge::m_running` (type: `std::atomic<bool>`) | Found |
| C106 | Enum `ProjectionType` found | Found |
| C107 | Method `Primitive::uploadGPU` | Found |
| C108 | Method `Renderer::renderScene` | Found |
| C109 | Namespace `hse` found | Found |

### Contradicted Claims (5)

| ID | Claim | Actual |
|----|-------|--------|
| C110 | Member `Primitive::m_normals` exists | NOT FOUND — Primitive has 15 members: `s_nextID, m_id, m_name, m_type, m_position, m_rotation, m_scale, m_rotationSpeed, m_material, m_vertices...` |
| C111 | File `src/renderer/renderer_state.cpp` exists | NOT FOUND |
| C112 | Class `TextureManager` exists | NOT FOUND in repository |
| C113 | Member `Primitive::m_textureID` exists | NOT FOUND — Primitive has no texture member |
| C114 | File `src/resource/texture_manager.cpp` exists | NOT FOUND |

## Knowledge Corpus Metrics (HSE-011)

| Metric | Value |
|--------|-------|
| Knowledge items | 64 |
| Knowledge files | 18 |
| Authoritative sources | 15 (3× A, 2× B, 10× D) |
| Domains covered | 16 |
| Unique concepts | 167 |
| Benchmark missions | 55 |
| Ablation conditions | 7 |

### Domain Coverage

| Domain | File(s) | Items | Source |
|--------|---------|-------|--------|
| rendering | `graphics_pipeline.json` | 8 | SRC-006 |
| rendering_state | `depth_testing.json`, `alpha_blending.json` | 7 | SRC-006 |
| textures | `opengl_textures.json`, `cubemaps.json` | 7 | SRC-002, SRC-006 |
| gpu_resources | `buffer_objects.json` | 3 | SRC-002 |
| gpu_resources_extended | `bgfx_architecture.json` | 3 | SRC-030 |
| input_handling | `glfw_input.json` | 4 | SRC-003 |
| math | `glm_mathematics.json` | 4 | SRC-016 |
| gui | `dear_imgui.json` | 3 | SRC-012 |
| ecs | `entt_ecs.json` | 2 | SRC-027 |
| serialization | `nlohmann_json.json` | 2 | SRC-025 |
| physics | `box2d_physics.json` | 3 | SRC-028 |
| audio | `miniaudio.json` | 3 | SRC-022 |
| profiling | `tracy_profiler.json` | 2 | SRC-023 |
| resource_management | `godot_settings_and_uid.json` | 3 | SRC-009 |
| scene_management | `godot_scene_tree.json` | 3 | SRC-004 |
| engine_architecture | `raylib_core.json` | 3 | SRC-029 |

## Ground Truth

| Metric | Value |
|--------|-------|
| Total files | 27 |
| Headers | 11 |
| Sources | 14 |
| Tests | 2 |
| Classes | 12 (incl. Material from commit `8202f7d`) |
| Structs | 8 |
| Enums | 3 |
| Functions | 127 |
| Namespace | `hse` |

### Discrepancy from HSE-011

HSE-011 reported **10 classes** in its handoff. The current ground truth shows **12 classes** because:

1. **`Material`** was added in commit `8202f7d` (HSE-012 texture mapping attempt) at `include/renderer/material.h`
2. **`Texture`** exists at `include/renderer/texture.h` (added with Material)

HSE-011's RESULTS.json referenced 10 classes because the corpus was scanned before commit `8202f7d` added Material/Texture. The ground truth scan in `ground_truth_scan.json` includes both, giving 12.

### Additional Untracked Files (Baseline)

These exist in working tree but are NOT committed. They are HSE-012 material wiring work:

- `include/renderer/material.h`
- `src/renderer/material.cpp`
- `src/material_demo.cpp`

The ground truth scan includes them because they are on disk, but they are not part of the HSE-011 baseline commit `53b7653`.

## Benchmark Baseline Summary

The HSE-009 verifier baseline of 15 claims (10 verified + 5 contradicted) is the measurement instrument for HSE-012. The 5 contradicted claims (C110-C114) represent known errors in HSE-009's blind predictions — they claimed symbols/files that don't exist. This 66% verification rate is the floor that HSE-012 knowledge-grounded predictions must beat.
