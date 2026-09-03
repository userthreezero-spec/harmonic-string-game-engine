# PHASE 1211 — Harmonic Stream Model Forensics — HarmonicStringGameEngine

**Engine:** `C:\Users\Rico\HarmonicStringGameEngine` v0.1.0 | **Date:** 2026-09-02 | **Mode:** source-to-runtime trace, no inference by name

---

## 1. Inventory

Scanned: `include/scene/scene.h:15`, `src/scene/scene.cpp:130`, `include/scene/primitive.h:14`, `src/scene/primitive.cpp:60`, `include/scene/camera.h:10`, `src/scene/camera.cpp:55`, `src/renderer/renderer.cpp:1`, `include/renderer/material.h:1`, `include/math/vec3.h:5`, `include/math/mat4.h:6`, `src/main.cpp:48`, `src/room_demo.cpp:23`, `include/scene/scene_builder.h:30`, `src/scene/scene_builder.cpp:32`. Checked for absent dirs: `src/audio`, `src/physics`, `src/stream`, `src/harmonic`, `include/scene/timeline.h`, `include/scene/animated*` — **not present** (0 hits, verified via directory listing).

## 2. Concept Classification

| Concept | Verdict | Evidence `file:line` |
|---------|---------|----------------------|
| harmonic state | **ABSENT** | No `HarmonicState`, `harmonic`, `frequency`, `amplitude`, `phase`, `energy` field in any `include/*` or `src/*`. Grep for `harmonic` = 0 hits. |
| frequency | **ABSENT** (only `rotationSpeed` as generic) | `primitive.h:32` `setRotationSpeed(Vec3)`, `scene.cpp:132` `rot += speed*deltaTime`. This is angular velocity deg/s, not material frequency. No Hz, no E/rho. |
| phase | **ABSENT** | No phase accumulator except `camera.cpp:57` `m_orbitYaw += speed*deltaTime` — orbit yaw, not harmonic phase. |
| amplitude | **ABSENT** | No amplitude variable. Scale is direct `setScale` `primitive.h:31`, no oscillation amplitude. |
| energy | **ABSENT** | 0 hits. |
| material properties | **REPRESENTED_ONLY_AS_METADATA** | `material.h:14` `albedo/roughness/metallic` + optional `Texture` — visual PBR properties only. No density, tension, E, rho. `scene_builder.cpp:35` creates `mat_floor`/`mat_wall` with colors only. |
| density | **ABSENT** | 0 hits. |
| tension | **ABSENT** | 0 hits. |
| resonance | **ABSENT** | 0 hits. |
| coupling | **PARTIALLY_IMPLEMENTED** | Via `Primitive` hierarchy: `primitive.h:39` `setParent` / `addChild` + `primitive.cpp:60` `computeWorldMatrix` local `S*R*T` * parentWorld. Coupling = rigid transform inheritance, not spring k. No coefficient. |
| oscillation | **VISUAL/ANIMATED APPROXIMATION** (minimal) | `scene.cpp:133` `rot += speed*deltaTime` produces continuous rotation per `Scene::update`. `src/main.cpp:39` `triangle setRotationSpeed({0,1,0})`. No sin/cos envelope beyond linear ramp. |
| propagation | **PROVEN_IMPLEMENTED** | `primitive.cpp:83` `m_worldMatrix = m_localMatrix * parent->getWorldMatrix()` recursed to children `primitive.cpp:90`. Changing parent position propagates to all descendants via `computeAllWorldMatrices()` `scene.cpp:123` called before render `renderer.cpp:242`. Proven in `tests/test_hierarchy.cpp` and `test_assembly`. |
| relationships | **PARTIALLY_IMPLEMENTED** | String `Relationship` table does not exist; relationships expressed as `Primitive` parent hierarchy (`Group` parent) and via `scene_builder.cpp:350` `parent` JSON field. No semantic types (`supports/part_of`) stored — only parent pointer. |
| spatial relationships | **PROVEN_IMPLEMENTED** | `Vec3 position/scale` `primitive.h:48` + `Mat4 worldMatrix` + `BoundingBox::intersects` `primitive.cpp:253` spatial query. Hierarchy encodes containment. |
| temporal relationships | **ABSENT** | No `Timeline`, no keyframes, no interpolation. Only `deltaTime` accumulation in rotation. |
| transformation | **PROVEN_IMPLEMENTED** | `primitive.cpp:60` `computeWorldMatrix` S*R*T*Parent, consumed in `renderer.cpp:247` `glUniformMatrix4fv(uModel, model)` → `glDrawElements`. |
| emergent behavior | **ABSENT** | No feedback, no collective mode. |

## 3. Mathematical Model Trace

All active equations in native runtime:

