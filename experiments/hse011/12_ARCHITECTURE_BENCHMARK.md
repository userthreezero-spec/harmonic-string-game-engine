# HSE-011 FILE 12: Architecture Benchmark

**Date**: 2026-08-18
**Experiment**: HSE-011 Knowledge Saturation Experiment
**Status**: DEFINED — NOT YET EXECUTED

---

## Purpose

Define 50+ benchmark missions that test whether accumulated knowledge translates to correct architectural output. Each mission specifies exactly what the AI should produce, grounded in HSE's actual repository structure. Every claim is evaluated against what the HSE-009 architecture verifier would say.

## Scoring Method

Each mission is scored on 5 dimensions (0-2 each, max 10):
- **Symbol Accuracy**: Correct file paths, class names, method names, field names
- **Architecture Quality**: Correct subsystem/module placement, proper dependencies
- **Implementation Readiness**: Code could be written from the specification
- **Acceptance Criteria**: Testable, measurable pass/fail criteria
- **Verification Plan**: How to confirm the implementation works

## Ground Truth Baseline

From ground_truth.json:
- 27 source files, 10 classes, 8 structs, 3 enums, 125 functions
- Namespace: `hse`
- Key classes: Bridge, CommandQueue, PipeServer, Window, Mat4, Renderer, Camera, Primitive, Scene, SceneBuilder

---

## Tier A: Rendering Pipeline (Missions 1-10)

### Mission 1: Triangle Rendering Pipeline

| Field | Value |
|-------|-------|
| Feature | Render a single triangle to screen |
| Expected Subsystem | Rendering |
| Expected Module | `src/renderer/renderer.cpp` |
| Expected Class | `Renderer` |
| Expected Method | `Renderer::renderScene()` |
| Expected Data Structure | `m_vertices: std::vector<float>` on `Primitive` |
| Dependencies | OpenGL 3.3, GLEW, GLFW window |
| Implementation Order | 1 (foundational) |
| Acceptance Test | Triangle appears in window with correct color |
| Verification Method | Visual inspection + pixel readback via `Renderer::readPixels()` |

### Mission 2: Indexed Rendering

| Field | Value |
|-------|-------|
| Feature | Render geometry using index buffers |
| Expected Subsystem | Rendering |
| Expected Module | `src/scene/primitive.cpp` |
| Expected Class | `Primitive` |
| Expected Method | `Primitive::uploadGPU()` |
| Expected Data Structure | `m_ebo: unsigned int`, `m_indices: std::vector<uint32_t>` |
| Dependencies | VAO, VBO from Mission 1 |
| Implementation Order | 2 |
| Acceptance Test | Quad renders with 4 vertices + 6 indices (not 6 vertices) |
| Verification Method | GPU memory inspection, draw call count |

### Mission 3: Shader Compilation and Linking

| Field | Value |
|-------|-------|
| Feature | Compile vertex/fragment shaders, link into program |
| Expected Subsystem | Rendering |
| Expected Module | `src/renderer/renderer.cpp` |
| Expected Class | `Renderer` |
| Expected Method | `Renderer::initialize()` |
| Expected Data Structure | `m_state->m_shaderProgram` (via RendererState pimpl) |
| Dependencies | None (self-contained) |
| Implementation Order | 3 |
| Acceptance Test | Shader compiles without error, program links, uniform locations queryable |
| Verification Method | `glGetShaderiv(GL_COMPILE_STATUS)`, `glGetProgramiv(GL_LINK_STATUS)` |

### Mission 4: Vertex Attribute Configuration

| Field | Value |
|-------|-------|
| Feature | Configure vertex attributes for position + color |
| Expected Subsystem | Rendering |
| Expected Module | `src/scene/primitive.cpp` |
| Expected Class | `Primitive` |
| Expected Method | `Primitive::uploadGPU()` |
| Expected Data Structure | Stride calculation: 6 floats (3 pos + 3 color) * sizeof(float) |
| Dependencies | VAO, VBO from Mission 2 |
| Implementation Order | 4 |
| Acceptance Test | `glVertexAttribPointer` called with correct stride/offset for position (0) and color (12 bytes) |
| Verification Method | OpenGL state query |

### Mission 5: Depth Testing Configuration

| Field | Value |
|-------|-------|
| Feature | Enable depth testing to prevent z-fighting |
| Expected Subsystem | Rendering |
| Expected Module | `src/renderer/renderer.cpp` |
| Expected Class | `Renderer` |
| Expected Method | `Renderer::initialize()` or `Renderer::beginFrame()` |
| Expected Data Structure | N/A (OpenGL state) |
| Dependencies | None |
| Implementation Order | 5 |
| Acceptance Test | `glIsEnabled(GL_DEPTH_TEST)` returns GL_TRUE |
| Verification Method | OpenGL state query |

