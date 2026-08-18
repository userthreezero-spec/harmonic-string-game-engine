# HSE-010 ABLATION STUDY — Design and Framework

## Study Design

### Independent Variable
Knowledge layer (L0-L10), added cumulatively.

### Dependent Variables
1. Architecture quality score (0-10 per mission)
2. Symbol accuracy (HSE-009 verifier)
3. Dependency accuracy (include + link)
4. Implementation readiness (0-2)
5. Acceptance criteria quality (0-2)
6. Unsupported assumptions count
7. Verification corrections needed
8. Novel concept handling

### Control Variables
- Repository state (fixed at commit 9837936)
- Benchmark missions (25 missions, fixed)
- Scoring rubric (5 dimensions × 0-2 = max 10)
- Verifier (HSE-009 v2)

---

## Knowledge Layer Definitions

### Layer A: Repository Only (Baseline)

**Corpus**: 27 HSE source files, CMakeLists.txt, 2 test files
**Size**: ~2,373 lines of code
**Source**: Repository ground truth

**Expected benchmark performance**:
- Symbol grounding: 100% (HSE-009 verified)
- Architecture quality: 3-4/10 (can identify but not design)
- Implementation readiness: LOW (can see code, can't plan features)

---

### Layer B: + C++ Engineering

**Added corpus**:
- C++20 features relevant to game engines
- Smart pointer patterns (unique_ptr, shared_ptr, weak_ptr)
- RAII principles for resource management
- Move semantics and perfect forwarding
- Const correctness patterns
- Error handling strategies
- Template basics for generic programming

**Estimated size**: ~5,000 words
**Source type**: Technical documentation (cppreference, C++ Core Guidelines)

**Expected marginal improvement**: HIGH (+1.0-1.5)
**Reasoning**: C++ patterns directly improve code quality judgments

---

### Layer C: + Software Architecture

**Added corpus**:
- SOLID principles applied to game engines
- Design patterns: Command, Observer, Factory, Strategy, State
- Interface design and API boundaries
- Layered architecture evaluation
- Event-driven architecture patterns
- Error handling architectures

**Estimated size**: ~8,000 words
**Source type**: Architecture texts, design pattern references

**Expected marginal improvement**: HIGH (+1.0-1.5)
**Reasoning**: Architecture patterns amplify domain knowledge

---

### Layer D: + C++ Implementation Engineering

**Added corpus**:
- RAII wrappers for OpenGL objects (VAO, VBO, EBO, textures)
- GPU resource lifetime management patterns
- Thread-safe data structures (SPSC, MPSC, lock-free queues)
- Memory allocator patterns (pool, stack, arena)
- Serialization patterns (JSON, binary, reflection)
- API boundary design (PIMPL, modules)
- Testing patterns (unit, integration, GPU tests)
- Build system management (CMake targets, dependencies)

**Estimated size**: ~10,000 words
**Source type**: Real C++ implementations, RAII guides, build system docs

**Expected marginal improvement**: HIGH (+1.0-1.5)
**Reasoning**: Implementation patterns bridge theory and practice

---

### Layer E: + 3D Mathematics

**Added corpus**:
- Vector operations (add, subtract, dot, cross, normalize, lerp, slerp)
- Matrix operations (multiply, inverse, transpose, determinant)
- Transform hierarchy (model → world → view → projection)
- Quaternion math (rotation, interpolation, gimbal lock avoidance)
- Projection matrix derivation
- Bounding volumes (AABB, sphere, OBB)
- Frustum math (plane extraction, culling tests)
- Ray casting and intersection tests
- Coordinate system conversions

**Estimated size**: ~8,000 words
**Source type**: Math textbooks, graphics programming references

**Expected marginal improvement**: MEDIUM (+0.5-1.0)
**Reasoning**: Math is essential but more specialized

---

### Layer F: + Rendering Engineering

**Added corpus**:
- OpenGL 3.3 core profile specification details
- Shader programming (vertex, fragment, geometry, compute)
- Buffer management (VAO, VBO, EBO, UBO, SSBO)
- Texture pipeline (loading, formats, filtering, mipmapping)
- Lighting models (Phong, PBR, image-based lighting)
- Render passes (forward, deferred, clustered)
- Post-processing (bloom, tone mapping, SSAO, FXAA)
- Instancing and batch rendering
- GPU synchronization (fences, barriers, queries)
- Frame lifecycle (clear, render, post-process, present)
- Debug tooling (GL debug callbacks, profiling)

**Estimated size**: ~15,000 words
**Source type**: OpenGL specification, rendering guides, GPU programming docs

**Expected marginal improvement**: HIGH (+1.0-1.5)
**Reasoning**: Rendering is HSE's primary domain

---

### Layer G: + Game Engine Architecture

**Added corpus**:
- Scene graph patterns (flat, tree, DAG)
- ECS architectures (sparse set, archetype, bitfield)
- Resource management (handles, reference counting, caching)
- Asset pipeline (loading, processing, hot-reloading)
- World/level management
- Runtime/editor separation
- Physics integration patterns
- Audio integration patterns

**Estimated size**: ~10,000 words
**Source type**: Engine architecture texts, game engine development books

**Expected marginal improvement**: MEDIUM (+0.5-1.0)
**Reasoning**: Engine architecture builds on rendering knowledge

---

### Layer H: + Tooling and Editor Architecture

**Added corpus**:
- Immediate-mode UI (Dear ImGui patterns)
- Retained-mode UI patterns
- Scene hierarchy panels
- Property inspectors
- Viewport interaction (gizmos, selection, manipulation)
- Asset browsers
- Undo/redo systems (command pattern, snapshot)
- Multi-window management
- Keyboard/mouse input handling

**Estimated size**: ~8,000 words
**Source type**: Editor development guides, ImGui documentation

**Expected marginal improvement**: LOW-MEDIUM (+0.3-0.7)
**Reasoning**: Editor is future concern, not current blocker

---

### Layer I: + Existing Engine Architecture Studies

**Added corpus** (analysis of real engines):
- Godot: Scene system, resource system, editor architecture
- bgfx: Cross-platform rendering abstraction
- Hazel: Window abstraction, event system, renderer layer
- Filament: PBR rendering, material system
- raylib: Simple API design, immediate-mode philosophy
- OGRE: Resource management, scene manager pattern
- EnTT: ECS implementation, entity management
- Dear ImGui: Immediate-mode UI, widget system

**Estimated size**: ~50,000 lines of source code analysis
**Source type**: Real open-source engine source code

**Expected marginal improvement**: MEDIUM (+0.5-1.0)
**Reasoning**: Real implementations provide concrete reference points

---

### Layer J: + Production Software Construction

**Added corpus**:
- Build system management (CMake, conan, vcpkg)
- CI/CD pipelines (GitHub Actions)
- Code quality tools (clang-tidy, clang-format)
- Profiling tools (tracy, superluminal)
- Debugging techniques (GDB, LLDB, Visual Studio)
- Memory debugging (AddressSanitizer, leak detection)
- Crash handling (stack traces, minidump)
- Logging frameworks (spdlog)
- Configuration management (JSON, TOML)
- Packaging and distribution

**Estimated size**: ~5,000 words
**Source type**: DevOps guides, production engineering references

**Expected marginal improvement**: LOW (+0.2-0.5)
**Reasoning**: Production practices are important but not blocking

---

### Layer K: + Architecture-to-Implementation Practice

**Added corpus** (case studies):
- Case study: Adding texture mapping to a simple renderer
- Case study: Implementing a scene hierarchy from scratch
- Case study: Building a resource management system
- Case study: Adding undo/redo to an editor
- Case study: Implementing instanced rendering
- Case study: Building a job system
- Case study: Adding serialization to a scene graph
- Case study: Implementing a material system
- Case study: Building a basic editor
- Case study: Adding GPU profiling

**Estimated size**: ~15,000 words
**Source type**: Implementation guides with real code examples

**Expected marginal improvement**: MEDIUM-HIGH (+0.7-1.2)
**Reasoning**: Bridges the gap between knowledge and implementation

---

## Expected Results (Hypothesis)

| Layer | Cumulative Corpus | Expected Avg Score | Marginal Gain | Cumulative Gain |
|-------|-------------------|-------------------|---------------|-----------------|
| A (L0) | ~2,373 lines | 3.8/10 | — | — |
| B (+C++) | +5,000 words | 5.0/10 | +1.2 | +1.2 |
| C (+Arch) | +8,000 words | 6.2/10 | +1.2 | +2.4 |
| D (+Impl) | +10,000 words | 7.4/10 | +1.2 | +3.6 |
| E (+Math) | +8,000 words | 8.0/10 | +0.6 | +4.2 |
| F (+Render) | +15,000 words | 8.8/10 | +0.8 | +5.0 |
| G (+Engine) | +10,000 words | 9.2/10 | +0.4 | +5.4 |
| H (+Editor) | +8,000 words | 9.4/10 | +0.2 | +5.6 |
| I (+Engines) | +50,000 lines | 9.6/10 | +0.2 | +5.8 |
| J (+Prod) | +5,000 words | 9.7/10 | +0.1 | +5.9 |
| K (+Practice) | +15,000 words | 9.9/10 | +0.2 | +6.1 |

**Key prediction**: The "knee" in HSE-007 was at L3 (architecture patterns) because the corpus was too small. With a larger corpus, the knee shifts to L5-L6 (rendering + engine architecture), and meaningful gains continue through L10.

---

## Measurement Protocol

### For Each Layer

1. **Baseline verification**: Run HSE-009 verifier on repository (unchanged)
2. **Benchmark execution**: Run all 25 missions with cumulative knowledge
3. **Symbol verification**: Run HSE-009 verifier on all symbol claims
4. **Architecture scoring**: Score each mission on 5 dimensions
5. **Architecture change tracking**: Record whether proposed architecture changes
6. **Novelty testing**: Run Tier E missions to test for memorization
7. **Documentation**: Record all results in ABLATION_RESULTS.md

### Architecture Change Tracking

For each layer, record:

| Mission | Boundary Changed? | Symbol Changed? | Dependency Changed? | Strategy Changed? |
|---------|-------------------|-----------------|---------------------|-------------------|
| A1 | Y/N | Y/N | Y/N | Y/N |
| A2 | Y/N | Y/N | Y/N | Y/N |
| ... | ... | ... | ... | ... |

This is the primary signal for saturation:
- If architecture STOPS changing after a layer → saturation approaching
- If architecture CONTINUES changing → not yet saturated

---

## Saturation Criteria

### TRUE SATURATION
- Architecture stability: <10% of missions change architecture after layer K
- Score plateau: <5% improvement in last 2 layers
- Symbol accuracy: plateau at >95%
- Knowledge gaps: <5% of high-value gaps remain open

### PARTIAL SATURATION
- Architecture stability: 10-30% of missions change architecture
- Score plateau: 5-15% improvement in last 2 layers
- Some domains saturated, others continue improving

### FALSE SATURATION
- Architecture continues changing significantly (>30% of missions)
- Score improvement >15% in last 2 layers
- New knowledge continues correcting architectural mistakes

### UNKNOWN
- Insufficient data to determine saturation
- Benchmark too easy to differentiate layers
- Knowledge retrieval failures mask true capability

---

## Execution Plan

### Phase 1: Framework Setup (Current)
- Define all layers
- Create benchmark missions
- Establish scoring rubric
- Document methodology

### Phase 2: Baseline Execution
- Run Layer A (repository only) across all 25 missions
- Establish baseline scores

### Phase 3: Layer-by-Layer Execution
- For each layer B through K:
  1. Add knowledge to corpus
  2. Run all 25 missions
  3. Record scores and architecture changes
  4. Run HSE-009 verifier on symbol claims
  5. Update ABLATION_RESULTS.md

### Phase 4: Analysis
- Calculate marginal gains per layer
- Identify diminishing returns point
- Assess architecture stability
- Determine saturation classification

### Phase 5: Reporting
- Complete all deliverables
- Write final findings
- Make recommendation for HSE-011
