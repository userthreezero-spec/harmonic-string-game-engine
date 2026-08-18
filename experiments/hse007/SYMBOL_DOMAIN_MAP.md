# HSE-007 Symbol-to-Domain Grounding Map

Every HSE symbol linked to the knowledge domain it belongs to.
What WebOS needs to know about each domain to understand that symbol.

## Layer 1: Math (`hse::math`)

### Vec3 (math/vec3.h)
| Symbol | Domain | Knowledge Required |
|--------|--------|-------------------|
| `x, y, z` | 3D Math | Cartesian coordinates |
| `operator+` | 3D Math | Vector addition (component-wise) |
| `operator-` | 3D Math | Vector subtraction |
| `operator*(float)` | 3D Math | Scalar multiplication |
| `length()` | 3D Math | Euclidean norm: sqrt(x²+y²+z²) |
| `normalize()` | 3D Math | Unit vector (length=1), used for directions |
| `dot()` | 3D Math | Scalar product, measures alignment |
| `cross()` | 3D Math | Vector product, gives perpendicular vector |

**Domain context**: Linear algebra. Every 3D position, direction, color, and scale is a Vec3. Normalize is used for directions. Dot is used for lighting (angle between surface normal and light direction). Cross is used for generating perpendicular vectors (normals from edges).

### Mat4 (math/mat4.h)
| Symbol | Domain | Knowledge Required |
|--------|--------|-------------------|
| `elements[16]` | 3D Math | Column-major 4x4 matrix (OpenGL convention) |
| `identity()` | 3D Math | 1s on diagonal — no transformation |
| `translate()` | 3D Math | Moves points by (x,y,z) offset |
| `rotate()` | 3D Math | Rotates around axis by angle (radians) |
| `scale()` | 3D Math | Multiplies each axis by factor |
| `perspective()` | Rendering | Projects 3D→2D with perspective foreshortening |
| `ortho()` | Rendering | Projects 3D→2D without foreshortening |
| `lookAt()` | Rendering | Creates view matrix from eye/target/up |
| `operator*` | 3D Math | Matrix multiplication (combines transforms) |

**Domain context**: `perspective(fov, aspect, near, far)` — fov is vertical field of view in degrees, aspect is width/height. Uses `1/tan(fov/2)` to compute the frustum. `lookAt` creates a view matrix that transforms world space → camera space.

---

## Layer 2: Core (`hse`)

### Window (core/window.h)
| Symbol | Domain | Knowledge Required |
|--------|--------|-------------------|
| `WindowProps` struct | Engine Architecture | Configuration object pattern (width, height, title) |
| `init()` | Engine Architecture | GLFW initialization + GLEW extension loading |
| `swapBuffers()` | Rendering | Double-buffering: display frame N while rendering N+1 |
| `pollEvents()` | Engine Architecture | Process OS events (keyboard, mouse, resize) |
| `shouldClose()` | Engine Architecture | GLFW window close flag |
| `getWidth/getHeight()` | Engine Architecture | Query current window dimensions |
| `getKey()` | Engine Architecture | GLFW key state query (ESC to close) |
| `getHandle()` | Engine Architecture | Raw GLFWwindow pointer for rendering |
| `setVSync()` | Rendering | Synchronize frame rate to monitor refresh |
| `m_handle` | Rendering | GLFWwindow* — the native window handle |

**Domain context**: GLFW is an industry-standard window/input library. The Window class wraps GLFW to provide a clean C++ interface. The destructor calls `glfwDestroyWindow` and `glfwTerminate` — RAII pattern.

---

## Layer 3: Renderer (`hse`)

### Renderer (renderer/renderer.h)
| Symbol | Domain | Knowledge Required |
|--------|--------|-------------------|
| `beginFrame()` | Rendering | Clears buffers, prepares for new frame |
| `render()` | Rendering | Draws a Primitive (bind VAO → glDrawElements) |
| `renderWithMatrix()` | Rendering | Draws with custom model-view-projection matrix |
| `endFrame()` | Rendering | Swaps buffers (double-buffering) |
| `setWireframe()` | Rendering | GL polygon mode (fill vs wireframe) |
| `setClearColor()` | Rendering | Background color (glClearColor) |
| `readPixels()` | Rendering | GPU→CPU framebuffer readback (glReadPixels) |
| `getClearColor()` | Rendering | Query current clear color |
| `getFrameCount()` | Rendering | Frame counter |
| `incrementFrameCount()` | Rendering | Advance frame counter |
| `m_shader` | Rendering | Single shader program (vertex + fragment) |
| `m_state` | Engine Architecture | Pimpl pattern (hides OpenGL details) |

**Domain context**: Single forward rendering pass. One shader program handles all primitives (uniform color, no textures). `renderWithMatrix` is used for custom transforms (camera view). The shader applies model→view→projection transform in vertex shader and passes color through in fragment shader.