### Mission 6: Alpha Blending

| Field | Value |
|-------|-------|
| Feature | Enable alpha blending for transparent objects |
| Expected Subsystem | Rendering |
| Expected Module | `src/renderer/renderer.cpp` |
| Expected Class | `Renderer` |
| Expected Method | `Renderer::initialize()` |
| Expected Data Structure | N/A (OpenGL state) |
| Dependencies | Mission 5 (depth testing) |
| Implementation Order | 6 |
| Acceptance Test | `glIsEnabled(GL_BLEND)` returns GL_TRUE, `glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)` |
| Verification Method | OpenGL state query |

### Mission 7: Camera Projection Matrix

| Field | Value |
|-------|-------|
| Feature | Generate perspective projection matrix |
| Expected Subsystem | Scene/Camera |
| Expected Module | `src/scene/camera.cpp` |
| Expected Class | `Camera` |
| Expected Method | `Camera::update()` |
| Expected Data Structure | `m_projectionMatrix: Mat4`, `m_fovDeg: float`, `m_nearPlane: float`, `m_farPlane: float` |
| Dependencies | Mat4::perspective() |
| Implementation Order | 7 |
| Acceptance Test | Projection matrix produces correct NDC coordinates for known input |
| Verification Method | Unit test: compare matrix elements against hand-calculated values |

### Mission 8: Camera View Matrix

| Field | Value |
|-------|-------|
| Feature | Generate view matrix from position/target/up |
| Expected Subsystem | Scene/Camera |
| Expected Module | `src/scene/camera.cpp` |
| Expected Class | `Camera` |
| Expected Method | `Camera::update()` |
| Expected Data Structure | `m_viewMatrix: Mat4`, `m_position: Vec3`, `m_target: Vec3`, `m_up: Vec3` |
| Dependencies | Mat4::lookAt() |
| Implementation Order | 8 |
| Acceptance Test | View matrix transforms world coordinates to view space correctly |
| Verification Method | Unit test: known eye/target/up produces known view matrix |

### Mission 9: Render Loop Frame Timing

| Field | Value |
|-------|-------|
| Feature | Calculate delta time between frames |
| Expected Subsystem | Core |
| Expected Module | `src/core/window.cpp` |
| Expected Class | `Window` |
| Expected Method | `Window::getDeltaTime()` |
| Expected Data Structure | `m_lastFrameTime: double` |
| Dependencies | GLFW glfwGetTime() |
| Implementation Order | 9 |
| Acceptance Test | Delta time is positive and approximately correct (within 1ms) |
| Verification Method | Timing comparison against system clock |

### Mission 10: Viewport Resize Handling

| Field | Value |
|-------|-------|
| Feature | Handle window resize and update viewport |
| Expected Subsystem | Core/Rendering |
| Expected Module | `src/core/window.cpp` |
| Expected Class | `Window` |
| Expected Method | `Window::setResizeCallback()` |
| Expected Data Structure | `m_width: int`, `m_height: int` |
| Dependencies | GLFW framebuffer callback |
| Implementation Order | 10 |
| Acceptance Test | On resize, viewport dimensions match window dimensions |
| Verification Method | Resize window, query `glGetIntegerv(GL_VIEWPORT)` |

---

## Tier B: Input System (Missions 11-16)

### Mission 11: Keyboard Input Polling

| Field | Value |
|-------|-------|
| Feature | Poll keyboard state each frame |
| Expected Subsystem | Input |
| Expected Module | `src/core/window.cpp` |
| Expected Class | `Window` |
| Expected Method | `Window::pollEvents()` |
| Expected Data Structure | N/A (GLFW callback) |
| Dependencies | GLFW |
| Implementation Order | 11 |
| Acceptance Test | Key press detected within same frame |
| Verification Method | Press key, check flag state |

### Mission 12: Mouse Position Tracking

| Field | Value |
|-------|-------|
| Feature | Track mouse position relative to window |
| Expected Subsystem | Input |
| Expected Module | `src/core/window.cpp` |
| Expected Class | `Window` |
| Expected Method | `Window::pollEvents()` |
| Expected Data Structure | N/A (GLFW callback) |
| Dependencies | GLFW |
| Implementation Order | 12 |
| Acceptance Test | Mouse position updates when mouse moves |
| Verification Method | Move mouse, query position |

### Mission 13: Gamepad Axis Reading

