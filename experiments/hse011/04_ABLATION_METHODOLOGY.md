# HSE-011 FILE 04: Ablation Study Methodology

**Experiment**: HSE-011 Knowledge Saturation with Authoritative Sources
**Date**: 2026-08-18
**Status**: DESIGN COMPLETE — ready for execution

---

## Purpose

Design a rigorous ablation study that systematically removes knowledge layers to measure the marginal contribution of each domain. Unlike HSE-010's theoretical framework, this methodology is grounded in HSE-011's actual 58 knowledge items and 15 authoritative sources.

---

## Study Design

### Independent Variable
Knowledge condition (Condition A through G), each adding a cumulative set of knowledge layers.

### Dependent Variables (9 Metrics)

| # | Metric | Scale | Measurement Method | What It Tests |
|---|--------|-------|-------------------|---------------|
| 1 | **Architecture Score** | 1-10 | Expert evaluation across 5 dimensions (symbol accuracy, architecture quality, implementation readiness, acceptance criteria, verification plan) | Overall architectural reasoning quality |
| 2 | **Symbol Accuracy** | 0-100% | HSE-009 verifier: count correct symbol references / total claims | Grounding in actual codebase |
| 3 | **Dependency Accuracy** | 0-100% | Count correct include + link dependencies / total dependency claims | Understanding of build graph |
| 4 | **Implementation Readiness** | 0-3 | 0=description only, 1=can plan, 2=can specify, 3=can implement | Bridge from knowledge to code |
| 5 | **Unsupported Assumptions** | count | Count claims that cannot be verified against HSE ground truth or authoritative sources | Honesty about unknowns |
| 6 | **Acceptance Quality** | 0-3 | 0=none, 1=vague, 2=measurable, 3=verifiable with test cases | Quality of proposed acceptance criteria |
| 7 | **Verification Quality** | 0-3 | 0=none, 1=manual, 2=automated, 3=regression-integrated | Quality of proposed verification plan |
| 8 | **Novelty/Generalization** | 0-3 | 0=memorized, 1=adapted, 2=novel combination, 3=generalizable insight | Ability to handle unseen problems |
| 9 | **Architecture Stability** | % | % of missions where architecture proposal changes between layers | Convergence signal |

### Control Variables

| Variable | Value | Rationale |
|----------|-------|-----------|
| Repository state | Commit 59ab54a (HEAD) | Fixed reference point |
| Benchmark missions | 10 missions (see below) | Representative sample across complexity tiers |
| Scoring rubric | 5 dimensions × 0-2 = max 10 | Consistent with HSE-010 |
| Verifier | HSE-009 v2 (ground_truth.json) | Objective symbol verification |
| Knowledge corpus | HSE-011's 58 items from 15 sources | Measured, not estimated |

---

## Benchmark Missions (10)

### Tier A: Simple (3 missions)

| ID | Mission | Tests | Complexity |
|----|---------|-------|------------|
| M1 | Add texture mapping to a cube | Rendering pipeline, resource management, shader modification | LOW |
| M2 | Add a second camera with toggle | Scene management, camera system, input handling | LOW |
| M3 | Add color uniform to shader | Shader pipeline, uniform setting, renderer modification | LOW |

### Tier B: Intermediate (3 missions)

| ID | Mission | Tests | Complexity |
|----|---------|-------|------------|
| M4 | Add point light with attenuation | Lighting model, math (distance calc), shader programming | MEDIUM |
| M5 | Add scene hierarchy (parent-child transforms) | Scene graph, transform propagation, matrix math | MEDIUM |
| M6 | Add instanced rendering for 1000 cubes | GPU optimization, buffer management, draw call batching | MEDIUM |

### Tier C: Complex (2 missions)

| ID | Mission | Tests | Complexity |
|----|---------|-------|------------|
| M7 | Add physics integration (Box2D rigid bodies) | External library integration, game loop modification, timestep | HIGH |
| M8 | Add PBR material system | Material model, BRDF, texture pipeline, lighting equations | HIGH |

### Tier D: Cross-Domain (2 missions)

| ID | Mission | Tests | Complexity |
|----|---------|-------|------------|
| M9 | Add scene serialization (save/load) | Serialization, resource management, file I/O, format design | HIGH |
| M10 | Add basic editor with ImGui | UI integration, input handling, scene manipulation, renderer overlay | VERY HIGH |

---

