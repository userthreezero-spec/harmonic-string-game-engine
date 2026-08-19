# HSE-011 FILE 14: Knowledge Value Matrix

**Date**: 2026-08-18
**Experiment**: HSE-011 Knowledge Saturation Experiment
**Status**: COMPLETE

---

## Purpose

Evaluate each knowledge source across domains and assign value scores. Identify which domains have high-value sources and which remain knowledge-starved.

## Coverage Matrix

Values: **NONE** (no coverage), **PARTIAL** (some relevant knowledge), **FULL** (comprehensive coverage of domain)

| Source ID | Source Name | Class | Rendering | Input | Math | Scene | Resources | Audio | Physics | UI | Profiling | Build | Serialization | General Eng | Items |
|-----------|-------------|-------|-----------|-------|------|-------|-----------|-------|---------|-----|-----------|-------|---------------|-------------|-------|
| SRC-002 | docs.gl (OpenGL Ref) | A | FULL | NONE | NONE | NONE | PARTIAL | NONE | NONE | NONE | NONE | NONE | NONE | NONE | 7 |
| SRC-003 | GLFW 3.4 Docs | B | NONE | FULL | NONE | NONE | NONE | NONE | NONE | NONE | NONE | NONE | NONE | PARTIAL | 4 |
| SRC-006 | LearnOpenGL | F | FULL | NONE | NONE | NONE | NONE | NONE | NONE | NONE | NONE | NONE | NONE | NONE | 16 |
| SRC-009 | Godot Engine | D | PARTIAL | NONE | NONE | PARTIAL | FULL | NONE | NONE | NONE | NONE | NONE | NONE | FULL | 3 |
| SRC-011 | Hazel Engine | D | PARTIAL | NONE | NONE | NONE | PARTIAL | NONE | NONE | NONE | NONE | NONE | NONE | FULL | 2 |
| SRC-012 | Dear ImGui | D | PARTIAL | NONE | NONE | NONE | NONE | NONE | NONE | FULL | NONE | NONE | NONE | NONE | 3 |
| SRC-016 | GLM Manual | B | NONE | NONE | FULL | NONE | NONE | NONE | NONE | NONE | NONE | NONE | NONE | NONE | 4 |
| SRC-022 | miniaudio | D | NONE | NONE | NONE | NONE | NONE | FULL | NONE | NONE | NONE | NONE | NONE | NONE | 3 |
| SRC-023 | Tracy Profiler | D | NONE | NONE | NONE | NONE | NONE | NONE | NONE | NONE | FULL | NONE | NONE | NONE | 2 |
| SRC-025 | nlohmann/json | D | NONE | NONE | NONE | NONE | NONE | NONE | NONE | NONE | NONE | NONE | FULL | NONE | 2 |
| SRC-027 | EnTT ECS | D | NONE | NONE | NONE | PARTIAL | NONE | NONE | NONE | NONE | NONE | NONE | NONE | FULL | 2 |
| SRC-028 | Box2D v3 | D | NONE | NONE | NONE | NONE | NONE | NONE | FULL | NONE | NONE | NONE | NONE | NONE | 3 |

**Legend**: Sources shown are those with ingested knowledge items (12 of 32 cataloged). Remaining 20 sources are cataloged in SOURCE_REGISTRY.json but not yet ingested.

---

## Sources NOT Yet Ingested

These sources are in the registry but have no knowledge items extracted:

| Source ID | Source Name | Class | Expected Domains | Priority |
|-----------|-------------|-------|-----------------|----------|
| SRC-001 | OpenGL 3.3 Spec | A | Rendering, Shaders, Textures | HIGH |
| SRC-004 | GLFW 3.4 Source | C | Window, Input, Platform | MEDIUM |
| SRC-005 | GLEW Docs | B | Extension Loading | LOW |
| SRC-007 | LearnOpenGL Source | F | Rendering | LOW |
| SRC-008 | bgfx | D | Rendering Abstraction | MEDIUM |
| SRC-010 | OGRE 3D | D | Scene Graph, Rendering | MEDIUM |
| SRC-013 | Filament | D | PBR Rendering | HIGH |
| SRC-014 | Filament PBR Theory | B | PBR, Lighting | HIGH |
| SRC-015 | GLM Source | D | Mathematics | LOW |
| SRC-017 | Magnum | D | Graphics Abstraction | LOW |
| SRC-018 | SDL3 | D | Platform, Input, Audio | LOW |
| SRC-019 | Dear ImGui Wiki | B | UI Paradigm | MEDIUM |
| SRC-020 | cppreference | B | C++ Standard Library | MEDIUM |
| SRC-021 | Microsoft C++ Docs | B | Windows, MSVC | LOW |
| SRC-024 | CMake Docs | B | Build Systems | MEDIUM |
| SRC-026 | Flecs | D | ECS | MEDIUM |
| SRC-029 | Bullet Physics | D | 3D Physics | MEDIUM |
| SRC-030 | Real-Time Rendering | E | Rendering Algorithms | MEDIUM |
| SRC-031 | nlohmann/json Source | D | Serialization | LOW |
| SRC-032 | cereal | D | Serialization | LOW |