| Field | Value |
|-------|-------|
| Feature | Read gamepad joystick axes |
| Expected Subsystem | Input |
| Expected Module | `src/core/window.cpp` |
| Expected Class | `Window` |
| Expected Method | New method `Window::getGamepadAxis()` |
| Expected Data Structure | GLFWgamepadstate |
| Dependencies | GLFW 3.3+ gamepad API |
| Implementation Order | 13 |
| Acceptance Test | Axis values in range [-1.0, 1.0] for connected controller |
| Verification Method | Physical controller input |

### Mission 14: Input-to-Camera Mapping

| Field | Value |
|-------|-------|
| Feature | Map WASD keys to camera movement |
| Expected Subsystem | Input + Scene |
| Expected Module | `src/main.cpp` or new input handler |
| Expected Class | `Camera` |
| Expected Method | `Camera::setPosition()` |
| Expected Data Structure | `m_position: Vec3` |
| Dependencies | Window input, Camera |
| Implementation Order | 14 |
| Acceptance Test | Pressing W moves camera forward in view direction |
| Verification Method | Visual inspection + position query |

### Mission 15: Mouse Look Camera Control

| Field | Value |
|-------|-------|
| Feature | Map mouse movement to camera rotation |
| Expected Subsystem | Input + Scene |
| Expected Module | `src/main.cpp` or new input handler |
| Expected Class | `Camera` |
| Expected Method | `Camera::lookAt()` |
| Expected Data Structure | `m_target: Vec3` |
| Dependencies | Mouse input, Camera |
| Implementation Order | 15 |
| Acceptance Test | Moving mouse horizontally rotates camera yaw |
| Verification Method | Visual inspection + target vector query |

### Mission 16: Scroll Wheel FOV Control

| Field | Value |
|-------|-------|
| Feature | Map scroll wheel to camera FOV |
| Expected Subsystem | Input + Scene |
| Expected Module | `src/main.cpp` or new input handler |
| Expected Class | `Camera` |
| Expected Method | `Camera::setFOV()` |
| Expected Data Structure | `m_fovDeg: float` |
| Dependencies | GLFW scroll callback, Camera |
| Implementation Order | 16 |
| Acceptance Test | Scroll up decreases FOV (zoom in), scroll down increases FOV (zoom out) |
| Verification Method | Scroll, query FOV value |

---

## Tier C: Scene Management (Missions 17-24)

### Mission 17: Add Primitive to Scene

| Field | Value |
|-------|-------|
| Feature | Add a new primitive object to the scene |
| Expected Subsystem | Scene |
| Expected Module | `src/scene/scene.cpp` |
| Expected Class | `Scene` |
| Expected Method | `Scene::addPrimitive()` |
| Expected Data Structure | `m_primitives: std::vector<std::shared_ptr<Primitive>>` |
| Dependencies | Primitive |
| Implementation Order | 17 |
| Acceptance Test | Primitive count increases by 1, object appears in render |
| Verification Method | `Scene::getPrimitiveCount()` |

### Mission 18: Remove Primitive by ID

| Field | Value |
|-------|-------|
| Feature | Remove a primitive by its unique ID |
| Expected Subsystem | Scene |
| Expected Module | `src/scene/scene.cpp` |
| Expected Class | `Scene` |
| Expected Method | `Scene::removePrimitiveByID()` |
| Expected Data Structure | ID index (internal) |
| Dependencies | Primitive ID system |
| Implementation Order | 18 |
| Acceptance Test | Primitive count decreases, object disappears from render |
| Verification Method | `Scene::getPrimitiveCount()` + visual |

### Mission 19: Find Primitive by Name

| Field | Value |
|-------|-------|
| Feature | Look up a primitive by its name string |
| Expected Subsystem | Scene |
| Expected Module | `src/scene/scene.cpp` |
| Expected Class | `Scene` |
| Expected Method | `Scene::findByName()` |
| Expected Data Structure | `m_name: std::string` on Primitive |
| Dependencies | Primitive naming |
| Implementation Order | 19 |
| Acceptance Test | Returns correct primitive for known name, nullptr for unknown |
| Verification Method | Unit test with known names |

### Mission 20: Primitive Transform Operations

| Field | Value |
|-------|-------|
| Feature | Set position, rotation, scale on a primitive |
| Expected Subsystem | Scene |
| Expected Module | `src/scene/primitive.cpp` |
| Expected Class | `Primitive` |
| Expected Method | `Primitive::setPosition()`, `setRotation()`, `setScale()` |
| Expected Data Structure | `m_position: Vec3`, `m_rotation: Vec3`, `m_scale: Vec3` |
| Dependencies | Vec3 |
| Implementation Order | 20 |
| Acceptance Test | Object moves/rotates/scales in scene |
| Verification Method | Query transform values + visual |

### Mission 21: Scene Update Loop

