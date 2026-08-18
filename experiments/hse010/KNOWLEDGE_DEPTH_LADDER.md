# HSE-010 KNOWLEDGE DEPTH LADDER

## Purpose

Define 11 structured knowledge layers (L0-L10) for the ablation study. Each layer adds specific, documentable knowledge. The ablation study tests whether adding each layer improves architectural reasoning.

---

## L0 — Repository Ground Truth

**What it is**: Actual HSE source code and build configuration.

**Sources**:
- 27 source files (headers + implementations)
- CMakeLists.txt
- 2 test files

**Knowledge provided**:
- Class/struct/enum definitions
- Method signatures and implementations
- Field inventories
- Include relationships
- Build targets and dependencies
- Thread model (bridge reader + GL main)
- Command protocol (18 types)
- Data flow (pipe → queue → GL thread)

**Expected value**: Enables symbol grounding. Can answer "what exists" but not "what should we build."

**HSE-007 score**: 3.8/10

---

## L1 — Basic C++ Engineering

**What it is**: C++ language features relevant to game engine development.

**Sources to ingest**:
- C++20 core features (concepts, ranges, coroutines)
- Smart pointer patterns (unique_ptr, shared_ptr, weak_ptr)
- RAII principles and resource management
- Move semantics and perfect forwarding
- STL containers and algorithms relevant to engines
- Lambda expressions and closures
- Template basics (enough for generic programming)
- Const correctness and constexpr

**Specific knowledge**:
- When to use unique_ptr vs shared_ptr (HSE uses too much shared_ptr)
- RAII for GL objects (Primitive::uploadGPU manual management → RAII wrapper)
- Move semantics for scene transfers (Primitive has deleted move)
- Const correctness for render methods (bind() should be const)

**Expected value**: Improves code quality judgments. Helps identify HSE's C++ anti-patterns.

**HSE-007 score**: Not tested as separate layer

---

## L2 — Software Architecture Fundamentals

**What it is**: General software architecture patterns and principles.

**Sources to ingest**:
- SOLID principles (especially Single Responsibility, Dependency Inversion)
- Design patterns relevant to engines (Command, Observer, Factory, Strategy)
- Interface design and API boundaries
- Layered architecture patterns
- Event-driven architecture
- State machine patterns
- Error handling strategies (exceptions vs error codes vs expected)
- Configuration management

**Specific knowledge**:
- HSE's Bridge uses Command pattern correctly
- HSE lacks Observer pattern (no event bus)
- HSE's Renderer uses pimpl correctly
- Primitive violates Single Responsibility (geometry + GPU + state)
- No interface abstraction between Bridge and Scene

**Expected value**: Improves architectural evaluation. Helps identify design trade-offs.

**HSE-007 score**: Partially tested in L3

---

## L3 — C++ Implementation Engineering

**What it is**: Real-world C++ patterns for game engine construction.

**Sources to ingest**:
- RAII wrappers for OpenGL objects (VAO, VBO, EBO, textures)
- GPU resource lifetime management
- Thread-safe data structures (SPSC, MPSC, lock-free)
- Memory allocators (pool, stack, arena)
- String handling (SSO, string_view, interning)
- Serialization patterns (JSON, binary, reflection)
- API boundary design (public/private, PIMPL, modules)
- Testing patterns (unit, integration, GPU, benchmarks)
- Build system management (CMake targets, dependencies)

**Specific knowledge**:
- GLObject RAII wrapper: glGenVertexArrays in constructor, glDeleteVertexArrays in destructor
- Resource handle pattern: index + generation for stable references
- Job system pattern: thread pool + task queue + futures
- Scene serialization: recursive traversal + property reflection
- GPU resource tracking: allocation/deallocation audit trail

**Expected value**: Enables implementation-grade architectural decisions.

**HSE-007 score**: Not tested

---

## L4 — 3D Mathematics

**What it is**: Mathematical foundations for 3D rendering and simulation.

**Sources to ingest**:
- Vector operations (add, subtract, dot, cross, normalize, lerp)
- Matrix operations (multiply, inverse, transpose, determinant)
- Transform hierarchy (model → world → view → projection)
- Quaternion math (rotation, interpolation, gimbal lock avoidance)
- Projection matrices (perspective, orthographic derivation)
- Coordinate systems (world, view, screen, tangent space)
- Interpolation (lerp, slerp, smoothstep, easing curves)
- Bounding volumes (AABB, sphere, OBB)
- Frustum math (plane extraction, culling tests)
- Ray casting and intersection tests

