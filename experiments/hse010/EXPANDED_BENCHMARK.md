# HSE-010 EXPANDED BENCHMARK — 25 Missions Across 5 Tiers

## Purpose

Replace HSE-007's 5-mission benchmark with 25 missions covering simple to complex, familiar to novel. Each mission tests architectural reasoning against actual HSE source code.

---

## Scoring Rubric

Each mission scored on 5 dimensions (0-2 each, max 10):

| Dimension | 0 | 1 | 2 |
|-----------|---|---|---|
| **Symbol Accuracy** | Wrong files/symbols | Right area, wrong details | Exact files, classes, methods, fields |
| **Architecture Quality** | Would not compile/design broken | Partially correct architecture | Sound architecture, correct dependencies |
| **Implementation Readiness** | Cannot start coding | Could start with significant research | Could start coding immediately |
| **Acceptance Criteria** | No measurable criteria | Partial criteria | Complete, testable criteria |
| **Verification Plan** | No verification | Manual verification only | Automated verification with HSE-009 |

---

## Tier A — Simple (5 missions)

### A1: Add a new PrimitiveType (Pyramid)

**Mission**: Add a pyramid primitive type to HSE.

**Tests**: Enum extension, geometry generation, uploadGPU compatibility.

**Expected symbols**: `PrimitiveType::Pyramid`, `Primitive::generateGeometry()` new case

**Difficulty**: LOW — follows existing pattern exactly

**Novelty**: LOW — similar to existing Sphere stub

---

### A2: Modify primitive default color

**Mission**: Change the default color for new primitives from orange to white.

**Tests**: Field initialization, downstream effects, shader uniform unchanged.

**Expected symbols**: `Primitive::m_color` initialization in constructor

**Difficulty**: TRIVIAL — single line change

**Novelty**: TRIVIAL — simple value change

---

### A3: Add a bridge command (GET_PRIMITIVE_COUNT)

**Mission**: Add a new bridge command that returns the number of primitives in the scene.

**Tests**: Command type extension, handler implementation, JSON response.

**Expected symbols**: `Command::Type::CMD_GET_PRIMITIVE_COUNT`, `Bridge::executeCommand()` new case

**Difficulty**: LOW — follows existing command pattern

**Novelty**: LOW — follows existing pattern

---

### A4: Add camera reset command

**Mission**: Add a bridge command that resets camera to default position.

**Tests**: Command extension, camera state modification.

**Expected symbols**: `Command::Type::CMD_RESET_CAMERA`, `Camera::setPosition()`, `Camera::lookAt()`

**Difficulty**: LOW — straightforward command addition

**Novelty**: LOW — follows existing patterns

---

### A5: Add primitive name query

**Mission**: Add a bridge command that returns the name of a primitive by index.

**Tests**: Command handling, scene lookup, JSON serialization.

**Expected symbols**: `Command::Type::CMD_GET_PRIMITIVE_NAME`, `Scene::getPrimitive()`, `Primitive::getName()`

**Difficulty**: LOW — follows existing patterns

**Novelty**: LOW — extension of existing command pattern

---

## Tier B — Intermediate (5 missions)

### B1: Add texture mapping to primitives

**Mission**: Add UV coordinate support and texture loading to primitives.

**Tests**: Rendering pipeline, GPU resource management, shader modification.

**Expected symbols**: `m_texCoords` field, `m_textureID` field, shader uniform `uTexture`, `glActiveTexture`, `glBindTexture`

**Difficulty**: MEDIUM — requires shader modification, new VBO, texture pipeline

**Novelty**: MEDIUM — HSE-007 tested this, but with HSE-009 verification now available

**HSE-007 score**: 6→10 across L1-L5

---

### B2: Add point light to scene

**Mission**: Add a single point light with Phong lighting.

**Tests**: Shader modification, uniform management, lighting calculation.

**Expected symbols**: `Light` struct, shader uniforms `uLightPos`, `uLightColor`, `uLightIntensity`, fragment shader Phong calculation

**Difficulty**: MEDIUM — requires shader rewrite, normal usage, uniform binding

**Novelty**: MEDIUM — HSE-007 tested this

**HSE-007 score**: 4→10 across L1-L5

---

### B3: Add scene hierarchy (parent/child transforms)

**Mission**: Add parent-child relationships with transform inheritance.

**Tests**: Data structure change, matrix multiplication, scene management.

**Expected symbols**: `SceneNode` class, `m_parent` pointer, `m_children` vector, `getWorldTransform()` accumulation

**Difficulty**: MEDIUM-HIGH — requires scene graph redesign, flat→tree transition

**Novelty**: MEDIUM — HSE-007 tested this

**HSE-007 score**: 3→10 across L1-L5