| Field | Value |
|-------|-------|
| Feature | Update all primitives each frame (e.g., rotation) |
| Expected Subsystem | Scene |
| Expected Module | `src/scene/scene.cpp` |
| Expected Class | `Scene` |
| Expected Method | `Scene::update()` |
| Expected Data Structure | `m_rotationSpeed: Vec3` on Primitive |
| Dependencies | Delta time from Window |
| Implementation Order | 21 |
| Acceptance Test | Objects with rotationSpeed actually rotate each frame |
| Verification Method | Position query across frames |

### Mission 22: Camera-to-Scene Binding

| Field | Value |
|-------|-------|
| Feature | Select active camera for rendering |
| Expected Subsystem | Scene |
| Expected Module | `src/scene/scene.cpp` |
| Expected Class | `Scene` |
| Expected Method | `Scene::setActiveCamera()`, `Scene::getActiveCamera()` |
| Expected Data Structure | `m_cameras: std::vector<std::shared_ptr<Camera>>`, `m_activeCameraIndex: int` |
| Dependencies | Camera |
| Implementation Order | 22 |
| Acceptance Test | Rendering uses the active camera's view/projection |
| Verification Method | Switch cameras, verify rendered view changes |

### Mission 23: Scene Export (HSC Format)

| Field | Value |
|-------|-------|
| Feature | Export scene state to .hsc file |
| Expected Subsystem | Serialization |
| Expected Module | `src/scene/scene_builder.cpp` |
| Expected Class | `SceneBuilder` |
| Expected Method | `SceneBuilder::exportHSC()` |
| Expected Data Structure | `ProjectManifest` struct |
| Dependencies | Scene, Camera, ProjectManifest |
| Implementation Order | 23 |
| Acceptance Test | .hsc file created, contains all primitive data and camera state |
| Verification Method | File existence + parse content |

### Mission 24: Scene Import (HSC Format)

| Field | Value |
|-------|-------|
| Feature | Import scene state from .hsc file |
| Expected Subsystem | Serialization |
| Expected Module | `src/scene/scene_builder.cpp` |
| Expected Class | `SceneBuilder` |
| Expected Method | `SceneBuilder::importState()` |
| Expected Data Structure | `ProjectManifest` struct |
| Dependencies | Export from Mission 23 |
| Implementation Order | 24 |
| Acceptance Test | Imported scene matches exported scene (round-trip) |
| Verification Method | Export -> Import -> compare primitive count, positions, colors |

---

## Tier D: Bridge/External Control (Missions 25-30)

### Mission 25: Named Pipe Server

| Field | Value |
|-------|-------|
| Feature | Create named pipe for external process communication |
| Expected Subsystem | Bridge |
| Expected Module | `src/bridge/pipe_transport.cpp` |
| Expected Class | `PipeServer` |
| Expected Method | `PipeServer::start()`, `PipeServer::waitClient()` |
| Expected Data Structure | `m_pipe: HANDLE`, `m_pipeName: std::string` |
| Dependencies | Windows API |
| Implementation Order | 25 |
| Acceptance Test | External process can connect to pipe |
| Verification Method | Connect from test client |

### Mission 26: Command Queue (Lock-Free)

| Field | Value |
|-------|-------|
| Feature | Thread-safe command queue for bridge communication |
| Expected Subsystem | Bridge |
| Expected Module | `include/bridge/command_queue.h` |
| Expected Class | `CommandQueue` |
| Expected Method | `CommandQueue::push()`, `CommandQueue::pop()` |
| Expected Data Structure | `m_read: std::atomic<size_t>`, `m_write: std::atomic<size_t>` |
| Dependencies | std::atomic |
| Implementation Order | 26 |
| Acceptance Test | Push/pop from different threads without data race |
| Verification Method | Thread sanitizer + stress test |

### Mission 27: JSON Command Parsing

| Field | Value |
|-------|-------|
| Feature | Parse JSON commands from bridge into Command struct |
| Expected Subsystem | Bridge |
| Expected Module | `src/bridge/bridge.cpp` |
| Expected Class | `Bridge` |
| Expected Method | `Bridge::parseCommand()` |
| Expected Data Structure | `Command` struct with `type`, `seq`, position/rotation/scale/color fields |
| Dependencies | JSON parsing (nlohmann/json or manual) |
| Implementation Order | 27 |
| Acceptance Test | Valid JSON produces correct Command, invalid JSON returns error |
| Verification Method | Unit test with known JSON inputs |

### Mission 28: Command Execution

| Field | Value |
|-------|-------|
| Feature | Execute parsed commands against scene/camera/renderer |
| Expected Subsystem | Bridge |
| Expected Module | `src/bridge/bridge.cpp` |
| Expected Class | `Bridge` |
| Expected Method | `Bridge::executeCommand()` |
| Expected Data Structure | `Command::Type` enum (CMD_ADD_PRIMITIVE, CMD_SET_POSITION, etc.) |
| Dependencies | Scene, Camera, Renderer, Command parsing |
| Implementation Order | 28 |
| Acceptance Test | CMD_SET_POSITION changes primitive position, CMD_SET_COLOR changes color |
| Verification Method | Execute command, query object state |

