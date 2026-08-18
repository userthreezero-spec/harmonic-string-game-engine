# HSE-011 SOURCE_CATALOG.md — Human-Readable Source Catalog

**Date**: 2026-08-18
**Phase**: 2 — Source Archaeology
**Status**: COMPLETE

---

## Overview

This document provides a human-readable catalog of all 32 authoritative sources selected for HSE-011 deep knowledge ingestion. For machine-readable version, see `SOURCE_REGISTRY.json`.

---

## Tier 1: Primary Specifications (Classification A)

| ID | Source | Domain | Why It Matters |
|----|--------|--------|----------------|
| SRC-001 | OpenGL 3.3 Core Profile Specification (Khronos) | Rendering, Shaders, Textures | The definitive standard for OpenGL. HSE uses OpenGL 3.3 directly. |

---

## Tier 2: Official Documentation (Classification B)

| ID | Source | Domain | Why It Matters |
|----|--------|--------|----------------|
| SRC-002 | OpenGL Reference Pages (docs.gl) | Rendering | Practical function reference for OpenGL. More usable than spec PDF. |
| SRC-003 | GLFW 3.4 Documentation | Window Management, Input | HSE uses GLFW directly. Essential for understanding window/input layer. |
| SRC-005 | GLEW Documentation | Extension Loading | HSE uses GLEW for extension loading. Simple but critical. |
| SRC-014 | Filament PBR Theory | PBR, Lighting | State-of-the-art PBR theory with mathematical derivations. |
| SRC-016 | GLM Manual | Mathematics | Comprehensive reference for vec3, mat4, quaternions. |
| SRC-019 | Dear ImGui Wiki | UI Architecture | Definitive explanation of IMGUI paradigm. |
| SRC-020 | cppreference.com | C++ Standard Library | Definitive C++ reference. Based on ISO standard. |
| SRC-021 | Microsoft C++ Documentation | Windows, MSVC | Authoritative for Windows-specific C++ behavior. |
| SRC-024 | CMake Documentation | Build Systems | HSE uses CMake. Official documentation. |

---

## Tier 3: Official Source Code (Classification C)

| ID | Source | Domain | Why It Matters |
|----|--------|--------|----------------|
| SRC-004 | GLFW 3.4 Source | Window Management | Shows platform abstraction implementation. |
| SRC-018 | SDL3 Source | Platform Abstraction | Alternative platform abstraction. 12K+ stars. |

---

## Tier 4: Mature Open-Source Implementations (Classification D)

### Rendering Engines
| ID | Source | Stars | Domain | Why It Matters |
|----|--------|-------|--------|----------------|
| SRC-008 | bgfx | 14K | Rendering Abstraction | Cross-platform rendering backend abstraction. |
| SRC-013 | Filament | 20K | PBR Rendering | State-of-the-art PBR engine. Google-backed. |
| SRC-017 | Magnum | 4K | Graphics Abstraction | Clean C++11 OpenGL wrapper. |

### Game Engines
| ID | Source | Stars | Domain | Why It Matters |
|----|--------|-------|--------|----------------|
| SRC-009 | Godot Engine | 90K | Scene Graph, Resources | Complete engine. Shows scene graph and resource lifecycle. |
| SRC-010 | OGRE 3D | 3K | Scene Graph, Rendering | Mature scene graph. Referenced by GEA textbook. |
| SRC-011 | Hazel | 13K | Engine Architecture | Educational engine with clear architecture. |

### UI
| ID | Source | Stars | Domain | Why It Matters |
|----|--------|-------|--------|----------------|
| SRC-012 | Dear ImGui | 50K | Immediate-Mode UI | Definitive IMGUI library. Shows vertex buffer output. |

### Math
| ID | Source | Stars | Domain | Why It Matters |
|----|--------|-------|--------|----------------|
| SRC-015 | GLM | 11K | Mathematics | Standard math library for OpenGL C++. |

### ECS
| ID | Source | Stars | Domain | Why It Matters |
|----|--------|-------|--------|----------------|
| SRC-026 | Flecs | 7K | ECS | Most popular C/C++ ECS. Battle-tested. |
| SRC-027 | EnTT | 10K | ECS | Used by Godot 4. Modern C++ design. |

### Physics
| ID | Source | Stars | Domain | Why It Matters |
|----|--------|-------|--------|----------------|
| SRC-028 | Box2D | 9K | 2D Physics | Standard 2D physics. Used by Unity, Godot. |
| SRC-029 | Bullet Physics | 13K | 3D Physics | Standard 3D physics engine. |