| Equation | Where defined | Inputs | Caller | Frequency | State modified | Consumer | Affects sim? | Affects rendering? | Kind |
|----------|---------------|--------|--------|-----------|----------------|----------|--------------|--------------------|------|
| `rot += speed * dt` (component-wise) | `scene.cpp:133` | `getRotation()`, `getRotationSpeed()`, `deltaTime` from `Window::getDeltaTime()` `core/window.cpp:103` `glfwGetTime` | `Scene::update(dt)` per `main.cpp:52` / `room_demo.cpp:43` | per frame, per primitive with non-zero speed | `Primitive::m_rotation` (dirty) | `computeWorldMatrix` → `Renderer` | Yes | Yes (model rotation) | **Procedural** linear |
| `m_orbitYaw += orbitSpeed*dt; pos = target + radius*cosPitch*[cosYaw,sinPitch,sinYaw]` | `camera.cpp:57` | `m_orbitSpeed`, `m_orbitRadius`, `deltaTime` | `scene.cpp:142` `cam->updateOrbit(dt)` gated by `m_orbitEnabled` | per frame if orbit enabled | `Camera::m_position`, `m_viewMatrix`, `m_projectionMatrix` | `renderer.cpp:218` `uView/uProjection` | Yes (camera) | Yes | **Geometric** |
| `perspective(fov,aspect,near,far): tan(fov/2)` | `math/mat4.cpp:perspective` | fov, aspect, near, far | `camera.cpp:104` `update()` | on camera set/change | `m_projectionMatrix` | renderer | Yes | Yes | **Geometric** |
| `lookAt(eye,target,up): f=(center-eye).norm(), s=f x up, u=s x f` | `math/mat4.cpp:lookAt` | eye,target,up | `camera.cpp:101` | per camera update | `m_viewMatrix` | renderer | Yes | Yes | **Geometric** |
| `M_local = S * Rx * Ry * Rz * T ; M_world = M_local * M_parent` (row-vector) | `primitive.cpp:73` | position/rotation/scale, parent world | `computeWorldMatrix` via `scene.cpp:123` `computeAllWorldMatrices` per frame + on setParent/markDirty | per dirty primitive per frame/render | `m_localMatrix`, `m_worldMatrix` | `renderer.cpp:247` | Yes | **Yes** | **Geometric** |
| `BoundingBox expand(world * unitCorner)` + `intersects` | `primitive.cpp:253`, `math/bounding_box.h` | worldMatrix, unit cube corners | `scene.cpp:146` `checkCollision`, `getCollisions` | on query | `BoundingBox` result | caller (game logic) | Yes | No | Geometric |
| `v = sqrt(E/rho)` etc. | **ABSENT natively** | — | — | — | — | — | No | No | — |
| `lerp/sin/cos/tween/timeline` | **ABSENT** | — | — | — | — | — | — | — | — |

Units: `position` in scene units (no physical meters annotation), `rotation` degrees for `Mat4::rotate` `toRadians` `mat4.cpp:toRadians`, `deltaTime` seconds from `glfwGetTime` double, `orbitSpeed` rad/s, `radius` scene units, `scale` unitless, `color` 0-1 linear, `FOV` degrees.

Constants: `MAX` none, `orbitRadius` default 5 `camera.h:58`, `near 0.1 far 1000` `camera.h:51`, `PI 3.14159265` `camera.cpp:71`, `albedo 1,1,1` `material.h:42`.

Numerical integration: **none** (explicit Euler step `rot += speed*dt` is sole integrator, no RK). Interpolation: **none** (no lerp beyond matrix construction). Trig: `sin/cos/tan` in `camera.cpp:61` and `mat4.cpp:rotate/perspective`.

Energy/material/resonance/coupling/propagation beyond rigid hierarchy: **absent**.

## 4. Stream vs Animation Verdict

MODEL A: `time → sin/cos → transform → renderer`
MODEL B: `harmonic state → interacting entities → propagation/coupling/resonance → evolving state → simulation → renderer`

**Current implementation: MODEL A (minimal variant: time → linear rotation → hierarchy → renderer).**

Evidence:
- Per-frame execution IS real: `while(!shouldClose){ dt=getDeltaTime(); pollEvents(); scene->update(dt); computeAllWorldMatrices(); renderScene}` `main.cpp:48`, `room_demo.cpp:40`. Delta-time driven, bounded, clean shutdown `renderer.shutdown()`.
- However downstream is only `rot += speed*dt`, no harmonic state, no coupling coefficient, no resonance calculation. Propagation exists but is rigid parent→child world matrix, not harmonic energy flow.
- No `Timeline`, no `Animated*`, no `sin` envelope in default engine. Camera orbit is optional geometric path.
- Therefore: **genuinely a continuously executing runtime (PROVEN), but not a harmonic stream — ordinary hierarchical animation with delta-time integration.**

Do not upgrade terminology: continuous execution ≠ harmonic stream.

## 5. Multi-Entity Interaction

- **Isolated:** `Primitive setPosition({1,0,0}); computeWorldMatrix()` → world = local only. Verified in `test_hierarchy`.
- **Two related (parent/child):** `parent at (1,0,0)`, `child at (0,1,0) parent=house` → child world `= childLocal * parentWorld` = `(1,1,0)` in world. Proven via `primitive.cpp:84` multiplication order and `scene_builder.cpp:73` `tableGroup` → `tableTop at (0,0,0)` local becomes `(1.5,-0.7,-1.8)` world. **Direct transform inheritance**.
- **Multiple related (hierarchy):** `Group tableGroup` → `tableTop + 4 legs` `scene_builder.cpp:80` all `setParent(tableGroup)` → moving `tableGroup` propagates to 5 descendants in one `computeWorldMatrix` recursion `primitive.cpp:90`. Test `test_assembly` verifies aggregate bounding box merges children.
- **Hierarchy depth 2+:** `house Group → floor Group → wall Primitive` — world = wallLocal * floorWorld * houseWorld. Proven by recursive `computeWorldMatrix`.

Result type: **rigid transform inheritance + spatial bounding-box collision**; no animation synchronization, no harmonic coupling, no resonance.

## 6. Honest Gap

Do not fake physics. Label rotationSpeed as procedural animation, label parent/child as relationship propagation (transform hierarchy), keep energy/resonance/harmonic state ABSENT.