### Mission 29: Scene Observation (JSON)

| Field | Value |
|-------|-------|
| Feature | Generate JSON observation of current scene state |
| Expected Subsystem | Bridge |
| Expected Module | `src/bridge/bridge.cpp` |
| Expected Class | `Bridge` |
| Expected Method | `Bridge::getObservation()`, `Bridge::getObjectObservation()` |
| Expected Data Structure | JSON string output |
| Dependencies | Scene, Camera, Renderer |
| Implementation Order | 29 |
| Acceptance Test | Observation JSON contains all primitive positions, colors, names |
| Verification Method | Parse observation JSON, compare to scene state |

### Mission 30: Snapshot Diffing

| Field | Value |
|-------|-------|
| Feature | Track changes between scene snapshots |
| Expected Subsystem | Bridge |
| Expected Module | `src/bridge/bridge.cpp` |
| Expected Class | `Bridge` |
| Expected Method | `Bridge::recordSnapshot()`, `Bridge::diffSnapshots()` |
| Expected Data Structure | `SceneSnapshot`, `ObjSnapshot`, `PropertyChange` structs |
| Dependencies | Scene, Camera |
| Implementation Order | 30 |
| Acceptance Test | Moving a primitive produces a PropertyChange with old/new position |
| Verification Method | Record snapshot, modify scene, record again, diff |

---

## Tier E: New Subsystem Architecture (Missions 31-40)

These missions test whether accumulated knowledge enables correct architecture for subsystems HSE does not yet have.

### Mission 31: Texture Loading Pipeline

| Field | Value |
|-------|-------|
| Feature | Load image file to GPU texture |
| Expected Subsystem | Resources |
| Expected Module | New: `src/resource/texture_manager.cpp` |
| Expected Class | New: `TextureManager` |
| Expected Method | `TextureManager::loadTexture()` |
| Expected Data Structure | Texture ID (GLuint), image dimensions, pixel data |
| Dependencies | stb_image, OpenGL texture API |
| Implementation Order | 31 |
| Acceptance Test | Image file loads, texture ID valid, texture binds correctly |
| Verification Method | `glIsTexture()` + render textured quad |

### Mission 32: Texture Mapping on Primitive

| Field | Value |
|-------|-------|
| Feature | Apply texture to a primitive using UV coordinates |
| Expected Subsystem | Rendering + Resources |
| Expected Module | `src/scene/primitive.cpp` |
| Expected Class | `Primitive` |
| Expected Method | `Primitive::uploadGPU()` (extended) |
| Expected Data Structure | `m_texCoords: std::vector<float>` (new), `m_textureID: GLuint` (new) |
| Dependencies | TextureManager, shader with sampler2D |
| Implementation Order | 32 |
| Acceptance Test | Primitive renders with texture applied |
| Verification Method | Visual inspection |

### Mission 33: Cubemap Skybox

| Field | Value |
|-------|-------|
| Feature | Render a cubemap skybox behind the scene |
| Expected Subsystem | Rendering |
| Expected Module | New: `src/renderer/skybox.cpp` |
| Expected Class | New: `Skybox` |
| Expected Method | `Skybox::load()`, `Skybox::render()` |
| Expected Data Structure | Cubemap texture (6 faces), skybox VAO/VBO |
| Dependencies | Cubemap textures, depth testing (depth func = LEQUAL) |
| Implementation Order | 33 |
| Acceptance Test | Skybox visible behind all geometry, depth test passes |
| Verification Method | Visual + depth test verification |

### Mission 34: Audio Device Initialization

| Field | Value |
|-------|-------|
| Feature | Initialize audio playback device |
| Expected Subsystem | Audio |
| Expected Module | New: `src/audio/audio_engine.cpp` |
| Expected Class | New: `AudioEngine` |
| Expected Method | `AudioEngine::initialize()` |
| Expected Data Structure | `ma_device`, `ma_device_config` |
| Dependencies | miniaudio |
| Implementation Order | 34 |
| Acceptance Test | Audio device opens without error, sample rate detected |
| Verification Method | Play test tone, verify output |

### Mission 35: Sound Playback

