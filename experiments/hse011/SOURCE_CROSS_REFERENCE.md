# HSE-011 SOURCE_CROSS_REFERENCE.md — Source Cross-Reference Matrix

**Date**: 2026-08-18
**Phase**: 2 — Source Archaeology
**Status**: COMPLETE

---

## Purpose

Cross-reference matrix showing which sources cover which topics. Used to:
1. Identify knowledge overlaps (multiple sources on same topic = higher confidence)
2. Identify knowledge gaps (no source on a topic = needs research)
3. Plan ingestion order (start with most cross-referenced sources)

---

## Cross-Reference Matrix

### OpenGL Rendering Pipeline

| Topic | SRC-001 (GL Spec) | SRC-002 (GL Ref) | SRC-006 (LearnGL) | SRC-008 (bgfx) | SRC-013 (Filament) | Coverage |
|-------|:--:|:--:|:--:|:--:|:--:|:--:|
| Buffer creation/management | X | X | X | X | - | 4/5 |
| VAO/VBO/EBO lifecycle | X | X | X | X | - | 4/5 |
| Shader compilation | X | X | X | X | X | 5/5 |
| Texture creation | X | X | X | X | X | 5/5 |
| Framebuffer objects | X | X | X | X | - | 4/5 |
| State management | X | X | - | X | - | 3/5 |
| Draw calls | X | X | X | X | X | 5/5 |
| Extension loading | X | - | - | X | - | 2/5 |

**Confidence**: HIGH — 4+ sources on most rendering topics

### Window Management & Input

| Topic | SRC-003 (GLFW Docs) | SRC-004 (GLFW Src) | SRC-018 (SDL3) | Coverage |
|-------|:--:|:--:|:--:|:--:|
| Window creation | X | X | X | 3/3 |
| OpenGL context | X | X | X | 3/3 |
| Keyboard input | X | X | X | 3/3 |
| Mouse input | X | X | X | 3/3 |
| Gamepad input | X | X | X | 3/3 |
| Window resize handling | X | X | X | 3/3 |

**Confidence**: HIGH — All major window/input topics covered by 3 sources

### Mathematics

| Topic | SRC-015 (GLM) | SRC-016 (GLM Manual) | SRC-020 (cppref) | Coverage |
|-------|:--:|:--:|:--:|:--:|
| vec3/vec4 operations | X | X | - | 2/3 |
| mat4 transformations | X | X | - | 2/3 |
| Quaternion operations | X | X | - | 2/3 |
| Matrix multiplication | X | X | - | 2/3 |
| Projection matrices | X | X | - | 2/3 |
| Common math functions | X | X | X | 3/3 |

**Confidence**: HIGH — GLM covers all math HSE needs

### Engine Architecture Patterns

| Topic | SRC-009 (Godot) | SRC-010 (OGRE) | SRC-011 (Hazel) | SRC-025 (GEA Book) | Coverage |
|-------|:--:|:--:|:--:|:--:|:--:|
| Scene graph | X | X | - | X | 3/4 |
| Resource management | X | X | X | X | 4/4 |
| Layer/stack pattern | - | - | X | X | 2/4 |
| Event system | X | - | X | X | 3/4 |
| Renderer initialization | X | X | X | X | 4/4 |
| Main loop design | X | X | X | X | 4/4 |

**Confidence**: HIGH — All major architecture topics covered

### ECS (Entity Component System)

| Topic | SRC-026 (Flecs) | SRC-027 (EnTT) | SRC-009 (Godot) | Coverage |
|-------|:--:|:--:|:--:|:--:|
| Entity creation | X | X | X | 3/3 |
| Component registration | X | X | X | 3/3 |
| System/queries | X | X | X | 3/3 |
| Relationships | X | - | - | 1/3 |
| Prefabs | X | - | - | 1/3 |

**Confidence**: MEDIUM — Core ECS covered, advanced features less so

### Physics

| Topic | SRC-028 (Box2D) | SRC-029 (Bullet) | SRC-025 (GEA Book) | Coverage |
|-------|:--:|:--:|:--:|:--:|
| Collision detection | X | X | X | 3/3 |
| Rigid body dynamics | X | X | X | 3/3 |
| Constraint solving | X | X | X | 3/3 |
| Broadphase | X | X | X | 3/3 |
| Narrowphase | X | X | X | 3/3 |
| Integration loop | X | X | X | 3/3 |

**Confidence**: HIGH — All physics topics covered

### PBR Rendering

| Topic | SRC-013 (Filament) | SRC-014 (Filament PBR) | SRC-030 (RTR Book) | Coverage |
|-------|:--:|:--:|:--:|:--:|
| BRDF models | X | X | X | 3/3 |
| Material parameterization | X | X | - | 2/3 |
| Lighting equations | X | X | X | 3/3 |
| Image-based lighting | X | X | X | 3/3 |
| Shadow mapping | X | - | X | 2/3 |
| Post-processing | X | X | X | 3/3 |

**Confidence**: HIGH — Comprehensive PBR coverage

### Serialization

| Topic | SRC-031 (nlohmann/json) | SRC-032 (cereal) | Coverage |
|-------|:--:|:--:|:--:|
| JSON parsing | X | X | 2/2 |
| Binary serialization | - | X | 1/2 |
| Object serialization | X | X | 2/2 |
| Schema validation | - | - | 0/2 |

**Confidence**: MEDIUM — Basic serialization covered, schema validation gap

---

## Coverage Summary

| Domain | Sources | Confidence | Gap? |
|--------|---------|------------|------|
| OpenGL Rendering | 5 | HIGH | No |
| Window/Input | 3 | HIGH | No |
| Mathematics | 3 | HIGH | No |
| Engine Architecture | 4 | HIGH | No |
| ECS | 3 | MEDIUM | Partial |
| Physics | 3 | HIGH | No |
| PBR Rendering | 3 | HIGH | No |
| Audio | 1 | LOW | Yes |
| Serialization | 2 | MEDIUM | Partial |
| Profiling | 1 | MEDIUM | No |
| Build Systems | 1 | MEDIUM | No |

---

## Recommended Ingestion Order

Based on cross-reference coverage:

1. **SRC-001 + SRC-002 + SRC-006**: OpenGL fundamentals (highest coverage)
2. **SRC-015 + SRC-016**: Mathematics (essential for all rendering)
3. **SRC-003 + SRC-004**: Window/input (essential for HSE's GLFW layer)
4. **SRC-025**: Game Engine Architecture textbook (foundational concepts)
5. **SRC-009 + SRC-010 + SRC-011**: Engine architecture patterns
6. **SRC-013 + SRC-014**: PBR rendering (advanced topic)
7. **SRC-026 + SRC-027**: ECS patterns
8. **SRC-028 + SRC-029**: Physics
9. **SRC-012**: UI (Dear ImGui)
10. **Remaining sources**: Fill gaps as needed
