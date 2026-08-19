# HSE-012: Complete Ground Truth Model

## Repository Overview

- **Branch**: `harmonic-string-game-engine`
- **Commit**: `53b7653`
- **Namespace**: `hse`
- **Build system**: CMake (`CMakeLists.txt`)
- **External deps**: GLFW 3, GLEW, OpenGL 3.3 core profile

---

## Files (27)

### Headers (11)

| # | Path |
|---|------|
| 1 | `include/bridge/bridge.h` |
| 2 | `include/bridge/command_queue.h` |
| 3 | `include/bridge/pipe_transport.h` |
| 4 | `include/core/window.h` |
| 5 | `include/math/mat4.h` |
| 6 | `include/math/vec3.h` |
| 7 | `include/renderer/renderer.h` |
| 8 | `include/scene/camera.h` |
| 9 | `include/scene/primitive.h` |
| 10 | `include/scene/scene.h` |
| 11 | `include/scene/scene_builder.h` |

### Sources (14)

| # | Path |
|---|------|
| 12 | `src/bridge/bridge.cpp` |
| 13 | `src/bridge/pipe_transport.cpp` |
| 14 | `src/bridge_main.cpp` |
| 15 | `src/core/window.cpp` |
| 16 | `src/main.cpp` |
| 17 | `src/math/mat4.cpp` |
| 18 | `src/math/vec3.cpp` |
| 19 | `src/renderer/renderer.cpp` |
| 20 | `src/room_demo.cpp` |
| 21 | `src/scene/camera.cpp` |
| 22 | `src/scene/primitive.cpp` |
| 23 | `src/scene/scene.cpp` |
| 24 | `src/scene/scene_builder.cpp` |
| 25 | `src/workspace_main.cpp` |

### Tests (2)

| # | Path |
|---|------|
| 26 | `tests/test_math.cpp` |
| 27 | `tests/test_scene.cpp` |

---

## Classes (12)

### 1. Bridge (`include/bridge/bridge.h:42`)

**Fields (14)**:

| Name | Type | Access |
|------|------|--------|
| `m_pipe` | `PipeServer` | private |
| `m_queue` | `CommandQueue` | private |
| `m_readerThread` | `std::thread` | private |
| `m_running` | `std::atomic<bool>` | private |
| `m_readerReady` | `std::atomic<bool>` | private |
| `m_commandsProcessed` | `int` | private |
| `m_sceneRevision` | `int` | private |
| `m_frameCount` | `uint64_t` | private |
| `m_seqCounter` | `uint64_t` | private |
| `m_snapshots` | `std::deque<SceneSnapshot>` | private |
| `MAX_SNAPSHOTS` | `constexpr size_t` | private |
| `m_lastCommandID` | `std::string` | private |
| `m_lastCommandStatus` | `std::string` | private |
| `m_manifest` | `ProjectManifest` | private |

**Methods (21)**:

| Name | Signature | Access |
|------|-----------|--------|
| `start` | `bool start()` | public |
| `stop` | `void stop()` | public |
| `pumpCommands` | `void pumpCommands(std::shared_ptr<Scene>, std::shared_ptr<Camera>, Renderer&)` | public |
| `getObservation` | `std::string getObservation(const Scene&, const Camera&, const Renderer&)` | public |
| `getObjectObservation` | `std::string getObjectObservation(uint64_t, const Scene&)` | public |
| `getCameraObservation` | `std::string getCameraObservation(const Camera&)` | public |
| `getHealthObservation` | `std::string getHealthObservation(const Scene&, const Renderer&)` | public |
| `getDeltaObservation` | `std::string getDeltaObservation(uint64_t)` | public |
| `getCaptureFrame` | `std::string getCaptureFrame(Renderer&)` | public |
| `setProjectManifest` | `void setProjectManifest(const ProjectManifest&)` | public |
| `getProjectManifest` | `const ProjectManifest& getProjectManifest() const` | public |
| `isRunning` | `bool isRunning() const` | public |
| `getCommandsProcessed` | `int getCommandsProcessed() const` | public |
| `getSceneRevision` | `int getSceneRevision() const` | public |
| `getFrameCount` | `uint64_t getFrameCount() const` | public |
| `readerThread` | `void readerThread()` | private |
| `parseCommand` | `std::string parseCommand(const std::string&, Command&)` | private |
| `executeCommand` | `void executeCommand(const Command&, std::shared_ptr<Scene>, std::shared_ptr<Camera>, Renderer&)` | private |
| `makeAck` | `std::string makeAck(uint64_t, bool, const std::string& = "")` | private |
| `recordSnapshot` | `void recordSnapshot(const Scene&, const Camera&)` | private |
| `diffSnapshots` | `std::vector<PropertyChange> diffSnapshots(const SceneSnapshot&, const SceneSnapshot&) const` | private |