### Audio
| ID | Source | Stars | Domain | Why It Matters |
|----|--------|-------|--------|----------------|
| SRC-022 | miniaudio | 5K | Audio | Single-file audio library. Well-documented. |

### Serialization
| ID | Source | Stars | Domain | Why It Matters |
|----|--------|-------|--------|----------------|
| SRC-031 | nlohmann/json | 42K | JSON | Standard C++ JSON library. |
| SRC-032 | cereal | 3K | Serialization | Header-only serialization. |

### Profiling
| ID | Source | Stars | Domain | Why It Matters |
|----|--------|-------|--------|----------------|
| SRC-023 | Tracy Profiler | 10K | Profiling | Gold standard for real-time frame profiling. |

### Tutorial Code
| ID | Source | Stars | Domain | Why It Matters |
|----|--------|-------|--------|----------------|
| SRC-007 | LearnOpenGL Source | 12K | Rendering | Working OpenGL 3.3 code examples. |

---

## Tier 5: Secondary References (Classification E)

| ID | Source | Domain | Why It Matters |
|----|--------|--------|----------------|
| SRC-025 | Game Engine Architecture (4th Ed) | Engine Architecture | Industry standard textbook. Written by Naughty Dog lead. |
| SRC-030 | Real-Time Rendering (4th Ed) | Rendering Algorithms | Definitive rendering algorithms reference. |

---

## Tier 6: Tutorial Resources (Classification F)

| ID | Source | Domain | Why It Matters |
|----|--------|--------|----------------|
| SRC-006 | LearnOpenGL (tutorials) | Rendering | Best OpenGL 3.3 tutorial. Not counted toward saturation. |

---

## Coverage Matrix

| Domain | A | B | C | D | E | F | Total |
|--------|---|---|---|---|---|---|-------|
| Rendering | 1 | 2 | 0 | 3 | 1 | 1 | 8 |
| Shaders | 1 | 1 | 0 | 1 | 0 | 1 | 4 |
| Textures | 1 | 0 | 0 | 1 | 0 | 1 | 3 |
| Mathematics | 0 | 1 | 0 | 1 | 0 | 0 | 2 |
| ECS | 0 | 0 | 0 | 2 | 0 | 0 | 2 |
| Physics | 0 | 0 | 0 | 2 | 0 | 0 | 2 |
| Audio | 0 | 0 | 0 | 1 | 0 | 0 | 1 |
| Serialization | 0 | 0 | 0 | 2 | 0 | 0 | 2 |
| Profiling | 0 | 0 | 0 | 1 | 0 | 0 | 1 |
| Build Systems | 0 | 1 | 0 | 0 | 0 | 0 | 1 |
| Engine Architecture | 0 | 0 | 2 | 3 | 1 | 0 | 6 |
| Window Management | 0 | 1 | 1 | 1 | 0 | 0 | 3 |
| Input Handling | 0 | 1 | 1 | 1 | 0 | 0 | 3 |
| Resource Management | 0 | 0 | 0 | 3 | 0 | 0 | 3 |
| Scene Graph | 0 | 0 | 0 | 3 | 0 | 0 | 3 |
| UI | 0 | 1 | 0 | 1 | 0 | 0 | 2 |
| C++ Standard Library | 0 | 2 | 0 | 0 | 0 | 0 | 2 |
| **TOTAL** | **1** | **10** | **1** | **16** | **2** | **1** | **31** |

---

## Quality Summary

- **A-D sources**: 28 (87.5%) — Count toward saturation
- **E sources**: 2 (6.25%) — Useful but not counted
- **F sources**: 1 (3.12%) — Tutorial only, not counted
- **G sources**: 0 (0%) — No untrusted sources included

**Total authoritative knowledge sources**: 28
**Estimated total readable content**: ~500K words (across all sources)
**HSE-relevant content estimate**: ~200K words

---

## Gaps Identified

The following domains have limited coverage:

1. **Audio**: Only miniaudio (1 source). Need OpenAL or FMOD docs for spatial audio.
2. **Networking**: No sources selected. Low priority for HSE's current scope.
3. **Animation**: No sources selected. HSE doesn't have skeletal animation yet.
4. **Particle Systems**: No sources selected. HSE doesn't have particles yet.
5. **Terrain**: No sources selected. HSE doesn't have terrain yet.

These gaps are acceptable because:
- HSE-007's knowledge gaps were primarily in rendering, math, resource management
- The selected sources cover all critical gaps identified in HSE-010
- Additional sources can be added as needed