---

## Layer 4: Scene

### Primitive (scene/primitive.h)
| Symbol | Domain | Knowledge Required |
|--------|--------|-------------------|
| `PrimitiveType` enum | Engine Architecture | Geometry type (Triangle, Quad, Cube, Sphere) |
| `m_vertices` | Rendering | Vertex positions (3 floats per vertex) |
| `m_colors` | Rendering | Per-vertex colors (3 floats per vertex) |
| `m_normals` | Rendering | Surface normals (3 floats per vertex, unused in shader) |
| `m_indices` | Rendering | Triangle indices (3 ints per face) |
| `m_position` | 3D Math | World-space position (Vec3) |
| `m_rotation` | 3D Math | Euler rotation in degrees (Vec3) |
| `m_scale` | 3D Math | Scale factors (Vec3) |
| `m_name` | Engine Architecture | Human-readable identifier |
| `m_id` | Engine Architecture | Unique uint64 identifier |
| `uploadGPU()` | Rendering | Creates VAO/VBO/EBO, uploads to GPU |
| `generateGeometry()` | Rendering | Generates vertices/indices for each PrimitiveType |
| `getTransformationMatrix()` | 3D Math | Builds Mat4 from position/rotation/scale |
| `draw()` | Rendering | Binds VAO, issues glDrawElements |

**Domain context**: Each Primitive owns its GPU resources. The transformation matrix applies scale→rotation→translation (SRT order). Generate geometry is a factory method that populates vertices based on type. Sphere is stubbed (1x1 quad).

### Camera (scene/camera.h)
| Symbol | Domain | Knowledge Required |
|--------|--------|-------------------|
| `m_position` | 3D Math | Camera position in world space |
| `m_target` | 3D Math | Point camera looks at |
| `m_up` | 3D Math | Camera's up direction (usually 0,1,0) |
| `m_fov` | Rendering | Vertical field of view in degrees |
| `m_nearPlane/m_farPlane` | Rendering | Clipping planes (near=0.1, far=100) |
| `m_aspectRatio` | Rendering | Viewport width/height |
| `m_perspective` | Rendering | true=perspective projection, false=orthographic |
| `getViewMatrix()` | Rendering | lookAt(m_position, m_target, m_up) |
| `getProjectionMatrix()` | Rendering | perspective() or ortho() based on m_perspective |
| `update()` | Engine Architecture | Empty (future: input handling) |

**Domain context**: Camera generates two matrices that the shader needs. View matrix transforms world→camera space. Projection matrix transforms camera→clip space. The shader multiplies: `projection * view * model * vertex`.

### Scene (scene/scene.h)
| Symbol | Domain | Knowledge Required |
|--------|--------|-------------------|
| `m_primitives` | Engine Architecture | Vector of shared_ptr<Primitive> (flat scene graph) |
| `m_cameras` | Engine Architecture | Vector of shared_ptr<Camera> |
| `m_name` | Engine Architecture | Scene identifier |
| `m_nextID` | Engine Architecture | Auto-incrementing ID counter |
| `m_revision` | Engine Architecture | Version counter (incremented on mutation) |
| `addPrimitive()` | Engine Architecture | Adds to vector, assigns ID |
| `addCamera()` | Engine Architecture | Adds to vector |
| `removePrimitive()` | Engine Architecture | Shared_ptr null check → erase from vector |
| `findByID()` | Engine Architecture | O(1) lookup via unordered_map index |
| `findByName()` | Engine Architecture | O(n) linear search by name |
| `removePrimitiveByID()` | Engine Architecture | ID-based removal with index rebuild |
| `importState()` | Engine Architecture | JSON → Scene (SceneBuilder) |
| `exportHSC()` | Engine Architecture | Scene → JSON (SceneBuilder) |

**Domain context**: Flat scene graph — no hierarchy. All primitives are at the same level. The ID index (unordered_map) enables O(1) lookup for bridge commands. Revision counter enables delta detection.

### SceneBuilder (scene/scene_builder.h)
| Symbol | Domain | Knowledge Required |
|--------|--------|-------------------|
| `createRoom()` | Engine Architecture | Factory method: creates floor, walls, ceiling (11 quads) |
| `createDemoScene()` | Engine Architecture | Factory method: creates sample scene |
| `importState()` | Engine Architecture | JSON string → Scene (parses scene_objects, cameras) |
| `exportHSC()` | Engine Architecture | Scene → JSON string (serializes objects, cameras, clear_color) |
| `createProject()` | Engine Architecture | Creates directory + initial .hsc file |
| `ProjectManifest` struct | Engine Architecture | Project metadata (name, version, scene count) |

**Domain context**: SceneBuilder is a factory/serializer. It creates preset scenes and handles round-trip serialization between JSON and Scene objects. The .hsc format is a JSON file with version, project metadata, scene objects, and cameras.