**Source**: `src/bridge/bridge.cpp`

### 2. CommandQueue (`include/bridge/command_queue.h:47`)

**Fields (2 real)**:

| Name | Type | Access |
|------|------|--------|
| `m_read` | `std::atomic<size_t>` | private |
| `m_write` | `std::atomic<size_t>` | private |

Note: Ground truth scanner misidentifies `return true`/`return false` in inline methods as fields. Actual data members are `m_read` and `m_write`, plus a `std::array<Command, 256>` ring buffer.

**Methods (4)**:

| Name | Signature | Access |
|------|-----------|--------|
| `push` | `bool push(const Command&)` | public |
| `pop` | `bool pop(Command&)` | public |
| `empty` | `bool empty() const` | public |
| `clear` | `void clear()` | public |

### 3. PipeServer (`include/bridge/pipe_transport.h:11`)

**Fields (4)**:

| Name | Type | Access |
|------|------|--------|
| `m_pipeName` | `std::string` | private |
| `m_running` | `bool` | private |
| `m_clientConnected` | `bool` | private |
| `m_pipe` | `HANDLE` | private |

**Methods (7)**:

| Name | Signature | Access |
|------|-----------|--------|
| `start` | `bool start()` | public |
| `stop` | `void stop()` | public |
| `waitClient` | `bool waitClient(int timeoutMs = 5000)` | public |
| `readLine` | `bool readLine(std::string&)` | public |
| `writeLine` | `bool writeLine(const std::string&)` | public |
| `isClientConnected` | `bool isClientConnected() const` | public |
| `disconnectClient` | `void disconnectClient()` | public |

**Source**: `src/bridge/pipe_transport.cpp` (two platform implementations: Win32 + stub)

### 4. Window (`include/core/window.h:16`)

**Fields (4)**:

| Name | Type | Access |
|------|------|--------|
| `m_window` | `GLFWwindow*` | private |
| `m_width` | `int` | private |
| `m_height` | `int` | private |
| `m_lastFrameTime` | `double` | private |

**Methods (10)**:

| Name | Signature | Access |
|------|-----------|--------|
| `isValid` | `bool isValid() const` | public |
| `shouldClose` | `bool shouldClose() const` | public |
| `pollEvents` | `void pollEvents()` | public |
| `swapBuffers` | `void swapBuffers()` | public |
| `getDeltaTime` | `float getDeltaTime()` | public |
| `getWidth` | `int getWidth() const` | public |
| `getHeight` | `int getHeight() const` | public |
| `getNative` | `GLFWwindow* getNative() const` | public |
| `setResizeCallback` | `void setResizeCallback(std::function<void(int,int)>)` | public |
| `framebufferSizeCallback` | `friend void framebufferSizeCallback(...)` | private |

**Source**: `src/core/window.cpp`

### 5. Mat4 (`include/math/mat4.h:6`)

**Fields (0)**: Data stored as `float data[16]` (POD array, not detected as named member)

**Methods (9)**:

| Name | Signature | Access |
|------|-----------|--------|
| `identity` | `static Mat4 identity()` | public |
| `perspective` | `static Mat4 perspective(float fovDeg, float aspect, float near, float far)` | public |
| `ortho` | `static Mat4 ortho(float left, float right, float bottom, float top, float near, float far)` | public |
| `lookAt` | `static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up)` | public |
| `translate` | `static Mat4 translate(const Vec3& v)` | public |
| `rotate` | `static Mat4 rotate(float angleDeg, const Vec3& axis)` | public |
| `scale` | `static Mat4 scale(const Vec3& v)` | public |
| `ptr` | `const float* ptr() const` | public |
| `toRadians` | `static float toRadians(float deg)` | private |

**Source**: `src/math/mat4.cpp`

### 6. Renderer (`include/renderer/renderer.h:11`)

**Fields (3)**:

| Name | Type | Access |
|------|------|--------|
| `RendererState` | `struct` (forward-declared, defined in .cpp) | private |
| `m_state` | `std::unique_ptr<RendererState>` | private |
| `m_frameCount` | `uint64_t` | private |

**Methods (11)**:

| Name | Signature | Access |
|------|-----------|--------|
| `initialize` | `bool initialize()` | public |
| `shutdown` | `void shutdown()` | public |
| `beginFrame` | `void beginFrame()` | public |
| `renderScene` | `void renderScene(const Scene&, const Camera&)` | public |
| `endFrame` | `void endFrame()` | public |
| `setClearColor` | `void setClearColor(float r, float g, float b, float a = 1.0f)` | public |
| `getClearColor` | `void getClearColor(float& r, float& g, float& b) const` | public |
| `setViewport` | `void setViewport(int x, int y, int width, int height)` | public |
| `readPixels` | `bool readPixels(std::vector<uint8_t>&, int&, int&)` | public |
| `getFrameCount` | `uint64_t getFrameCount() const` | public |
| `incrementFrameCount` | `void incrementFrameCount()` | public |

**Source**: `src/renderer/renderer.cpp` (includes embedded GLSL shader strings)

### 7. Camera (`include/scene/camera.h:9`)

**Fields (10)**:

| Name | Type | Access |
|------|------|--------|
| `m_type` | `ProjectionType` | private |
| `m_position` | `Vec3` | private |
| `m_target` | `Vec3` | private |
| `m_up` | `Vec3` | private |
| `m_fovDeg` | `float` | private |
| `m_nearPlane` | `float` | private |
| `m_farPlane` | `float` | private |
| `m_aspectRatio` | `float` | private |
| `m_viewMatrix` | `Mat4` | private |
| `m_projectionMatrix` | `Mat4` | private |

**Methods (16)**:

| Name | Signature | Access |
|------|-----------|--------|
| `setPosition` | `void setPosition(const Vec3&)` | public |
| `lookAt` | `void lookAt(const Vec3&)` | public |
| `setFOV` | `void setFOV(float)` | public |
| `setNearPlane` | `void setNearPlane(float)` | public |
| `setFarPlane` | `void setFarPlane(float)` | public |
| `setAspectRatio` | `void setAspectRatio(float)` | public |
| `getProjectionType` | `ProjectionType getProjectionType() const` | public |
| `getPosition` | `const Vec3& getPosition() const` | public |
| `getTarget` | `const Vec3& getTarget() const` | public |
| `getFOV` | `float getFOV() const` | public |
| `getNearPlane` | `float getNearPlane() const` | public |
| `getFarPlane` | `float getFarPlane() const` | public |
| `getAspectRatio` | `float getAspectRatio() const` | public |
| `getViewMatrix` | `const Mat4& getViewMatrix() const` | public |
| `getProjectionMatrix` | `const Mat4& getProjectionMatrix() const` | public |
| `update` | `void update()` | public |

**Source**: `src/scene/camera.cpp`

### 8. Primitive (`include/scene/primitive.h:12`)

**Fields (15)**:

| Name | Type | Access |
|------|------|--------|
| `s_nextID` | `uint64_t` | private |
| `m_id` | `uint64_t` | private |
| `m_name` | `std::string` | private |
| `m_type` | `PrimitiveType` | private |
| `m_position` | `Vec3` | private |
| `m_rotation` | `Vec3` | private |
| `m_scale` | `Vec3` | private |
| `m_color` | `Vec3` | private |
| `m_rotationSpeed` | `Vec3` | private |
| `m_vertices` | `std::vector<float>` | private |
| `m_indices` | `std::vector<uint32_t>` | private |
| `m_vao` | `unsigned int` | private |
| `m_vbo` | `unsigned int` | private |
| `m_ebo` | `unsigned int` | private |
| `m_uploaded` | `bool` | private |

**Methods (25)**:

| Name | Signature | Access |
|------|-----------|--------|
| `getID` | `uint64_t getID() const` | public |
| `getName` | `const std::string& getName() const` | public |
| `setName` | `void setName(const std::string&)` | public |
| `setPosition` | `void setPosition(const Vec3&)` | public |
| `setRotation` | `void setRotation(const Vec3&)` | public |
| `setScale` | `void setScale(const Vec3&)` | public |
| `setColor` | `void setColor(const Vec3&)` | public |
| `setRotationSpeed` | `void setRotationSpeed(const Vec3&)` | public |
| `getType` | `PrimitiveType getType() const` | public |
| `getPosition` | `const Vec3& getPosition() const` | public |
| `getRotation` | `const Vec3& getRotation() const` | public |
| `getScale` | `const Vec3& getScale() const` | public |
| `getColor` | `const Vec3& getColor() const` | public |
| `getRotationSpeed` | `const Vec3& getRotationSpeed() const` | public |
| `getVertices` | `const std::vector<float>& getVertices() const` | public |
| `getIndices` | `const std::vector<uint32_t>& getIndices() const` | public |
| `getVAO` | `unsigned int getVAO() const` | public |
| `getVBO` | `unsigned int getVBO() const` | public |
| `getEBO` | `unsigned int getEBO() const` | public |
| `getIndexCount` | `size_t getIndexCount() const` | public |
| `uploadGPU` | `void uploadGPU()` | public |
| `bind` | `void bind() const` | public |
| `unbind` | `void unbind() const` | public |
| `nextID` | `static uint64_t nextID()` | public |
| `generateGeometry` | `void generateGeometry()` | private |

**Source**: `src/scene/primitive.cpp`

### 9. Scene (`include/scene/scene.h:13`)

**Fields (5)**:

| Name | Type | Access |
|------|------|--------|
| `m_name` | `std::string` | private |
| `m_primitives` | `std::vector<std::shared_ptr<Primitive>>` | private |
| `m_cameras` | `std::vector<std::shared_ptr<Camera>>` | private |
| `m_activeCameraIndex` | `int` | private |

Note: Ground truth scanner reports duplicate `m_primitives` due to parsing error (a return-type `getPrimitives()` was misidentified). Actual unique fields: 4 data + 1 getter return type artifact = reported 5.

**Methods (17)**:

| Name | Signature | Access |
|------|-----------|--------|
| `getName` | `const std::string& getName() const` | public |
| `getPrimitiveCount` | `size_t getPrimitiveCount() const` | public |
| `getCameraCount` | `size_t getCameraCount() const` | public |
| `addPrimitive` | `void addPrimitive(std::shared_ptr<Primitive>)` | public |
| `removePrimitive` | `void removePrimitive(int)` | public |
| `removePrimitiveByID` | `bool removePrimitiveByID(uint64_t)` | public |
| `update` | `void update(float deltaTime)` | public |
| `findByID` | `std::shared_ptr<Primitive> findByID(uint64_t) const` | public |
| `findByName` | `std::shared_ptr<Primitive> findByName(const std::string&) const` | public |
| `findIndexByID` | `int findIndexByID(uint64_t) const` | public |
| `getPrimitives` | `const std::vector<std::shared_ptr<Primitive>>& getPrimitives() const` | public |
| `addCamera` | `void addCamera(std::shared_ptr<Camera>)` | public |
| `setActiveCamera` | `void setActiveCamera(int)` | public |
| `getActiveCamera` | `std::shared_ptr<Camera> getActiveCamera() const` | public |
| `rebuildIDIndex` | `void rebuildIDIndex()` | private |