## Condition Definitions

### Condition A: Source Code Only (L0)

**Knowledge included**:
- HSE repository (27 source files, CMakeLists, 2 test files)
- Ground truth: 10 classes, 8 structs, 3 enums, 125 functions, 127 includes

**Knowledge excluded**:
- ALL external knowledge (58 items)

**Expected performance**:
- Architecture Score: 3.5-4.0/10
- Symbol Accuracy: 100% (ground truth)
- Dependency Accuracy: 100% (ground truth)
- Implementation Readiness: 0 (description only)
- Unsupported Assumptions: 0 (no claims beyond observations)
- Acceptance Quality: 0 (no criteria proposed)
- Verification Quality: 0 (no verification proposed)
- Novelty: 0 (can only restate what exists)

**What this tests**: Pure observational capability. Can the AI describe HSE accurately without any domain knowledge?

---

### Condition B: Source + C++ (L0 + L1)

**Knowledge included**:
- Condition A corpus
- S1: cppreference.com (B) — smart pointers, containers, threading
- S2: C++ Core Guidelines (B) — ownership, RAII, error handling
- Items: KNOW-CPP-001 through KNOW-CPP-005

**Knowledge excluded**:
- All rendering, math, engine architecture, physics, audio, animation, UI, profiling, build knowledge

**Expected performance**:
- Architecture Score: 4.5-5.5/10
- Symbol Accuracy: 100%
- Dependency Accuracy: 100%
- Implementation Readiness: 1 (can plan code quality improvements)
- Unsupported Assumptions: 1-2 (may assume C++ patterns without rendering context)
- Acceptance Quality: 1 (vague criteria like "use RAII")
- Verification Quality: 0
- Novelty: 1 (can adapt C++ patterns to HSE)

**What this tests**: Does C++ knowledge alone improve architectural reasoning? Can the AI identify HSE's C++ anti-patterns (over-use of shared_ptr, deleted move, Rule of Five violations)?

**Key improvements expected**:
- Identify Primitive's Rule of Five violation
- Recommend unique_ptr over shared_ptr for scene objects
- Suggest RAII wrapper for GPU resources
- Identify const-correctness issues

---

### Condition C: Source + Rendering (L0 + L2)

**Knowledge included**:
- Condition A corpus
- S5: OpenGL 3.3 Core Spec (A) — API behavior
- S6: docs.gl (B) — function reference
- S7: LearnOpenGL (F) — implementation patterns
- S8: LearnOpenGL Source (D) — working code
- S9: Khronos Wiki (B) — best practices
- Items: KNOW-GL-001 through KNOW-GL-003, KNOW-GL-TEX-001 through KNOW-GL-TEX-004, KNOW-CUBEMAP-001 through KNOW-CUBEMAP-003, KNOW-RENDER-001 through KNOW-RENDER-008, KNOW-DEPTH-001 through KNOW-DEPTH-004, KNOW-BLEND-001 through KNOW-BLEND-003

**Knowledge excluded**:
- C++ knowledge (L1), math (L3), engine architecture (L4-L8), build/test (L9)

**Expected performance**:
- Architecture Score: 6.0-7.0/10
- Symbol Accuracy: 100% + OpenGL API symbols
- Dependency Accuracy: 100%
- Implementation Readiness: 2 (can specify rendering features)
- Unsupported Assumptions: 2-3 (may assume rendering patterns without engine context)
- Acceptance Quality: 2 (measurable criteria like "glGetError returns GL_NO_ERROR")
- Verification Quality: 1 (manual verification against spec)
- Novelty: 2 (can generate novel rendering solutions)

**What this tests**: Does rendering knowledge alone provide the biggest single-domain gain? This is the critical test of HSE-011's hypothesis that HSE-007's "diminishing returns at L3" was actually a "diminishing returns at L2" problem.

**Key improvements expected**:
- Design texture mapping architecture with correct OpenGL calls
- Design lighting system with proper shader uniforms
- Identify depth testing gaps
- Propose framebuffer object usage
- Design cubemap/skybox rendering

---

### Condition D: Source + Rendering + Math (L0 + L2 + L3)

**Knowledge included**:
- Condition C corpus
- S14: GLM (D) — standard math library
- S15: GLM Manual (B) — type and function reference
- S16: Eigen (D) — expression template alternative
- Items: KNOW-GLM-001 through KNOW-GLM-004