| Field | Value |
|-------|-------|
| Feature | Play a WAV file through the audio engine |
| Expected Subsystem | Audio |
| Expected Module | `src/audio/audio_engine.cpp` |
| Expected Class | `AudioEngine` |
| Expected Method | `AudioEngine::playSound()` |
| Expected Data Structure | `ma_sound`, `ma_engine` |
| Dependencies | AudioEngine init, miniaudio high-level API |
| Implementation Order | 35 |
| Acceptance Test | Audio plays, stops on command |
| Verification Method | Audible output + callback verification |

### Mission 36: Basic 2D Physics

| Field | Value |
|-------|-------|
| Feature | Simulate rigid body physics for 2D objects |
| Expected Subsystem | Physics |
| Expected Module | New: `src/physics/physics_world.cpp` |
| Expected Class | New: `PhysicsWorld` |
| Expected Method | `PhysicsWorld::step()` |
| Expected Data Structure | Body list, AABB broadphase |
| Dependencies | Box2D or custom |
| Implementation Order | 36 |
| Acceptance Test | Object falls under gravity, collides with ground plane |
| Verification Method | Position query across frames |

### Mission 37: Collision Detection

| Field | Value |
|-------|-------|
| Feature | Detect AABB overlap between objects |
| Expected Subsystem | Physics |
| Expected Module | `src/physics/physics_world.cpp` |
| Expected Class | `PhysicsWorld` |
| Expected Method | `PhysicsWorld::checkCollision()` |
| Expected Data Structure | AABB struct (min, max corners) |
| Dependencies | PhysicsWorld, Primitive transforms |
| Implementation Order | 37 |
| Acceptance Test | Overlapping objects detected, non-overlapping not detected |
| Verification Method | Unit test with known positions |

### Mission 38: Immediate Mode GUI

| Field | Value |
|-------|-------|
| Feature | Render debug UI overlay using Dear ImGui |
| Expected Subsystem | UI |
| Expected Module | New: `src/ui/debug_ui.cpp` |
| Expected Class | New: `DebugUI` |
| Expected Method | `DebugUI::initialize()`, `DebugUI::render()` |
| Expected Data Structure | ImGui draw commands → vertex buffer → GPU |
| Dependencies | Dear ImGui, OpenGL backend |
| Implementation Order | 38 |
| Acceptance Test | GUI window appears, responds to input, renders over scene |
| Verification Method | Visual inspection + input test |

### Mission 39: Frame Profiling

| Field | Value |
|-------|-------|
| Feature | Instrument frame timing with Tracy profiler |
| Expected Subsystem | Profiling |
| Expected Module | Cross-cutting (Tracy macros in renderer, scene, bridge) |
| Expected Class | N/A (macro-based instrumentation) |
| Expected Method | `FrameMark`, `ZoneScoped` macros |
| Expected Data Structure | Tracy profiling data (frame timeline, zone hierarchy) |
| Dependencies | Tracy Profiler library |
| Implementation Order | 39 |
| Acceptance Test | Tracy shows frame timeline with labeled zones |
| Verification Method | Tracy GUI connection |

### Mission 40: Entity Component System

| Field | Value |
|-------|-------|
| Feature | Basic ECS: create entity, add/remove components, query |
| Expected Subsystem | ECS |
| Expected Module | New: `src/ecs/ecs_registry.cpp` |
| Expected Class | New: `ECSRegistry` (wrapper around EnTT) |
| Expected Method | `ECSRegistry::createEntity()`, `addComponent<T>()`, `getComponent<T>()` |
| Expected Data Structure | EnTT registry, sparse set storage |
| Dependencies | EnTT |
| Implementation Order | 40 |
| Acceptance Test | Entity created, component added, query returns entity |
| Verification Method | Unit test |

---

## Tier F: Cross-Domain Integration (Missions 41-50)

### Mission 41: Render Textured Scene with Camera Control

| Field | Value |
|-------|-------|
| Feature | Full rendering pipeline: camera + textured primitives + depth |
| Expected Subsystem | Rendering + Scene + Input |
| Expected Module | Multiple |
| Expected Class | Renderer, Camera, Primitive, Window |
| Expected Method | Full frame: pollEvents → update camera → renderScene → swapBuffers |
| Expected Data Structure | All rendering + scene structures |
| Dependencies | Missions 1-22 |
| Implementation Order | 41 |
| Acceptance Test | Interactive textured scene with camera movement |
| Verification Method | Visual + interaction |

### Mission 42: Bridge-Controlled Scene Editing

| Field | Value |
|-------|-------|
| Feature | External process adds/modifies objects via bridge |
| Expected Subsystem | Bridge + Scene + Rendering |
| Expected Module | bridge.cpp, scene.cpp, renderer.cpp |
| Expected Class | Bridge, Scene, Primitive |
| Expected Method | Bridge::pumpCommands → executeCommand → Scene::addPrimitive |
| Expected Data Structure | Command queue, scene state |
| Dependencies | Missions 17-30 |
| Implementation Order | 42 |
| Acceptance Test | External script adds object, it appears in rendered scene |
| Verification Method | Script → pipe → observe |