---

## Layer 5: Bridge

### CommandQueue (bridge/command_queue.h)
| Symbol | Domain | Knowledge Required |
|--------|--------|-------------------|
| `Command` struct | Software Architecture | Command pattern — encapsulates request |
| `Command::type` | Software Architecture | Enum: 18 command types |
| `Command::seq` | Software Architecture | Sequence number for ack correlation |
| `Command::objectID` | Engine Architecture | Target object for object-scoped commands |
| `Command::sinceRevision` | Engine Architecture | Delta detection threshold |
| `SPSCRingBuffer` | Software Architecture | Single-producer single-consumer ring buffer |
| `m_buffer` | Software Architecture | std::array<Command, 256> fixed-size |
| `m_head` | Software Architecture | Write index (producer only) |
| `m_tail` | Software Architecture | Read index (consumer only) |
| `push()` | Software Architecture | Lock-free enqueue (atomic operations) |
| `pop()` | Software Architecture | Lock-free dequeue (atomic operations) |

**Domain context**: SPSC ring buffer is a lock-free data structure. One thread writes (pipe reader), one thread reads (GL main thread). No mutex needed because there's exactly one producer and one consumer. 256 commands max — if full, oldest commands are dropped.

### PipeServer (bridge/pipe_transport.h)
| Symbol | Domain | Knowledge Required |
|--------|--------|-------------------|
| `create()` | Win32 API | CreateNamedPipeW with PIPE_ACCESS_INBOUND |
| `waitForConnection()` | Win32 API | ConnectNamedPipe — blocks until client connects |
| `readLine()` | Win32 API | ReadFile → parse NDJSON line |
| `isConnected()` | Win32 API | Query pipe connection state |
| `m_pipeHandle` | Win32 API | HANDLE to named pipe |

**Domain context**: Named pipe is a Windows IPC mechanism. `\\.\pipe\hse_bridge` is the pipe name. NDJSON (newline-delimited JSON) is the protocol format. The pipe is inbound-only (commands flow one direction), but observation responses flow back through the same pipe via a separate write mechanism.

### Bridge (bridge/bridge.h)
| Symbol | Domain | Knowledge Required |
|--------|--------|-------------------|
| `start()` | Software Architecture | Spawns reader thread, starts GL render loop |
| `stop()` | Software Architecture | Signals thread exit, joins |
| `tick()` | Game Engine Architecture | Frame tick: pump queue → execute commands → render |
| `execute()` | Software Architecture | Command dispatch: switch on type |
| `getScene()` | Engine Architecture | Access scene (const ref) |
| `getCamera()` | Engine Architecture | Access camera (const ref) |
| `getRenderer()` | Engine Architecture | Access renderer |
| `observeScene()` | Software Architecture | Full scene state as JSON |
| `observeObject()` | Software Architecture | Single object state by ID |
| `observeCamera()` | Software Architecture | Camera state as JSON |
| `observeHealth()` | Software Architecture | System health metrics |
| `observeDelta()` | Software Architecture | Changes since last query (by revision) |
| `observeFrameCapture()` | Rendering | Framebuffer → base64 RGBA |
| `getSceneDelta()` | Software Architecture | Diff between two revision numbers |
| `saveProject()` | Engine Architecture | exportHSC → file |
| `loadProject()` | Engine Architecture | importState from file |
| `m_readerThread` | Software Architecture | std::thread for pipe reading |
| `m_running` | Software Architecture | atomic<bool> thread control |

**Domain context**: Bridge is the core integration point. It owns the scene, camera, renderer, and pipe. The reader thread pushes commands to the SPSC queue. The main thread pops and executes. All GL calls happen on the main thread (OpenGL is single-threaded).

---

## Cross-Domain Symbol Dependencies

```
Rendering domain ←→ 3D Math domain
  Renderer uses Mat4 for transforms
  Camera generates view/projection matrices
  Primitive builds transformation matrix

Engine Architecture domain ←→ Rendering domain
  Bridge orchestrates frame lifecycle
  Scene manages GPU resources via Primitives
  Window provides framebuffer for rendering

Software Architecture domain ←→ Engine Architecture domain
  CommandQueue decouples pipe from GL thread
  Bridge translates commands to engine operations
  PipeServer provides external interface
```

## What This Map Reveals

1. **HSE has 5 knowledge domains** that interact: Math, Core, Renderer, Scene, Bridge
2. **Rendering is the center** — most symbols depend on rendering knowledge
3. **3D Math is foundational** — every spatial operation depends on Vec3/Mat4
4. **Engine Architecture is the glue** — connects rendering to external control
5. **Software Architecture is the enabler** — makes the system work across threads
6. **The hardest domain to learn is Rendering** — it requires understanding GPU pipelines, shaders, and OpenGL state machines
