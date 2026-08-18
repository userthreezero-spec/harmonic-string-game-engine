# HSE-007 Knowledge Map — Domains WebOS Needs

## Domain A: Modern C++ (Priority: HIGH)

### What HSE Actually Uses
- C++20 (CMakeLists.txt:4)
- `std::shared_ptr` (scene ownership, bridge commands)
- `std::unique_ptr` (renderer pimpl)
- `std::atomic` (bridge thread sync)
- `std::thread` (bridge reader)
- `std::vector`, `std::array`, `std::deque`, `std::unordered_map`
- `std::string`, `std::ostringstream`
- Move semantics (deleted on Window, Primitive)
- RAII (Window destroys GLFW, Primitive deletes GL resources)

### What's Missing for Deeper Understanding
- Template patterns (none used beyond STL)
- Concepts (C++20 feature, not used)
- Smart pointer ownership patterns (shared_ptr everywhere, could be unique_ptr in many cases)
- Lock-free programming theory (SPSC queue exists but pattern not generalized)

### Knowledge Value: **MEDIUM** — HSE uses basic C++ well. Deeper C++ knowledge helps with design decisions but doesn't change HSE architecture.

---

## Domain B: C++ Game-Engine Architecture (Priority: HIGH)

### What HSE Actually Has
- Frame lifecycle: poll → update → beginFrame → render → endFrame → swap
- Flat scene graph (vector of primitives, no hierarchy)
- Single renderer (forward rendering, single shader)
- Camera system (perspective/ortho, view + projection matrices)
- Primitive geometry (triangle, quad, cube, sphere stub)
- Bridge pattern (external command → queue → GL thread execution)

### What HSE Lacks (and needs to understand)
- Entity/Component systems
- Scene hierarchy (parent/child transforms)
- Resource management (no asset pipeline, no reference counting beyond shared_ptr)
- Event system (no observer pattern, no event bus)
- Input handling (only GLFW default ESC)
- Profiling/debugging infrastructure
- Multi-pass rendering
- Lighting model

### Knowledge Value: **HIGH** — Understanding engine architecture patterns directly improves HSE design decisions.

---

## Domain C: Rendering (Priority: CRITICAL)

### What HSE Actually Uses
- OpenGL 3.3 core profile
- Single vertex shader (model/view/projection transform)
- Single fragment shader (uniform color, no textures)
- Forward rendering (single pass)
- VAO/VBO/EBO per primitive
- glReadPixels for frame capture
- Depth testing enabled
- No lighting, no textures, no shadows

### What the Knowledge Must Cover
- Shader programming (vertex, fragment, geometry shaders)
- Texture mapping (UV coordinates, texture units)
- Lighting models (Phong, PBR)
- Transform hierarchy (model → world → view → projection)
- Framebuffer objects (off-screen rendering)
- Instancing (multiple objects with same mesh)
- Batch rendering (reduce draw calls)
- Spatial partitioning (octree, BSP, frustum culling)
- Post-processing (bloom, SSAO, tone mapping)

### Knowledge Value: **CRITICAL** — Rendering is HSE's primary purpose. Understanding rendering theory directly enables HSE's next features.

---

## Domain D: 3D Mathematics (Priority: HIGH)

### What HSE Actually Has
- Vec3: add, subtract, scalar multiply, length, normalize, dot, cross
- Mat4: identity, translate, rotate, scale, perspective, ortho, lookAt, multiply
- Euler angle rotation (applied as X * Y * Z)
- No quaternions, no interpolation, no spline math

### What's Missing
- Quaternions (avoid gimbal lock)
- Interpolation (lerp, slerp, smoothstep)
- Bezier/NURBS curves
- Ray casting / picking
- Bounding volumes (AABB, sphere, OBB)
- Plane equations
- Frustum math
- Numerical stability (epsilon comparisons, conditioning)

### Knowledge Value: **HIGH** — Math depth enables animation, physics, procedural geometry, and harmonic model.

---

## Domain E: UI/Editor Architecture (Priority: LOW for now)

### Current State: No UI
HSE has no editor, no UI, no property inspector. All control is via bridge protocol.

### Future Need
- Scene hierarchy panel
- Property inspector
- Viewport with gizmos
- Project browser
- Undo/redo system

### Knowledge Value: **LOW now, HIGH later** — Not blocking current HSE development.

---

## Domain F: Software Architecture (Priority: MEDIUM)

### What HSE Has
- Layered: Math → Core → Scene → Renderer → Bridge → Entry Points
- Module separation via static libraries (hse_core, hse_bridge)
- Command pattern (bridge commands)
- Observer pattern (missing — no event bus)
- Factory pattern (SceneBuilder creates scenes)
- Pimpl pattern (Renderer::RendererState)

### What Would Improve
- Dependency inversion (Bridge depends directly on Scene, not via interface)
- Event system (currently everything is synchronous)
- Plugin architecture (currently hardcoded)
- State machine (no explicit state management)

### Knowledge Value: **MEDIUM** — Improves architectural reasoning but doesn't change immediate HSE needs.

---

## Domain G: Mature Open-Source Engines (Priority: MEDIUM)

### Engines to Study (source-level)
- **raylib**: Simple, clean C API — good comparison for HSE's simplicity
- **bgfx**: Cross-platform rendering abstraction — shows how to abstract GL
- **Godot**: Full engine with editor — shows scene graph, resource system
- **EnTT**: ECS library — shows entity/component patterns
- **Dear ImGui**: Immediate-mode UI — shows editor architecture
- **GLFW**: Window/input abstraction — HSE already uses this

### Knowledge Value: **MEDIUM** — Provides architectural reference points, not direct implementation guidance.

---

## Domain H: Roblox (Priority: LOW)

### What Roblox Teaches
- Procedural world construction from primitives
- Lua scripting for behavior
- Studio as real-time editor
- Instance hierarchy (parent/child)
- Material system (SurfaceAppearance, etc.)

### Knowledge Value: **LOW** — Roblox is a comparison target, not an architectural guide.

---

## Domain I: Unreal Engine (Priority: LOW)

### What Unreal Teaches
- Actor/Component model
- Level/world system
- Material editor
- Blueprint visual scripting
- Advanced rendering (deferred, Lumen, Nanite)

### Knowledge Value: **LOW** — Too complex for HSE's current scale. Architectural principles only.

---

## Ingestion Priority Summary

| Domain | Priority | Expected Value | When |
|--------|----------|---------------|------|
| C. Rendering | CRITICAL | Enables HSE's next features | Now |
| D. 3D Mathematics | HIGH | Enables animation, procedural | Now |
| B. Engine Architecture | HIGH | Improves design decisions | Now |
| A. Modern C++ | MEDIUM | Improves code quality | Now |
| F. Software Architecture | MEDIUM | Improves reasoning | Now |
| G. Open-Source Engines | MEDIUM | Reference points | Later |
| E. UI/Editor | LOW | Not blocking | Later |
| H. Roblox | LOW | Comparison only | Later |
| I. Unreal | LOW | Architectural principles | Later |