### Mission 43: Save/Load with Textures

| Field | Value |
|-------|-------|
| Feature | Serialize scene including texture assignments |
| Expected Subsystem | Serialization + Resources |
| Expected Module | scene_builder.cpp, texture_manager.cpp |
| Expected Class | SceneBuilder, TextureManager |
| Expected Method | exportHSC() extended for texture paths |
| Expected Data Structure | HSC file with texture references |
| Dependencies | Missions 23-24, 31-32 |
| Implementation Order | 43 |
| Acceptance Test | Save → Load → textures still applied correctly |
| Verification Method | Round-trip test |

### Mission 44: Physics-Scene Integration

| Field | Value |
|-------|-------|
| Feature | Physics objects update scene transforms |
| Expected Subsystem | Physics + Scene |
| Expected Module | physics_world.cpp, scene.cpp |
| Expected Class | PhysicsWorld, Scene, Primitive |
| Expected Method | PhysicsWorld::step() → update Primitive positions |
| Expected Data Structure | Physics bodies linked to Primitive IDs |
| Dependencies | Missions 21, 36-37 |
| Implementation Order | 44 |
| Acceptance Test | Objects fall and collide, scene reflects physics state |
| Verification Method | Visual + position query |

### Mission 45: Audio-Scene Integration

| Field | Value |
|-------|-------|
| Feature | Play sounds based on scene events |
| Expected Subsystem | Audio + Scene |
| Expected Module | audio_engine.cpp, bridge.cpp |
| Expected Class | AudioEngine, Bridge |
| Expected Method | Bridge command triggers AudioEngine::playSound() |
| Expected Data Structure | Sound library, event mapping |
| Dependencies | Missions 28, 34-35 |
| Implementation Order | 45 |
| Acceptance Test | CMD_ADD_PRIMITIVE triggers spawn sound |
| Verification Method | Audible + bridge log |

### Mission 46: Debug UI with Scene Inspection

| Field | Value |
|-------|-------|
| Feature | ImGui panel showing scene hierarchy and object properties |
| Expected Subsystem | UI + Scene |
| Expected Module | debug_ui.cpp, scene.cpp |
| Expected Class | DebugUI, Scene, Primitive |
| Expected Method | DebugUI::render() reads Scene state |
| Expected Data Structure | ImGui tree nodes, input fields |
| Dependencies | Missions 17-22, 38 |
| Implementation Order | 46 |
| Acceptance Test | UI shows all objects, editing UI fields changes scene |
| Verification Method | Visual + interaction |

### Mission 47: Profiling-Guided Optimization

| Field | Value |
|-------|-------|
| Feature | Identify bottleneck using Tracy, optimize render loop |
| Expected Subsystem | Profiling + Rendering |
| Expected Module | renderer.cpp (instrumented) |
| Expected Class | Renderer |
| Expected Method | Renderer::renderScene() (optimized) |
| Expected Data Structure | Tracy frame data |
| Dependencies | Missions 1-10, 39 |
| Implementation Order | 47 |
| Acceptance Test | Tracy shows frame time < 16ms (60fps) |
| Verification Method | Tracy statistics |

### Mission 48: Build System Integration

| Field | Value |
|-------|-------|
| Feature | CMakeLists.txt compiles all subsystems with correct dependencies |
| Expected Subsystem | Build |
| Expected Module | `CMakeLists.txt` |
| Expected Class | N/A |
| Expected Method | N/A |
| Expected Data Structure | CMake targets, include paths, link libraries |
| Dependencies | All subsystems |
| Implementation Order | 48 |
| Acceptance Test | `cmake --build .` succeeds with zero errors |
| Verification Method | Clean build |

### Mission 49: Cross-Platform Window Creation

| Field | Value |
|-------|-------|
| Feature | Create OpenGL 3.3 window using GLFW with correct hints |
| Expected Subsystem | Core |
| Expected Module | `src/core/window.cpp` |
| Expected Class | `Window` |
| Expected Method | `Window::isValid()` |
| Expected Data Structure | `m_window: GLFWwindow*` |
| Dependencies | GLFW, GLEW |
| Implementation Order | 49 |
| Acceptance Test | Window appears, OpenGL context created, GLEW initialized |
| Verification Method | `glGetString(GL_VERSION)` returns 3.3+ |

### Mission 50: End-to-End Pipeline

