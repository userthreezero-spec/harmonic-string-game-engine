# HSE-011 Phase 3: Knowledge Frontier Map

## What We Know (37 knowledge items, 6 sources ingested)

### Rendering Pipeline (16 items — STRONG)
- Complete OpenGL 3.3 graphics pipeline: VBO → VAO → EBO → draw calls
- Shader compilation, linking, uniform setting
- Depth testing configuration (glDepthFunc, GL_LESS, z-fighting)
- Alpha blending (glBlendFunc, sort requirements, discard)
- Cubemap textures and skybox rendering
- Environment mapping (reflection/refraction)
- Framebuffer attachment patterns
- sRGB texture handling

### Input System (4 items — STRONG)
- GLFW key/mouse/gamepad input
- Callback registration and polling
- Event-driven architecture pattern

### GPU Resources (7 items — STRONG)
- Buffer creation and lifecycle
- Texture creation, parameters, mipmap generation
- Buffer orphaning for dynamic updates

### Mathematics (4 items — ADEQUATE)
- GLM types (vec2-4, mat2-4)
- Transform functions (perspective, translate, rotate, scale)
- Quaternion support (not yet used in HSE)
- Preprocessor configuration

### Resource Management (5 items — ADEQUATE)
- Godot versioned settings with dirty tracking
- Autoload singleton pattern
- Resource UID system (bidirectional cache)
- Hazel event dispatcher pattern
- Hazel layer stack architecture

---

## What We Don't Know (Critical Gaps)

### NO KNOWLEDGE INGESTED YET
1. **Scene Management** — No authoritative source on scene graphs, node hierarchies, parent-child transforms
2. **Materials System** — No PBR material model, no BRDF theory, no material properties beyond flat color
3. **Audio** — Zero audio knowledge (miniaudio source not yet ingested)
4. **Physics** — No physics engine knowledge (Box2D, Bullet not yet ingested)
5. **ECS Architecture** — No entity-component-system patterns (Flecs, EnTT not yet ingested)
6. **GUI/ImGui** — No immediate-mode GUI knowledge (Dear ImGui not yet ingested)
7. **Profiling** — No performance profiling knowledge (Tracy not yet ingested)
8. **Memory Management** — No smart pointer patterns, no allocation strategies
9. **Serialization** — No serialization library knowledge (cereal, nlohmann/json not yet ingested)
10. **Platform Abstraction** — No windowing abstraction beyond GLFW

### HSE-Specific Gaps
- `m_normals` doesn't exist (HSE-008 finding) — no normals in HSE at all
- `renderer_state.cpp` doesn't exist — no rendering state machine
- UV VBO lifecycle not analyzed
- TextureManager over-engineered vs actual usage
- No shader hot-reloading knowledge

---

## Recommended Next Sources (Priority Order)
1. **Dear ImGui** (SRC-012/019) — immediate mode GUI paradigm, vertex buffer management
2. **EnTT** (SRC-027) — ECS architecture, component pools, runtime type erasure
3. **miniaudio** (SRC-022) — audio initialization, mixing, spatial audio
4. **Tracy** (SRC-023) — profiling integration, frame markers, GPU profiling
5. **nlohmann/json** (SRC-025) — JSON serialization, type marshaling
6. **Godot renderer** (deeper) — scene tree, node lifecycle, composition over inheritance
7. **Box2D** (SRC-028) — physics stepping, collision detection, rigid body dynamics