---

## Value Scores

Scale: 1 (minimal) to 5 (maximum)

### Novelty Score (1-5)
How many NEW concepts does this source contribute that are not available from other ingested sources?

### Architecture Impact (1-5)
How much does this source change architectural decisions for HSE?

### Symbol Impact (1-5)
How much does this source improve the ability to predict exact HSE symbols?

### Implementation Value (1-5)
How directly applicable is this knowledge to implementing HSE features?

| Source ID | Source Name | Novelty | Arch Impact | Symbol Impact | Impl Value | Average |
|-----------|-------------|---------|-------------|---------------|------------|---------|
| SRC-002 | docs.gl | 4 | 3 | 4 | 5 | 4.0 |
| SRC-003 | GLFW Docs | 4 | 3 | 4 | 5 | 4.0 |
| SRC-006 | LearnOpenGL | 5 | 4 | 4 | 5 | 4.5 |
| SRC-009 | Godot Engine | 4 | 5 | 2 | 3 | 3.5 |
| SRC-011 | Hazel Engine | 3 | 4 | 1 | 2 | 2.5 |
| SRC-012 | Dear ImGui | 4 | 4 | 1 | 3 | 3.0 |
| SRC-016 | GLM Manual | 3 | 2 | 3 | 4 | 3.0 |
| SRC-022 | miniaudio | 5 | 3 | 1 | 4 | 3.3 |
| SRC-023 | Tracy Profiler | 4 | 3 | 1 | 3 | 2.8 |
| SRC-025 | nlohmann/json | 3 | 2 | 1 | 3 | 2.3 |
| SRC-027 | EnTT ECS | 4 | 4 | 1 | 3 | 3.0 |
| SRC-028 | Box2D v3 | 4 | 3 | 1 | 4 | 3.0 |

### Score Analysis

**Highest Value Sources**:
1. **SRC-006 (LearnOpenGL)** — 4.5 avg: Best single source. Covers rendering pipeline comprehensively with working code. 16 items, highest yield.
2. **SRC-002 (docs.gl)** — 4.0 avg: Authoritative API reference. Directly maps to HSE's OpenGL calls.
3. **SRC-003 (GLFW Docs)** — 4.0 avg: Directly applicable. HSE uses GLFW. Input and window patterns.
4. **SRC-009 (Godot)** — 3.5 avg: Highest architecture impact. Shows complete engine patterns but symbol mapping is weak.

**Lowest Value Sources**:
1. **SRC-025 (nlohmann/json)** — 2.3 avg: Serialization only. Low novelty (JSON is well-known).
2. **SRC-011 (Hazel)** — 2.5 avg: Overlapping with Godot for architecture. Low symbol impact.
3. **SRC-023 (Tracy)** — 2.8 avg: Narrow domain (profiling only). Macro-based, low architectural complexity.

---

## Domain Starvation Analysis

| Domain | Ingested Sources | Items | Starved? | Minimum Needed | Priority |
|--------|-----------------|-------|----------|----------------|----------|
| **Rendering** | 3 (SRC-002, SRC-006, partial SRC-009,011) | ~31 | NO | 3 | Saturated for basics, need PBR/shadows |
| **Input** | 1 (SRC-003) | 4 | YES | 2 | Need SRC-004 (GLFW source) or SRC-018 (SDL3) |
| **Math** | 1 (SRC-016) | 4 | YES | 2 | Need SRC-015 (GLM source) for advanced patterns |
| **Scene Mgmt** | 1 (SRC-009, partial) | 3 | YES | 3 | Need SRC-010 (OGRE) + deeper Godot study |
| **Resources** | 2 (SRC-009, SRC-011) | 5 | YES | 2 | Borderline. Need resource lifecycle patterns |
| **Audio** | 1 (SRC-022) | 3 | YES | 2 | Need second source (OpenAL or SDL3 audio) |
| **Physics** | 1 (SRC-028) | 3 | YES | 2 | Need SRC-029 (Bullet) for 3D physics patterns |
| **UI** | 1 (SRC-012) | 3 | YES | 2 | Need SRC-019 (ImGui wiki) for paradigm depth |
| **Profiling** | 1 (SRC-023) | 2 | YES | 2 | Borderline. Tracy is comprehensive for its domain |
| **Build** | 0 | 0 | YES | 1 | Need SRC-024 (CMake docs) |
| **Serialization** | 1 (SRC-025) | 2 | YES | 2 | Need SRC-032 (cereal) for comparison |
| **General Eng** | 3 (SRC-009, SRC-011, SRC-027) | 7 | NO | 2 | Adequate from engine studies |

