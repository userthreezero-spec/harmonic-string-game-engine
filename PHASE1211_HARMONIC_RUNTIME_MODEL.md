# PHASE 1211 — Harmonic Runtime Model — HarmonicStringGameEngine

**Derived from source tree 2026-09-02, no invented equations.**

## Canonical Paths
- Root: `C:\Users\Rico\HarmonicStringGameEngine`
- Entry: `src/main.cpp:10` (`hse_main`), `src/room_demo.cpp:10` (`hse_room_demo`) — both canonical, `src/home_demo` (this phase)
- Build: `CMakeLists.txt:11` `hse_core` (window, renderer, scene, camera, primitive, scene_builder, picker, math)
- Scene: `include/scene/scene.h:15`, `src/scene/scene.cpp:130`
- Primitive: `include/scene/primitive.h:14`, `src/scene/primitive.cpp:60`
- Renderer: `src/renderer/renderer.cpp:121` initialize, `211` renderScene

## Per-Subsystem Wiring

| Subsystem | Creates | Owns | Calls | Calls | Data In | Data Out | Next | Status |
|-----------|---------|------|-------|-------|---------|----------|------|--------|
| Window | `Window(props)` `src/core/window.cpp:27` | caller (`main`) | `isValid/pollEvents/swapBuffers/shouldClose/getDeltaTime` | `glfwInit/CreateWindow/MakeContextCurrent` | title/size/vsync | `GLFWwindow*`, width/height, dt via `glfwGetTime` | Renderer context | PROVEN_CONNECTED |
| Renderer | `Renderer()` `renderer.cpp:115` | caller | `initialize/shutdown/beginFrame/renderScene/endFrame` | `glCreateShader/Link/UseProgram` | Scene, Camera, Materials, selectedID | pixels, shaderProgram, cached uniform locs | Window swap | PROVEN_CONNECTED (locs cached `renderer.cpp:179`) |
| Scene | `make_shared<Scene>(name)` `scene.cpp:10` | caller | `addPrimitive/addCamera/addLight/addMaterial/reparent/update/computeAllWorldMatrices/checkCollision` | Primitive, Camera | primitives/lights/materials/cameras | primitives vector, world matrices, time via dt | Renderer | PROVEN_CONNECTED (update live) |
| Primitive | `make_shared<Primitive>(Cube/Quad/Group)` `primitive.cpp:10` | Scene + parent hierarchy | `setPosition/Rotation/Scale/RotationSpeed/Material/Parent/computeWorldMatrix/uploadGPU` | `generateGeometry` `primitive.cpp:132`, `glGenBuffers` | pos/rot/scale/speed/type/name/material | vertices/indices/worldMatrix/boundingBox | Renderer draw | PROVEN_CONNECTED |
| Material | `make_shared<Material>(name)` `material.h:10` | Scene + Primitive | `setAlbedo/Roughness/Metallic/AlbedoMap/apply` | — | albedo/roughness/metallic/texture | uniform values | Renderer via `material->apply` `renderer.cpp:267` | PROVEN_CONNECTED |
| Camera | `make_shared<Camera>(Perspective)` `camera.h:11` | Scene | `setPosition/lookAt/setFOV/setAspectRatio/enableOrbit/offsetOrbit/pan/zoom/updateOrbit/update` | `Mat4::lookAt/perspective` | pos/target/fov/aspect/orbitRadius/Yaw/Pitch | view/proj matrices | Renderer uniforms `renderer.cpp:218` | PROVEN_CONNECTED |
| Input | GLFW via `Window` `window.cpp:49` | Window | `pollEvents/isKeyPressed/isMousePressed/getMousePosition/getDeltaTime` + resize/scroll callbacks | `glfwPollEvents` | OS key/mouse/scroll, dt | close flag, w/h, mouse, scroll | Scene via direct `isKeyPressed` in app loop | PROVEN_CONNECTED (minimal, not event-bus) |
| Simulation | `Scene::update` `scene.cpp:130` | — | per frame `rot += speed*dt` + `cam updateOrbit` | Primitive | dt | mutated rotations | `computeAllWorldMatrices` | PROVEN_CONNECTED (minimal) |
| Harmonic Stream | **ABSENT** | — | — | — | — | — | — | ABSENT |
| Harmonic State Engine | **ABSENT** | — | — | — | — | — | — | ABSENT |
| Relationships | `Primitive` parent pointer `primitive.h:89` `weak_ptr<Primitive> m_parent` + `m_children` vector | Primitive | `setParent/addChild/removeChild/markDirtyRecursive` | — | parent ID | worldMatrix propagation | Scene `reparent` + `computeAllWorldMatrices` | PROVEN_CONNECTED (hierarchy) |
| Asset/HSC | `scene_builder.h:33` `importState/exportHSC` JSON | SceneBuilder static | `importState/exportState/exportHSC` | `fstream` | `*.hsc.json` / `scene_state.json` | Scene graph | caller | PROVEN_CONNECTED |
| Physics | `BoundingBox` `primitive.cpp:253` | Primitive | `getBoundingBox/getAggregateBoundingBox`, `Scene::checkCollision` `scene.cpp:146` | — | worldMatrix, unit corners | BoundingBox, collisions | caller | PROVEN_CONNECTED (simple AABB) |
| Audio | **ABSENT** | — | — | — | — | — | — | ABSENT |
| Networking | **ABSENT** | — | — | — | — | — | — | ABSENT |