**Specific knowledge**:
- HSE's Mat4::perspective uses standard OpenGL projection
- HSE uses Euler angles (gimbal lock risk)
- No interpolation utilities exist
- No bounding volume support
- No frustum culling

**Expected value**: Enables animation, physics, procedural geometry decisions.

**HSE-007 score**: Partially tested in L2

---

## L5 — Rendering Engineering

**What it is**: Deep understanding of real-time rendering pipelines.

**Sources to ingest**:
- OpenGL 3.3 core profile specification
- Shader programming (vertex, fragment, geometry, compute)
- Buffer management (VAO, VBO, EBO, UBO, SSBO)
- Texture pipeline (loading, formats, filtering, mipmaping)
- Lighting models (Phong, PBR, image-based lighting)
- Render passes (forward, deferred, clustered)
- Post-processing (bloom, tone mapping, SSAO, FXAA)
- Instancing and batch rendering
- GPU synchronization (fences, barriers, queries)
- Frame lifecycle (clear, render, post-process, present)
- Debug tooling (GL debug callbacks, profiling, frame capture)

**Specific knowledge**:
- HSE's shader is hardcoded string (renderer.cpp:13-31)
- HSE uses forward rendering, single pass
- No texture support, no lighting
- VAO/VBO/EBO created per Primitive (no sharing)
- glReadPixels for frame capture (blocking, slow)

**Expected value**: Enables rendering feature development. Critical for HSE's next features.

**HSE-007 score**: Partially tested in L2

---

## L6 — Game Engine Architecture

**What it is**: Architecture patterns specific to game engines.

**Sources to ingest**:
- Scene graph patterns (flat, tree, DAG)
- Entity/Component/System (ECS) architectures
- Resource management (handles, reference counting, caching, streaming)
- Asset pipeline (loading, processing, hot-reloading)
- World/level management
- Physics integration patterns
- Audio integration patterns
- Scripting integration patterns
- Console/platform abstraction
- Runtime/editor separation

**Specific knowledge**:
- HSE uses flat scene graph (vector<shared_ptr<Primitive>>)
- No ECS (direct class hierarchy)
- No resource management (manual GL calls)
- No asset pipeline (files loaded directly)
- No runtime/editor separation

**Expected value**: Enables architectural decisions about HSE's growth path.

**HSE-007 score**: Partially tested in L3

---

## L7 — Tooling and Editor Architecture

**What it is**: Architecture for development tools and editors.

**Sources to ingest**:
- Immediate-mode UI (Dear ImGui patterns)
- Retained-mode UI patterns
- Scene hierarchy panels
- Property inspectors
- Viewport interaction (gizmos, selection, manipulation)
- Asset browsers
- Undo/redo systems (command pattern, snapshot)
- Serialization for editor state
- Multi-window management
- Keyboard/mouse input handling

**Specific knowledge**:
- HSE has no editor (all control via bridge)
- Bridge protocol could support editor commands
- No undo/redo exists
- No property inspector
- No viewport interaction

**Expected value**: Enables editor development decisions.

**HSE-007 score**: Not tested

---

## L8 — Existing Engine Architecture Studies

**What it is**: Analysis of real open-source engines and frameworks.

**Sources to ingest** (specific projects):
- **Godot**: Scene system, resource system, editor architecture
- **bgfx**: Cross-platform rendering abstraction
- **Hazel**: Window abstraction, event system, renderer layer
- **Filament**: PBR rendering, material system
- **raylib**: Simple API design, immediate-mode philosophy
- **OGRE**: Resource management, scene manager pattern
- **EnTT**: ECS implementation, entity management
- **Dear ImGui**: Immediate-mode UI, widget system

**Analysis framework per engine**:
1. How does it manage GPU resources?
2. How does it handle scene hierarchy?
3. What is its threading model?
4. How does it serialize state?
5. What patterns could HSE adopt?

**Specific knowledge**:
- Godot uses RID (Resource ID) handle system
- bgfx uses render command abstraction (bgfx::Encoder)
- Hazel uses event system with dispatcher
- Filament uses PBR with material instances
- raylib uses single-header, C API simplicity
- OGRE uses ResourceManager with reference counting
- EnTT uses sparse set ECS
- Dear ImGui uses immediate-mode, no retained state

**Expected value**: Provides real-world reference points. Validates or challenges architectural assumptions.

**HSE-007 score**: Not tested

---

## L9 — Production Software Construction

**What it is**: Practices for production-quality software.