**Knowledge excluded**:
- C++ knowledge (L1), engine architecture (L4-L8), build/test (L9)

**Expected performance**:
- Architecture Score: 7.0-7.5/10
- Symbol Accuracy: 100% + OpenGL + GLM API symbols
- Dependency Accuracy: 100%
- Implementation Readiness: 2-3 (can specify and partially implement)
- Unsupported Assumptions: 1-2 (math provides precise specifications)
- Acceptance Quality: 2-3 (measurable with mathematical verification)
- Verification Quality: 1-2 (mathematical proofs + manual testing)
- Novelty: 2-3 (can combine math + rendering for novel solutions)

**What this tests**: Does adding math knowledge to rendering create a multiplicative effect? Can the AI now design transformations, projections, and interpolation correctly?

**Key improvements expected**:
- Design correct MVP matrix pipeline
- Propose quaternion rotation for camera/objects
- Design frustum culling with plane extraction
- Implement correct lighting calculations (dot products, falloff)
- Design bounding volume intersection tests

---

### Condition E: Source + Rendering + Math + Engine Architecture (L0 + L2 + L3 + L4)

**Knowledge included**:
- Condition D corpus
- S17: Godot Source (D) — scene graph, resources, renderer
- S18: Hazel (D) — event system, layer stack
- S19: raylib (D) — simple API design
- S20: bgfx (D) — rendering abstraction
- S26: Game Engine Architecture (E) — textbook foundation
- Items: resource_management items (3), event_system items (2)

**Knowledge excluded**:
- C++ knowledge (L1), resource/serialization (L5), scene/ECS (L6), UI (L7), physics/audio/animation (L8), build/test (L9)

**Expected performance**:
- Architecture Score: 8.0-8.5/10
- Symbol Accuracy: 100% + all API symbols
- Dependency Accuracy: 100%
- Implementation Readiness: 3 (can implement with full architecture)
- Unsupported Assumptions: 1 (engine architecture patterns resolve most ambiguity)
- Acceptance Quality: 3 (verifiable with test cases)
- Verification Quality: 2 (automated testing + architectural review)
- Novelty: 3 (can generate generalizable engine architecture insights)

**What this tests**: Does engine architecture knowledge provide the second major inflection point? Can the AI now evaluate HSE against industry patterns and propose architectural improvements?

**Key improvements expected**:
- Evaluate HSE's flat scene graph against tree/DAG alternatives
- Propose event system to decouple Bridge from Scene
- Design resource handle system with caching
- Recommend ECS migration path
- Propose renderer abstraction layer

---

### Condition F: Source + All Major Domains (L0 + L1-L8)

**Knowledge included**:
- Condition E corpus
- S1-S2: C++ (cppreference, Core Guidelines)
- S29-S30: Serialization (nlohmann/json, cereal)
- S27-S28: ECS (EnTT, Flecs)
- S31-S32: UI (Dear ImGui)
- S33-S34: Animation (glTF, Assimp)
- S35-S37: Physics (Box2D, Bullet, Jolt)
- S38-S39: Audio (miniaudio, OpenAL)
- All 58 knowledge items from 15 sources

**Knowledge excluded**:
- Build/test/profiling knowledge (L9), integrated synthesis (L10)

**Expected performance**:
- Architecture Score: 8.8-9.2/10
- Symbol Accuracy: 100%
- Dependency Accuracy: 100%
- Implementation Readiness: 3 (can implement any feature)
- Unsupported Assumptions: 0-1 (all domains covered)
- Acceptance Quality: 3 (verifiable across all domains)
- Verification Quality: 2-3 (automated + cross-domain)
- Novelty: 3 (can combine patterns across all domains)

**What this tests**: Does full domain coverage (minus build/test) approach ceiling performance? Are there diminishing returns from L8 knowledge?

**Key improvements expected**:
- All E-condition improvements
- Design physics integration with correct timestep
- Design audio system with spatialization
- Design animation pipeline from glTF
- Design ECS component layout
- Design ImGui editor overlay

---

### Condition G: Full Authoritative Corpus (L0-L10)

**Knowledge included**:
- ALL 58 knowledge items from ALL 15 sources
- ALL 47 sources available for reference
- ALL 126+ unique concepts
- Build/test/profiling knowledge (CMake, Google Test, Tracy)
- Integrated HSE-specific synthesis

**Knowledge excluded**: Nothing