**Source**: `src/scene/scene.cpp`

### 10. SceneBuilder (`include/scene/scene_builder.h:30`)

**Fields (0)**: Static utility class

**Methods (5)**:

| Name | Signature | Access |
|------|-----------|--------|
| `buildRoom` | `static std::shared_ptr<Scene> buildRoom()` | public |
| `importState` | `static std::shared_ptr<Scene> importState(const std::string&, ProjectManifest&)` | public |
| `exportState` | `static void exportState(const Scene&, const std::string&)` | public |
| `exportHSC` | `static void exportHSC(const Scene&, const Camera&, const ProjectManifest&, const std::string&)` | public |
| `createProject` | `static bool createProject(const std::string&, const std::string&)` | public |

**Source**: `src/scene/scene_builder.cpp`

### 11. Material (`include/renderer/material.h:9`)

**Fields (5)**:

| Name | Type | Access |
|------|------|--------|
| `m_name` | `std::string` | private |
| `m_albedo` | `Vec3` | private |
| `m_albedoMap` | `std::shared_ptr<Texture>` | private |
| `m_roughness` | `float` | private |
| `m_metallic` | `float` | private |

**Methods (11)**:

| Name | Signature | Access |
|------|-----------|--------|
| `Material` | `Material(const std::string&)` | public |
| `getName` | `const std::string& getName() const` | public |
| `setAlbedo` | `void setAlbedo(const Vec3&)` | public |
| `getAlbedo` | `const Vec3& getAlbedo() const` | public |
| `setAlbedoMap` | `void setAlbedoMap(std::shared_ptr<Texture>)` | public |
| `getAlbedoMap` | `std::shared_ptr<Texture> getAlbedoMap() const` | public |
| `hasAlbedoMap` | `bool hasAlbedoMap() const` | public |
| `setRoughness` | `void setRoughness(float)` | public |
| `getRoughness` | `float getRoughness() const` | public |
| `setMetallic` | `void setMetallic(float)` | public |
| `getMetallic` | `float getMetallic() const` | public |
| `apply` | `void apply(unsigned int shaderProgram) const` | public |

**Source**: `src/renderer/material.cpp` (untracked)

### 12. Texture (`include/renderer/texture.h:6`)

**Fields (4)**:

| Name | Type | Access |
|------|------|--------|
| `m_rendererID` | `unsigned int` | private |
| `m_width` | `int` | private |
| `m_height` | `int` | private |
| `m_channels` | `int` | private |

**Methods (6)**:

| Name | Signature | Access |
|------|-----------|--------|
| `Texture` | `Texture(const std::string&)` | public |
| `~Texture` | `~Texture()` | public |
| `bind` | `void bind(unsigned int slot = 0) const` | public |
| `unbind` | `void unbind() const` | public |
| `getWidth` | `int getWidth() const` | public |
| `getHeight` | `int getHeight() const` | public |
| `getID` | `unsigned int getID() const` | public |

**Source**: `src/renderer/texture.cpp` (untracked)

---

## Structs (8)

### 1. PropertyChange (`include/bridge/bridge.h:17`)

| Field | Type |
|-------|------|
| `objectID` | `std::string` |
| `property` | `std::string` |
| `oldValue` | `std::string` |
| `newValue` | `std::string` |

### 2. SceneSnapshot (`include/bridge/bridge.h:24`)

| Field | Type |
|-------|------|
| `revision` | `uint64_t` |
| `timestamp` | `double` |
| `id` | `uint64_t` |
| `name` | `std::string` |
| `type` | `int` |
| `objects` | `std::vector<ObjSnapshot>` |
| `camFov` | `float` |

### 3. ObjSnapshot (`include/bridge/bridge.h:27`)

| Field | Type |
|-------|------|
| `id` | `uint64_t` |
| `name` | `std::string` |
| `type` | `int` |

