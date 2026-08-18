# HSE-011 SOURCE_SELECTION.md — Source Selection Rationale

**Date**: 2026-08-18
**Phase**: 2 — Source Archaeology
**Status**: COMPLETE

---

## Selection Criteria

Sources were selected based on:

1. **Authority**: Must be from the official source owner or a recognized authority
2. **Relevance**: Must directly relate to domains HSE-007 lacked knowledge in
3. **Quality Classification**: Priority given to A-D classifications (primary specs through mature open-source)
4. **Recency**: Prefer current versions (2023+)
5. **Practical Value**: Must contain implementable knowledge, not just theory
6. **GitHub Stars**: 1000+ stars indicates community validation

---

## Selected Sources by Domain

### Rendering & OpenGL
| Source | Why Selected |
|--------|-------------|
| SRC-001: OpenGL 3.3 Core Spec (Khronos) | Primary standard. Authoritative for all OpenGL behavior. |
| SRC-002: OpenGL Reference Pages (docs.gl) | Practical function reference. More usable than spec PDF. |
| SRC-006/007: LearnOpenGL | Best OpenGL 3.3 tutorial. Working code examples. 12K+ stars. |
| SRC-008: bgfx | Cross-platform rendering abstraction. 14K+ stars. |
| SRC-014: Filament PBR Theory | State-of-the-art PBR. Mathematical derivations. Google-backed. |

### Window Management & Input
| Source | Why Selected |
|--------|-------------|
| SRC-003: GLFW 3.4 Docs | HSE uses GLFW directly. Official documentation. |
| SRC-004: GLFW 3.4 Source | Shows platform abstraction implementation. |
| SRC-018: SDL3 | Alternative platform abstraction. 12K+ stars. |

### Mathematics
| Source | Why Selected |
|--------|-------------|
| SRC-015/016: GLM | Standard math library for OpenGL C++. 11K+ stars. Based on GLSL spec. |

### UI
| Source | Why Selected |
|--------|-------------|
| SRC-012/019: Dear ImGui | Definitive IMGUI library. 50K+ stars. Authoritative for IMGUI paradigm. |

### Game Engine Architecture
| Source | Why Selected |
|--------|-------------|
| SRC-009: Godot Engine | Complete game engine. 90K+ stars. Shows scene graph, resource management. |
| SRC-010: OGRE 3D | Mature scene graph. Referenced by GEA textbook. |
| SRC-011: Hazel | Educational engine. Clear architecture. 13K+ stars. |
| SRC-017: Magnum | Clean C++11 graphics middleware. Good abstraction patterns. |
| SRC-025: Game Engine Architecture (Book) | Industry standard textbook. Written by Naughty Dog lead programmer. |
| SRC-030: Real-Time Rendering (Book) | Definitive rendering algorithms reference. |

### ECS (Entity Component System)
| Source | Why Selected |
|--------|-------------|
| SRC-026: Flecs | Most popular C/C++ ECS. 7K+ stars. Battle-tested. |
| SRC-027: EnTT | Used by Godot 4. 10K+ stars. Modern C++ design. |

### Physics
| Source | Why Selected |
|--------|-------------|
| SRC-028: Box2D | Standard 2D physics. Used by Unity, Godot. 9K+ stars. |
| SRC-029: Bullet Physics | Standard 3D physics. Used by many commercial games. |

### Audio
| Source | Why Selected |
|--------|-------------|
| SRC-022: miniaudio | Single-file audio library. Well-documented. |

### Serialization
| Source | Why Selected |
|--------|-------------|
| SRC-031: nlohmann/json | Standard C++ JSON library. 42K+ stars. |
| SRC-032: cereal | Well-established serialization. 3K+ stars. |

### C++ Standard Library
| Source | Why Selected |
|--------|-------------|
| SRC-020: cppreference.com | Definitive C++ reference. Based on ISO standard. |
| SRC-021: Microsoft C++ Docs | Authoritative for MSVC and Windows-specific behavior. |

### Profiling
| Source | Why Selected |
|--------|-------------|
| SRC-023: Tracy Profiler | Gold standard for real-time frame profiling. |

### Build Systems
| Source | Why Selected |
|--------|-------------|
| SRC-024: CMake Documentation | HSE uses CMake. Official documentation. |

---

## Sources Rejected

| Source | Why Rejected |
|--------|-------------|
| Unity Documentation | Proprietary engine. Patterns not transferable to open-source OpenGL project. |
| Unreal Engine Documentation | Proprietary. Too complex for HSE's scope. |
| Tutorial blogs (random) | Quality varies. Not authoritative. |
| Stack Overflow answers | Not authoritative. May be incorrect. |
| AI-generated content | Untrusted. Cannot verify accuracy. |

---

## Source Coverage Gaps

The following domains have limited authoritative source coverage:

1. **Audio systems**: miniaudio covers basic playback, but no authoritative reference for spatial audio or advanced audio DSP
2. **Networking**: No authoritative source selected yet (low priority for HSE's current scope)
3. **Animation systems**: No authoritative source selected yet (HSE doesn't have skeletal animation)
4. **Particle systems**: No authoritative source selected yet
5. **Terrain rendering**: No authoritative source selected yet

These gaps are acceptable because:
- HSE-007's knowledge gaps were primarily in rendering, math, resource management, and engine architecture
- The selected sources cover all critical gaps identified in HSE-010
- Additional sources can be added as needed during ingestion

---

## Next Steps

1. Create AUTHORITATIVE_SOURCE_POLICY.md with classification rules
2. Begin Phase 3 — Deep Knowledge Ingestion using these 32 sources
3. Track word counts and knowledge quality as ingestion proceeds