---

### B4: Add GPU resource RAII wrapper

**Mission**: Create RAII wrappers for VAO, VBO, EBO to prevent resource leaks.

**Tests**: RAII pattern, move semantics, resource lifecycle.

**Expected symbols**: `GLObject` class, `glGenVertexArrays` in constructor, `glDeleteVertexArrays` in destructor, move constructor/assignment

**Difficulty**: MEDIUM — requires understanding GL resource lifecycle, move semantics

**Novelty**: HIGH — HSE-007 did NOT test this

---

### B5: Add scene serialization v2

**Mission**: Extend SceneBuilder to support hierarchy serialization (parent/child relationships).

**Tests**: Recursive serialization, JSON structure, backward compatibility.

**Expected symbols**: `SceneBuilder::exportState()` recursive, `SceneBuilder::importState()` recursive, `SceneNode` JSON format

**Difficulty**: MEDIUM — extends existing serialization to handle tree structure

**Novelty**: MEDIUM — builds on existing SceneBuilder

---

## Tier C — Complex (5 missions)

### C1: Add material system

**Mission**: Add a Material struct that encapsulates shader program, textures, and uniforms.

**Tests**: Resource management, shader program management, uniform binding.

**Expected symbols**: `Material` class, `ShaderProgram` class, `m_material` in Primitive, uniform cache, texture binding

**Difficulty**: HIGH — requires shader program management, resource lifetime, uniform caching

**Novelty**: HIGH — HSE-007 did not test material system design

---

### C2: Add render batching for 1000+ primitives

**Mission**: Implement batch rendering to reduce draw calls for many primitives.

**Tests**: Vertex merging, dynamic VBO updates, draw call optimization.

**Expected symbols**: `BatchRenderer` class, merged VBO, `glDrawElements` single call, dirty flags, vertex format

**Difficulty**: HIGH — requires deep understanding of GL pipeline, vertex formats, dynamic buffers

**Novelty**: HIGH — HSE-007 mentioned optimization but didn't design batch system

---

### C3: Add async asset loading

**Mission**: Add background asset loading with thread pool.

**Tests**: Threading, resource synchronization, GL context ownership.

**Expected symbols**: `AssetLoader` class, `ThreadPool`, `std::future`, GL context sharing, resource ready flags

**Difficulty**: HIGH — requires threading, GL context management, synchronization

**Novelty**: HIGH — HSE-007 did not test async patterns

---

### C4: Add render pass abstraction

**Mission**: Abstract the rendering pipeline into configurable render passes.

**Tests**: Pipeline design, render target management, pass dependencies.

**Expected symbols**: `RenderPass` class, `RenderTarget` class, `RenderPipeline` class, pass ordering, input/output attachments

**Difficulty**: HIGH — requires deep rendering knowledge, pipeline architecture

**Novelty**: HIGH — HSE-007 did not test render pass design

---

### C5: Add undo/redo system

**Mission**: Implement undo/redo for all scene modifications.

**Tests**: Command pattern, state snapshot, history management.

**Expected symbols**: `CommandHistory` class, `Command` base class, `UndoStack`, `RedoStack`, snapshot serialization

**Difficulty**: HIGH — requires command inversion, state serialization, memory management

**Novelty**: MEDIUM — HSE-007 tested this, but with deeper knowledge now

**HSE-007 score**: 3→10 across L1-L5

---

## Tier D — Cross-Domain (5 missions)

### D1: Add instanced rendering with frustum culling

**Mission**: Implement instanced rendering for many identical primitives, with frustum culling to skip off-screen objects.

**Tests**: Instance buffers, frustum extraction, AABB testing, draw call optimization.

**Expected symbols**: `InstancedRenderer`, `Frustum` class, `AABB` struct, `glDrawElementsInstanced`, instance VBO

**Difficulty**: VERY HIGH — combines rendering, math, and optimization

**Novelty**: HIGH — HSE-007 mentioned instancing but didn't design the system

---

### D2: Add shader hot-reload

**Mission**: Add runtime shader recompilation without restarting.

**Tests**: File watching, shader compilation, error handling, program switching.

**Expected symbols**: `ShaderWatcher`, `ShaderCompiler`, `glGetShaderInfoLog`, `glLinkProgram`, hot-reload trigger

**Difficulty**: VERY HIGH — combines file I/O, GL pipeline, error handling

**Novelty**: HIGH — HSE-007 did not test hot-reload

---

### D3: Add scene streaming

**Mission**: Add ability to load/unload scene sections dynamically.

**Tests**: Resource management, streaming, memory budgets, LOD.

**Expected symbols**: `SceneStreamer`, `ResourcePool`, `StreamingConfig`, load/unload triggers, memory tracking