### 4. Command (`include/bridge/command_queue.h:9`)

| Field | Type |
|-------|------|
| `type` | `Type` (enum) |
| `seq` | `uint64_t` |
| `index` | `int` |
| `objectID` | `uint64_t` |
| `px` | `float` |
| `rx` | `float` |
| `sx` | `float` |
| `cr` | `float` |
| `primType` | `int` |
| `fov` | `float` |
| `sinceRevision` | `uint64_t` |

### 5. WindowProps (`include/core/window.h:9`)

| Field | Type |
|-------|------|
| `title` | `std::string` |
| `width` | `int` |
| `height` | `int` |
| `vsync` | `bool` |

### 6. Vec3 (`include/math/vec3.h:5`)

| Field | Type |
|-------|------|
| `x` | `float` |
| `y` | `float` |
| `z` | `float` |

### 7. WorldObject (`include/scene/scene_builder.h:8`)

| Field | Type |
|-------|------|
| `id` | `std::string` |
| `type` | `std::string` |

### 8. ProjectManifest (`include/scene/scene_builder.h:17`)

| Field | Type |
|-------|------|
| `hscVersion` | `std::string` |
| `engineVersion` | `std::string` |
| `projectName` | `std::string` |
| `description` | `std::string` |
| `sceneName` | `std::string` |
| `clearColorR` | `float` |
| `clearColorG` | `float` |
| `clearColorB` | `float` |
| `worldRevision` | `uint64_t` |
| `filePath` | `std::string` |

---

## Enums (3)

### 1. Type (`include/bridge/command_queue.h:10`)

```
CMD_NONE, CMD_ADD_PRIMITIVE, CMD_REMOVE_PRIMITIVE, CMD_SET_POSITION,
CMD_SET_ROTATION, CMD_SET_SCALE, CMD_SET_COLOR, CMD_SET_CAMERA,
CMD_SET_CLEAR_COLOR, CMD_SCENE_STATE, CMD_OBSERVE_SCENE,
CMD_OBSERVE_OBJECT, CMD_OBSERVE_CAMERA, CMD_OBSERVE_HEALTH,
CMD_CAPTURE_FRAME, CMD_SAVE_PROJECT, CMD_LOAD_PROJECT, CMD_HEALTH
```

### 2. ProjectionType (`include/scene/camera.h:7`)

Values not parsed by scanner (likely `PERSPECTIVE, ORTHOGRAPHIC`)

### 3. PrimitiveType (`include/scene/primitive.h:10`)

Values not parsed by scanner (likely `TRIANGLE, CUBE, SPHERE, QUAD`)

---

## Ownership Relationships

```
main.cpp ──────────→ Window, Renderer, Scene, Camera, Primitive
bridge_main.cpp ───→ Window, Renderer, Scene, Camera, Primitive, Bridge
room_demo.cpp ─────→ Window, Renderer, Scene, Camera, Primitive, SceneBuilder
workspace_main.cpp → Window, Renderer, Scene, Camera, Primitive, SceneBuilder, Bridge

Bridge ──owns──→ PipeServer, CommandQueue
         ──uses──→ Scene, Camera, Renderer, ProjectManifest, SceneSnapshot
Scene ──owns──→ Primitive[], Camera[]
Camera ──has──→ Mat4 (view/projection)
Primitive ──has──→ Vec3 (position/rotation/scale/color), PrimitiveType
Renderer ──owns──→ RendererState (PIMPL)
```

## Build Targets (CMakeLists.txt)

| Target | Type | Files |
|--------|------|-------|
| `harmonic-engine` | Executable | `src/main.cpp`, all libs |
| `bridge-mode` | Executable | `src/bridge_main.cpp`, all libs |
| `room-demo` | Executable | `src/room_demo.cpp`, all libs |
| `workspace` | Executable | `src/workspace_main.cpp`, all libs |
| `test_math` | Test | `tests/test_math.cpp` |
| `test_scene` | Test | `tests/test_scene.cpp` |
