# HSE-015 Architecture: Camera Orbit System

## Mission
Add a camera orbit system to the Harmonic String Engine.

## Interpretation
A camera that revolves around a target point at a fixed radius, with controllable angular velocity. The camera continuously updates its position along a circular path while maintaining focus on the target.

## Repository Archaeology Summary

### Camera class (`include/scene/camera.h`)
- 10 fields: m_type, m_position, m_target, m_up, m_fovDeg, m_nearPlane, m_farPlane, m_aspectRatio, m_viewMatrix, m_projectionMatrix
- 16 methods: setPosition, lookAt, setFOV, setNearPlane, setFarPlane, setAspectRatio, getters, update()
- update() recomputes view+projection via Mat4::lookAt()
- No orbit behavior

### Scene::update(dt) (`src/scene/scene.cpp:117-126`)
- Only updates primitive rotation speeds
- Camera is never updated per-frame
- This is the integration gap

### Mat4 (`include/math/mat4.h`)
- rotate(angleDeg, axis) — sufficient for orbital position
- lookAt(eye, center, up) — used by Camera::update()

### Vec3 (`include/math/vec3.h`)
- operator+, operator-, operator*, length, normalized, dot, cross
- Sufficient for orbital math

### Window (`include/core/window.h`)
- getDeltaTime() — available for time-based orbit

### Bridge (`include/bridge/bridge.h`)
- CMD_SET_CAMERA exists — sets position/target/FOV directly
- No orbit control commands

## Architecture

### Changes to Camera class

**New fields (4):**
```
float m_orbitRadius = 5.0f;       // Distance from target
float m_orbitYaw = 0.0f;         // Current angle in radians
float m_orbitSpeed = 1.0f;       // Radians per second
bool  m_orbitEnabled = false;    // Whether orbit is active
```

**New methods (5):**
```
void setOrbitRadius(float radius);
void setOrbitSpeed(float speed);        // radians per second
void enableOrbit(bool enabled);
bool isOrbitEnabled() const;
void updateOrbit(float deltaTime);      // advances yaw, recomputes position
```

**updateOrbit() logic:**
1. If !m_orbitEnabled, return
2. m_orbitYaw += m_orbitSpeed * deltaTime
3. Compute position from spherical coordinates:
   - x = m_target.x + m_orbitRadius * cos(m_orbitYaw)
   - y = m_target.y  (orbit in XZ plane)
   - z = m_target.z + m_orbitRadius * sin(m_orbitYaw)
4. m_position = {x, y, z}
5. update() to recompute matrices

### Changes to Scene::update(dt)

**Add camera orbit tick:**
```
void Scene::update(float deltaTime) {
    // Existing: primitive rotation
    for (auto& prim : m_primitives) { ... }

    // New: camera orbit
    auto cam = getActiveCamera();
    if (cam) {
        cam->updateOrbit(deltaTime);
    }
}
```

### Changes to Bridge

**New commands (3):**
```
CMD_ORBIT_START    — enable orbit with given radius + speed
CMD_ORBIT_STOP     — disable orbit
CMD_ORBIT_SET      — modify radius/speed while orbiting
```

**Command struct additions:**
```
float orbitRadius = 5.0f;
float orbitSpeed = 1.0f;
```

**Bridge observation:**
- Camera observation gains `orbit_enabled`, `orbit_radius`, `orbit_speed` fields

### Files modified
1. `include/scene/camera.h` — 4 fields + 5 methods
2. `src/scene/camera.cpp` — orbit method implementations
3. `include/scene/scene.h` — no change needed (update already exists)
4. `src/scene/scene.cpp` — add camera orbit tick in update()
5. `include/bridge/command_queue.h` — 3 new command types + orbit fields in Command
6. `src/bridge/bridge.cpp` — 3 new command handlers + orbit observation fields
7. `tests/test_orbit.cpp` — NEW: orbit unit tests
8. `CMakeLists.txt` — add test_orbit target

### Files NOT modified
- Renderer (unchanged — uses Camera as-is)
- SceneBuilder (orbit is runtime-only, not persisted)
- Window (unchanged)
- Material (unchanged)
- Math classes (unchanged)

## Verification Strategy
- Unit tests: orbit position computation, enable/disable, speed/radius
- Integration: Scene::update() triggers orbit
- Build: cmake + make clean
- Runtime: orbit_demo runs 60 frames, camera position changes