### Most Starved Domains (ranked by urgency)

1. **Build Systems** — 0 sources. CMake is HSE's build system and has zero knowledge coverage.
2. **Scene Management** — 1 partial source. The scene graph is central to HSE and barely covered.
3. **Input** — 1 source. GLFW docs cover basics, but no deeper patterns (gamepad, input mapping).
4. **Physics** — 1 source. Box2D covers 2D, but no 3D physics knowledge.
5. **Audio** — 1 source. miniaudio alone is insufficient for spatial audio patterns.
6. **UI** — 1 source. Dear ImGui alone covers the library but not integration patterns deeply.
7. **Math** — 1 source. GLM manual covers types but not advanced math (interpolation, noise, etc.).
8. **Serialization** — 1 source. nlohmann/json alone — no binary serialization knowledge.
9. **Resources** — 2 sources, but both are general engine studies. No dedicated resource management knowledge.
10. **Profiling** — 1 source. Adequate for Tracy but no comparative profiling approaches.

---

## Value by Domain (Aggregated)

| Domain | Total Novelty | Total Arch Impact | Total Symbol Impact | Total Impl Value | Avg per Source |
|--------|--------------|-------------------|--------------------|--------------------|----------------|
| Rendering | 13 | 11 | 12 | 15 | 3.6 |
| Input | 4 | 3 | 4 | 5 | 4.0 |
| Math | 3 | 2 | 3 | 4 | 3.0 |
| Scene | 4 | 5 | 2 | 3 | 3.5 |
| Resources | 7 | 8 | 2 | 5 | 3.6 |
| Audio | 5 | 3 | 1 | 4 | 3.3 |
| Physics | 4 | 3 | 1 | 4 | 3.0 |
| UI | 4 | 4 | 1 | 3 | 3.0 |
| Profiling | 4 | 3 | 1 | 3 | 2.8 |
| Build | 0 | 0 | 0 | 0 | N/A |
| Serialization | 3 | 2 | 1 | 3 | 2.3 |
| General Eng | 11 | 13 | 4 | 8 | 3.2 |

### Key Findings

1. **Rendering has highest aggregate value** — expected, as HSE is a rendering-focused engine
2. **General Engineering has surprisingly high architecture impact** — engine architecture patterns from Godot/Hazel/EnTT inform HSE's structural decisions
3. **Symbol impact is universally low** for non-rendering domains — because HSE doesn't yet have audio, physics, UI, profiling, or ECS code. There are no symbols to predict.
4. **Build systems have zero coverage** — completely unaddressed
5. **Serialization has lowest value scores** — JSON is simple, limited architectural impact

---

## Ingestion Priority Recommendation

Based on the value matrix, the next ingestion round should target:

| Priority | Source | Domain | Expected Value | Rationale |
|----------|--------|--------|---------------|-----------|
| 1 | SRC-024 (CMake Docs) | Build | HIGH | Zero coverage, HSE uses CMake |
| 2 | SRC-010 (OGRE 3D) | Scene Mgmt | HIGH | Mature scene graph patterns |
| 3 | SRC-013/014 (Filament) | PBR/Rendering | HIGH | Next rendering frontier |
| 4 | SRC-029 (Bullet) | Physics | MEDIUM | 3D physics patterns |
| 5 | SRC-020 (cppreference) | C++ Std Lib | MEDIUM | Smart pointers, containers |
| 6 | SRC-019 (ImGui Wiki) | UI | MEDIUM | Deep IMGUI paradigm |
| 7 | SRC-026 (Flecs) | ECS | MEDIUM | Compare with EnTT |
| 8 | SRC-018 (SDL3) | Platform | LOW | Alternative platform layer |
| 9 | SRC-030 (RTR Book) | Rendering | LOW | Advanced algorithms |
| 10 | SRC-032 (cereal) | Serialization | LOW | Binary serialization patterns |
