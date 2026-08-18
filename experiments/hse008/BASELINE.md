# HSE-008 BASELINE — Repository Ground Truth

## Verified Source Statistics

| Metric | Count | Evidence |
|--------|-------|----------|
| Header files | 11 | Verified via glob + read |
| Source files | 14 | Verified via glob + read |
| Test files | 2 | tests/test_math.cpp, tests/test_scene.cpp |
| Doc files | 3 | HSE_EXTERNAL_PROTOCOL.md, experiments/hse007/* |
| Namespace | 1 (hse) | Every file in `namespace hse` |
| Build targets | 6 executables, 2 static libs | CMakeLists.txt:25-48 |

## Verified Build Targets

| Target | Type | Source | Dependencies |
|--------|------|--------|-------------|
| hse_core | STATIC LIB | window, renderer, scene, camera, primitive, scene_builder, vec3, mat4 | OpenGL, GLEW, glfw |
| hse_bridge | STATIC LIB | pipe_transport, bridge | hse_core |
| hse_main | EXE | main.cpp | hse_core |
| hse_room_demo | EXE | room_demo.cpp | hse_core |
| hse_test_math | EXE | test_math.cpp | hse_core |
| hse_test_scene | EXE | test_scene.cpp | hse_core |
| hse_bridge_app | EXE | bridge_main.cpp | hse_bridge |
| hse_workspace_app | EXE | workspace_main.cpp | hse_bridge |

## Verified Architecture

```
Math Layer: Vec3 (21 lines), Mat4 (29 lines header, 105 lines impl)
Core Layer: Window (48 lines header, 80 lines impl) — GLFW wrapper
Renderer Layer: Renderer (37 lines header, 184 lines impl) — pimpl pattern
Scene Layer:
  Primitive (76 lines header, 99 lines impl)
  Camera (46 lines header, 52 lines impl)
  Scene (49 lines header, 88 lines impl)
  SceneBuilder (39 lines header, 430 lines impl)
Bridge Layer:
  CommandQueue (85 lines header, inline impl)
  PipeServer (33 lines header, 126 lines impl)
  Bridge (93 lines header, 638 lines impl)
```

## Verified Thread Model

```
MAIN (GL) THREAD:
  - GLFW event loop (pollEvents, swapBuffers)
  - All GL calls (Renderer, Primitive::uploadGPU, Primitive::draw)
  - Bridge::pumpCommands() → dequeues from SPSC
  - Bridge::executeCommand() → Scene/Camera mutations
  - All file I/O (import/export)
  - PipeServer::writeLine() for acks/responses

READER THREAD (std::thread):
  - PipeServer::waitClient() (blocking)
  - PipeServer::readLine() (blocking)
  - Bridge::parseCommand() (JSON parse)
  - CommandQueue::push() (lock-free)
  - PipeServer::writeLine() for error acks ⚠️ CONCURRENT WRITE
```

## Verified Symbol Inventory

### Enums (3)
| Enum | Values | File |
|------|--------|------|
| PrimitiveType | Triangle, Quad, Cube, Sphere | primitive.h:8 |
| ProjectionType | Perspective, Orthographic | camera.h:10 |
| Command::Type | CMD_NONE(0)..CMD_HEALTH(17) — 18 values | command_queue.h:12-29 |

### Classes/Structs (16)
| Class | Fields | Methods | File |
|-------|--------|---------|------|
| Vec3 | x, y, z | +, -, *, +=, length, normalized, dot, cross | vec3.h |
| Mat4 | data[16] | identity, perspective, ortho, lookAt, translate, rotate, scale, *(Mat4), *(Vec3), ptr, toRadians | mat4.h |
| WindowProps | title, width, height, vsync | (struct) | window.h |
| Window | m_window, ... | isValid, shouldClose, pollEvents, swapBuffers, setResizeCallback, getDeltaTime, getHandle, getWidth, getHeight, setVSync, getKey | window.h |
| Primitive | 15 fields | 17 methods | primitive.h |
| Camera | 10 fields | 16 methods | camera.h |
| Scene | 5 fields | 14 methods | scene.h |
| WorldObject | 12 fields | (struct) | scene_builder.h |
| ProjectManifest | 9 fields | (struct) | scene_builder.h |
| SceneBuilder | — | 5 static methods | scene_builder.h |
| Renderer | m_state, m_frameCount | 11 methods | renderer.h |
| Command | 14 fields | (struct) | command_queue.h |
| CommandQueue | m_ring, m_read, m_write | push, pop, empty, clear | command_queue.h |
| PipeServer | m_pipeName, m_running, m_clientConnected, m_pipe | 8 methods | pipe_transport.h |
| PropertyChange | 4 fields | (struct) | bridge.h |
| SceneSnapshot | 12+ fields | (struct) | bridge.h |
| Bridge | 13 fields | 18 methods | bridge.h |

## Verified HSE-007 Claims

| Claim | HSE-007 Says | Actual | Match? |
|-------|-------------|--------|--------|
| 14 Primitive fields | 14 fields | 15 fields (m_uploaded missing from list) | ⚠️ MINOR |
| Shader source location | renderer_state.cpp | renderer.cpp:13-31 | ✅ CORRECT (file name slightly off) |
| Sphere stub | Single vertex at origin | m_vertices = {0,0,0}, m_indices = {0} | ✅ CORRECT |
| uploadGPU VAO/VBO/EBO | 3 GL objects | glGenVertexArrays, glGenBuffers×2 | ✅ CORRECT |
| Vertex attrib pointer | location 0, 3 floats | glVertexAttribPointer(0, 3, GL_FLOAT, ...) | ✅ CORRECT |
| No UV coordinates | Correct | No m_texCoords field exists | ✅ CORRECT |
| No normals used | Correct | m_normals exists in HSE-007 claim but NOT in actual primitive.h | ❌ INCORRECT |
| Scene uses shared_ptr | Correct | vector<shared_ptr<Primitive>> | ✅ CORRECT |
| SPSC ring buffer 256 | Correct | RING_SIZE = 256 | ✅ CORRECT |
| 18 command types | Correct | CMD_NONE..CMD_HEALTH = 0..17 | ✅ CORRECT |
| Pipe is duplex | Correct | PIPE_ACCESS_DUPLEX | ✅ CORRECT |

**CRITICAL FINDING**: HSE-007 claimed `m_normals` exists in Primitive. It does NOT. The actual Primitive class has: m_vertices, m_indices (geometry data), m_position, m_rotation, m_scale, m_color, m_rotationSpeed (transform/state), m_type, m_name, m_id (identity), m_vao, m_vbo, m_ebo, m_uploaded (GPU state). No normals field.