**Expected performance**:
- Architecture Score: 9.2-9.6/10
- Symbol Accuracy: 100%
- Dependency Accuracy: 100%
- Implementation Readiness: 3
- Unsupported Assumptions: 0
- Acceptance Quality: 3
- Verification Quality: 3 (regression-integrated)
- Novelty: 3
- Architecture Stability: >90% (converged)

**What this tests**: What is the ceiling performance with full authoritative knowledge? Is there any remaining gap between Condition F and G?

**Key improvements expected**:
- All F-condition improvements
- CI/CD pipeline design
- Tracy profiling integration plan
- Google Test migration strategy
- Code quality tooling recommendations
- Cross-domain synthesis (rendering + physics + audio in one game loop)

---

## Execution Protocol

### For Each Condition

1. **Prepare Knowledge Corpus**
   - Compile all knowledge items for the condition
   - Format as structured context (JSON + markdown)
   - Estimate total word count

2. **Run All 10 Missions**
   - For each mission, provide the AI with:
     - The knowledge corpus for the current condition
     - The HSE repository state (ground truth)
     - The mission specification
   - Collect the AI's architectural proposal

3. **Score Each Mission**
   - Architecture Score (1-10): Expert evaluation
   - Symbol Accuracy: HSE-009 verifier on all symbol claims
   - Dependency Accuracy: Check include + link claims against CMakeLists
   - Implementation Readiness: 0-3 scale
   - Unsupported Assumptions: Count unverifiable claims
   - Acceptance Quality: 0-3 scale
   - Verification Quality: 0-3 scale
   - Novelty: 0-3 scale

4. **Track Architecture Stability**
   - Compare architecture proposals between consecutive conditions
   - Count how many missions change their architecture between layers

5. **Record Results**
   - Update ABLATION_RESULTS.md with scores
   - Update architecture change matrix
   - Calculate marginal gains

### Scoring Rubric (Per Mission)

| Dimension | 0 | 1 | 2 |
|-----------|---|---|---|
| **Symbol Accuracy** | >50% errors | 10-50% errors | <10% errors |
| **Architecture Quality** | Incoherent or wrong | Reasonable but incomplete | Well-structured and correct |
| **Implementation Readiness** | Description only | Can plan steps | Can implement directly |
| **Acceptance Criteria** | None | Vague ("should work") | Measurable and testable |
| **Verification Plan** | None | Manual testing | Automated + regression |

**Max per dimension**: 2
**Max per mission**: 10 (5 dimensions × 2)

---

## Saturation Detection Criteria

### TRUE SATURATION
- Architecture stability: <10% of missions change architecture between Condition F and G
- Score plateau: <5% improvement from F to G
- Symbol accuracy: plateau at 100%
- Knowledge gaps: <5% of high-value gaps remain

### PARTIAL SATURATION
- Architecture stability: 10-30% of missions change
- Score improvement: 5-15% from F to G
- Some domains saturated, others continue

### FALSE SATURATION (HSE-007's Error)
- Architecture continues changing >30% between F and G
- Score improvement >15% from F to G
- New knowledge corrects architectural mistakes

### NOT YET SATURATED
- Architecture changes >50% between F and G
- Score improvement >20%
- Major new capabilities emerge

---

## Comparison with HSE-010 Framework

| Aspect | HSE-010 | HSE-011 |
|--------|---------|---------|
| Layers | L0-L10 (theoretical) | A-G (grounded in actual sources) |
| Corpus size | ~84K words (planned) | ~500K+ words (available) |
| Knowledge items | 0 (self-generated) | 58 (from 15 sources) |
| Source quality | G (self-generated) | A-D (authoritative) |
| Missions | 25 | 10 (focused) |
| Execution | Template only | Actual execution planned |
| Verification | None | HSE-009 verifier |

---

## Risk Mitigation

| Risk | Mitigation |
|------|-----------|
| **Evaluator bias** (same AI scores its own output) | Use HSE-009 verifier for symbol claims; architecture scoring uses explicit rubric |
| **Mission too easy** | Include Tier C and D missions with high complexity |
| **Mission too hard** | All missions are grounded in HSE's actual architecture — not hypothetical |
| **Knowledge ordering effects** | Conditions are cumulative — each adds to previous |
| **Source quality variation** | Classification system (A-G) with weighting |
| **Time constraints** | 10 missions × 7 conditions = 70 evaluations — manageable |