| Field | Value |
|-------|-------|
| Feature | Complete pipeline: init → create scene → render → bridge → save |
| Expected Subsystem | All |
| Expected Module | All |
| Expected Class | All |
| Expected Method | Full initialization and frame loop |
| Expected Data Structure | All |
| Dependencies | Missions 1-48 |
| Implementation Order | 50 |
| Acceptance Test | Engine initializes, renders scene, accepts bridge commands, saves/loads |
| Verification Method | Integration test |

---

## Tier G: Novel Architecture (Missions 51-55)

These missions test generalization — they require combining knowledge in ways not directly present in any single source.

### Mission 51: Hot-Reload Shader System

| Field | Value |
|-------|-------|
| Feature | Detect shader file changes and recompile at runtime |
| Expected Subsystem | Rendering |
| Expected Module | New: `src/renderer/shader_hot_reload.cpp` |
| Expected Class | New: `ShaderManager` |
| Expected Method | `ShaderManager::watchAndReload()` |
| Expected Data Structure | File watch handle, shader cache |
| Dependencies | Shader compilation, file system |
| Implementation Order | 51 |
| Acceptance Test | Edit shader file while running, changes appear without restart |
| Verification Method | Visual + file modification |

### Mission 52: Resource Manager with Reference Counting

| Field | Value |
|-------|-------|
| Feature | Share GPU resources (textures, shaders) with reference counting |
| Expected Subsystem | Resources |
| Expected Module | New: `src/resource/resource_manager.cpp` |
| Expected Class | New: `ResourceManager` |
| Expected Method | `ResourceManager::acquire<T>()`, `ResourceManager::release<T>()` |
| Expected Data Structure | Reference-counted resource map |
| Dependencies | Texture, Shader resources |
| Implementation Order | 52 |
| Acceptance Test | Two primitives share same texture, releasing one doesn't free GPU resource |
| Verification Method | Reference count query + GPU state |

### Mission 53: Event System (Pub/Sub)

| Field | Value |
|-------|-------|
| Feature | Decoupled event dispatch between subsystems |
| Expected Subsystem | Core |
| Expected Module | New: `src/core/event_bus.cpp` |
| Expected Class | New: `EventBus` |
| Expected Method | `EventBus::subscribe<T>()`, `EventBus::emit<T>()` |
| Expected Data Structure | Type-erased callback map |
| Dependencies | C++ templates, std::function |
| Implementation Order | 53 |
| Acceptance Test | Subscribe to event, emit event, callback fires with correct data |
| Verification Method | Unit test |

### Mission 54: Layer Stack Architecture

| Field | Value |
|-------|-------|
| Feature | Application layers with receive-on-propagate input |
| Expected Subsystem | Core |
| Expected Module | New: `src/core/layer_stack.cpp` |
| Expected Class | New: `LayerStack` |
| Expected Method | `LayerStack::pushLayer()`, `LayerStack::onEvent()` |
| Expected Data Structure | Ordered layer list |
| Dependencies | Event system |
| Implementation Order | 54 |
| Acceptance Test | Top layer receives event first, can consume or pass through |
| Verification Method | Unit test with mock layers |

### Mission 55: Frame Graph (Render Passes)

| Field | Value |
|-------|-------|
| Feature | Ordered render passes with explicit input/output |
| Expected Subsystem | Rendering |
| Expected Module | New: `src/renderer/frame_graph.cpp` |
| Expected Class | New: `FrameGraph` |
| Expected Method | `FrameGraph::addPass()`, `FrameGraph::execute()` |
| Expected Data Structure | DAG of render passes, transient resource pool |
| Dependencies | FBO, texture resources |
| Implementation Order | 55 |
| Acceptance Test | Geometry pass → lighting pass → post-process pass renders correctly |
| Verification Method | Visual + FBO attachment verification |

---

## Summary Statistics

| Tier | Count | Domains Covered |
|------|-------|-----------------|
| A: Rendering Pipeline | 10 | rendering, core |
| B: Input System | 6 | input, core, scene |
| C: Scene Management | 8 | scene, serialization |
| D: Bridge/External Control | 6 | bridge |
| E: New Subsystems | 10 | resources, audio, physics, UI, profiling, ECS |
| F: Cross-Domain Integration | 10 | rendering+scene+input+bridge+audio+physics+UI+profiling+build |
| G: Novel Architecture | 5 | rendering, resources, core |
| **TOTAL** | **55** | **12+ domains** |

---

## Verification Against HSE-009 Verifier

Every mission's symbol claims should be verifiable by running the HSE-009 ground truth scanner:

- **Existing symbols** (Missions 1-30, 41-50): Must match ground_truth.json exactly
- **New symbols** (Missions 31-40, 51-55): Must be correctly identified as POSSIBLE (not yet in codebase)
- **Modified symbols**: Must correctly identify which existing symbols change

The benchmark cannot be scored without running these claims through the verifier. This is a critical dependency.