**Difficulty**: VERY HIGH — combines resource management, memory, streaming

**Novelty**: HIGH — HSE-007 did not test streaming

---

### D4: Add GPU profiling and diagnostics

**Mission**: Add GPU timing, draw call counting, and memory tracking.

**Tests**: GL queries, timing, diagnostics, overlay display.

**Expected symbols**: `GPUProfiler`, `GLTimer`, `DrawCallCounter`, `glBeginQuery`, `glEndQuery`, diagnostic overlay

**Difficulty**: VERY HIGH — combines rendering, performance, debugging

**Novelty**: HIGH — HSE-007 did not test profiling

---

### D5: Add editor viewport with gizmos

**Mission**: Add interactive viewport with transform gizmos for object manipulation.

**Tests**: Mouse picking, ray casting, gizmo rendering, transform manipulation.

**Expected symbols**: `Viewport`, `Gizmo`, `RayCaster`, `MousePicker`, `gluUnProject` equivalent, gizmo shader

**Difficulty**: VERY HIGH — combines rendering, math, UI, interaction

**Novelty**: HIGH — HSE-007 did not test editor design

---

## Tier E — Novel/Unseen (5 missions)

These missions test concepts NOT present in HSE-007's original corpus.

### E1: GPU resource lifetime redesign

**Mission**: Redesign how HSE manages GPU resources (VAO, VBO, EBO, textures) to prevent leaks and enable sharing.

**Tests**: RAII, resource tracking, reference counting, sharing, cleanup.

**Expected symbols**: `GPUResourceManager`, `ResourceHandle`, `ResourceCache`, `GLObject` RAII wrapper, reference counting

**Difficulty**: VERY HIGH — requires redesigning Primitive's GPU resource management

**Novelty**: VERY HIGH — HSE-007 never addressed GPU resource lifetime

---

### E2: Texture atlas system

**Mission**: Add texture atlas support for batching multiple primitives with different textures.

**Tests**: Atlas packing, UV remapping, texture coordinate offset, batch compatibility.

**Expected symbols**: `TextureAtlas`, `AtlasPacker`, UV offset in vertex shader, atlas texture binding

**Difficulty**: VERY HIGH — combines rendering, math, resource management

**Novelty**: VERY HIGH — HSE-007 never discussed texture atlasing

---

### E3: Render command abstraction

**Mission**: Abstract raw GL calls into a render command buffer that can be reordered or batched.

**Tests**: Command buffer design, GL state management, command sorting, deferred execution.

**Expected symbols**: `RenderCommandBuffer`, `RenderCommand` struct, command types, state sorting, deferred execution

**Difficulty**: VERY HIGH — requires deep GL understanding, command pattern, state management

**Novelty**: VERY HIGH — HSE-007 never discussed render command abstraction

---

### E4: Material instance system

**Mission**: Add a material instance system where multiple primitives share a material template but have different parameters.

**Tests**: Template/instance pattern, uniform override, texture binding, batch compatibility.

**Expected symbols**: `MaterialTemplate`, `MaterialInstance`, uniform override map, texture slot assignment

**Difficulty**: VERY HIGH — combines resource management, rendering, memory efficiency

**Novelty**: VERY HIGH — HSE-007 never discussed material instances

---

### E5: Multi-threaded scene update

**Mission**: Add ability to update scene logic on worker threads while rendering on main thread.

**Tests**: Thread safety, double buffering, command queue, synchronization.

**Expected symbols**: `SceneUpdater`, double-buffered scene, `std::atomic` flags, command queue, frame sync

**Difficulty**: VERY HIGH — requires deep threading knowledge, GL context ownership

**Novelty**: VERY HIGH — HSE-007 never discussed multi-threaded updates

---

## Mission Summary

| Tier | Missions | Difficulty | Novelty | Tests |
|------|----------|------------|---------|-------|
| A: Simple | 5 | LOW | LOW | Basic pattern following |
| B: Intermediate | 5 | MEDIUM | MEDIUM | Rendering + architecture |
| C: Complex | 5 | HIGH | HIGH | Deep system design |
| D: Cross-domain | 5 | VERY HIGH | HIGH | Multi-domain integration |
| E: Novel/Unseen | 5 | VERY HIGH | VERY HIGH | Concepts not in HSE-007 |
| **Total** | **25** | | | |

## Benchmark Usage

1. **Baseline (L0)**: Run all 25 missions with repository-only knowledge
2. **Each layer (L1-L10)**: Re-run all 25 missions with cumulative knowledge
3. **Measure**: Score changes, architecture changes, symbol accuracy changes
4. **Analyze**: Diminishing returns across layers and tiers