**Sources to ingest**:
- Build system management (CMake, conan, vcpkg)
- CI/CD pipelines (GitHub Actions, testing)
- Code quality tools (clang-tidy, clang-format, cppcheck)
- Profiling tools (tracy, superluminal, perf)
- Debugging techniques (GDB, LLDB, Visual Studio debugger)
- Memory debugging (valgrind, AddressSanitizer, leak detection)
- Crash handling (stack traces, minidump, crash reporting)
- Logging frameworks (spdlog, custom)
- Configuration management (JSON, TOML, command-line)
- Packaging and distribution (installers, portable builds)

**Specific knowledge**:
- HSE uses CMake but no CI/CD
- No code quality tools configured
- No profiling infrastructure
- Basic debugging (print statements)
- No crash handling
- No logging framework
- JSON configuration via SceneBuilder

**Expected value**: Enables production-quality decisions.

**HSE-007 score**: Not tested

---

## L10 — Architecture-to-Implementation Practice

**What it is**: Real examples of the full pipeline from requirement to implementation.

**Sources to ingest**:
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

**Each case study covers**:
1. Requirement analysis
2. Architecture design
3. Subsystem decomposition
4. Class design
5. Method implementation
6. Testing strategy
7. Integration points
8. Common pitfalls

**Specific knowledge**:
- How to trace requirement → architecture → code
- How to identify all affected files and symbols
- How to design acceptance criteria
- How to test GPU code
- How to integrate with existing systems

**Expected value**: Bridges the gap between architectural knowledge and implementation capability. This is the most critical layer for HSE.

**HSE-007 score**: Partially tested in L4

---

## Layer Interaction Matrix

| Layer | Depends On | Amplifies | Tested By |
|-------|-----------|-----------|-----------|
| L0 | None | All layers | Symbol grounding |
| L1 | L0 | L2, L3 | Code quality |
| L2 | L1 | L3, L6 | Architecture evaluation |
| L3 | L1, L2 | L5, L6 | Implementation readiness |
| L4 | L0 | L5, L6 | Math understanding |
| L5 | L0, L4 | L3, L6 | Rendering features |
| L6 | L2, L5 | L7, L8 | Engine design |
| L7 | L2, L6 | L9 | Tooling decisions |
| L8 | L2, L5, L6 | L10 | Reference validation |
| L9 | L1, L2 | L7, L10 | Production readiness |
| L10 | All | Implementation | End-to-end capability |

---

## Expected Diminishing Returns Profile

Based on HSE-007's methodology (but with much larger corpus):

| Transition | Expected Gain | Reasoning |
|-----------|---------------|-----------|
| L0→L1 | HIGH | C++ fundamentals directly improve code quality |
| L1→L2 | HIGH | Architecture patterns amplify domain knowledge |
| L2→L3 | HIGH | Implementation patterns bridge theory and practice |
| L3→L4 | MEDIUM | Math is essential but more specialized |
| L4→L5 | HIGH | Rendering is HSE's primary domain |
| L5→L6 | MEDIUM | Engine architecture builds on rendering |
| L6→L7 | LOW-MEDIUM | Editor is future concern, not current |
| L7→L8 | MEDIUM | Real engine studies validate patterns |
| L8→L9 | LOW | Production practices are important but not blocking |
| L9→L10 | MEDIUM | Implementation practice ties everything together |

**Key hypothesis**: HSE-007's diminishing returns started at L3 because the corpus was too small. With a larger, more diverse corpus, the "knee" may shift to L5 or L6.

---

## Knowledge Source Plan Per Layer

| Layer | Primary Sources | Source Type | Estimated Size |
|-------|----------------|-------------|----------------|
| L0 | Repository | Code | 2,373 lines |
| L1 | C++ references, cppreference.com | Documentation | ~5,000 words |
| L2 | Architecture texts, design patterns | Technical texts | ~8,000 words |
| L3 | Real C++ implementations, RAII guides | Code + docs | ~10,000 words |
| L4 | Math textbooks, graphics programming | Technical texts | ~8,000 words |
| L5 | OpenGL spec, rendering guides | Documentation | ~15,000 words |
| L6 | Engine architecture texts | Technical texts | ~10,000 words |
| L7 | Editor development guides, ImGui docs | Documentation | ~8,000 words |
| L8 | Open-source engine source code | Real code | ~50,000 lines |
| L9 | DevOps/production guides | Documentation | ~5,000 words |
| L10 | Implementation case studies | Code + docs | ~15,000 words |
| **Total** | | | **~134,000 words + 50K lines** |

This is approximately **100x larger** than HSE-007's corpus.