## Execution Graph (live, delta-time driven)

```
startup: main() [main.cpp:10] / room_demo [room_demo.cpp:10]
 → Window(props) [window.cpp:27] glfwInit, CreateWindow, MakeContext, callbacks
 → Renderer::initialize [renderer.cpp:121] glewInit, compile/link vertex+fragment, cache Uniforms locView/projection/model/viewPos/light/material
 → Scene("Main Scene") [scene.cpp:10], Camera Perspective [camera.h:11] setPosition/LookAt/Aspect/Update
 → Material Green [material.h:10] setAlbedo, addMaterial
 → Primitive Cube/Triangle [primitive.cpp:10] generateGeometry, setPosition/RotationSpeed/Material, uploadGPU [primitive.cpp:208] glGenBuffers
 → addPrimitive/addCamera/addLight to Scene
 → game loop [main.cpp:48] dt = window.getDeltaTime() [window.cpp:103] clamp? no (raw), pollEvents
   → Scene::update(dt) [scene.cpp:130] for each prim: rot += speed*dt; markDirty via setRotation; cam updateOrbit(dt)
   → Renderer::beginFrame [renderer.cpp:206] glClear
   → Renderer::renderScene(scene,camera) [renderer.cpp:211] glUseProgram, Use view/proj/viewPos/light uniforms (cached locs), const_cast computeAllWorldMatrices [scene.cpp:123] → recursive computeWorldMatrix [primitive.cpp:60] S*R*T*Parent → per prim if Group skip, glUniform uModel, material.apply, bind VAO, glDrawElements
   → Renderer::endFrame
   → Window::swapBuffers [window.cpp:77]
 → shutdown: Renderer::shutdown [renderer.cpp:199] glDeleteProgram; Window::~Window [window.cpp:59] glfwDestroyWindow (glfwTerminate not called, OS cleanup)
```

Data flow: `input(deltaTime from glfwGetTime)` → `Scene::update` mutates `Primitive.rotation` → `computeWorldMatrix` dirty propagation → `Renderer` consumes `getWorldMatrix` → `glDrawElements`. Camera orbit similarly: `Scene::update` → `Camera::updateOrbit` → `Renderer` view/proj.

No harmonic state, no E/rho, no resonance calculation exists to flow.

## Wiring Integrity Post-Freeze

- Bootstrap → loop → rendering: PROVEN (window+renderer+scene+primitive+camera+material).
- Loop → simulation (rotationSpeed): PROVEN (minimal but live).
- Simulation → hierarchy propagation → rendering: PROVEN via `computeAllWorldMatrices`.
- Input → simulation: PROVEN minimal via `isKeyPressed` and deltaTime; no harmonic event layer.
- Harmonic stream: ABSENT (honest, not mocked).
- Asset → scene → render: PROVEN via `SceneBuilder::buildRoom` and `importState`.

No second engine, no duplicate scene system.

