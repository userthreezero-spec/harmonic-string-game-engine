# HSE-015 Symbol Verification

## Symbols Referenced

| Symbol | Verified | Source | Status |
|--------|----------|--------|--------|
| Camera | ✅ | `include/scene/camera.h:9` | EXISTS |
| Camera::setPosition | ✅ | `include/scene/camera.h:13` | EXISTS |
| Camera::lookAt | ✅ | `include/scene/camera.h:14` | EXISTS |
| Camera::update | ✅ | `include/scene/camera.h:30` | EXISTS |
| Camera::getPosition | ✅ | `include/scene/camera.h:21` | EXISTS |
| Camera::getTarget | ✅ | `include/scene/camera.h:22` | EXISTS |
| Camera::m_position | ✅ | `include/scene/camera.h:34` | EXISTS (private) |
| Camera::m_target | ✅ | `include/scene/camera.h:35` | EXISTS (private) |
| Camera::m_up | ✅ | `include/scene/camera.h:36` | EXISTS (private) |
| Camera::m_viewMatrix | ✅ | `include/scene/camera.h:42` | EXISTS (private) |
| Camera::m_projectionMatrix | ✅ | `include/scene/camera.h:43` | EXISTS (private) |
| Scene::update | ✅ | `src/scene/scene.cpp:117` | EXISTS |
| Scene::getActiveCamera | ✅ | `src/scene/scene.cpp:73` | EXISTS |
| Mat4::rotate | ✅ | `include/math/mat4.h:17` | EXISTS |
| Mat4::lookAt | ✅ | `include/math/mat4.h:15` | EXISTS |
| Vec3 operator+ | ✅ | `include/math/vec3.h:13` | EXISTS |
| Vec3 operator- | ✅ | `include/math/vec3.h:14` | EXISTS |
| Vec3 operator* | ✅ | `include/math/vec3.h:15` | EXISTS |
| Vec3::length | ✅ | `include/math/vec3.h:17` | EXISTS |
| Window::getDeltaTime | ✅ | `include/core/window.h:30` | EXISTS |
| Command | ✅ | `include/bridge/command_queue.h` | EXISTS |
| Bridge | ✅ | `include/bridge/bridge.h` | EXISTS |

## Symbols Introduced

| Symbol | Purpose | Verified Against |
|--------|---------|-----------------|
| Camera::m_orbitRadius | Orbit distance from target | Camera field pattern (m_ prefix) |
| Camera::m_orbitYaw | Current orbital angle | Orbit math convention |
| Camera::m_orbitSpeed | Angular velocity (rad/s) | Consistent with getDeltaTime |
| Camera::m_orbitEnabled | Enable/disable flag | Consistent with bool patterns |
| Camera::setOrbitRadius | Setter | Matches setPosition pattern |
| Camera::setOrbitSpeed | Setter | Matches setFOV pattern |
| Camera::enableOrbit | Enable/disable | New method, consistent naming |
| Camera::isOrbitEnabled | Getter | Matches isValid pattern |
| Camera::updateOrbit | Per-frame tick | Matches update() pattern |
| Command::CMD_ORBIT_START | Bridge command | Matches CMD_ prefix convention |
| Command::CMD_ORBIT_STOP | Bridge command | Matches CMD_ prefix convention |
| Command::CMD_ORBIT_SET | Bridge command | Matches CMD_ prefix convention |
| Command::orbitRadius | Command field | Matches px/py/pz pattern |
| Command::orbitSpeed | Command field | Matches fov pattern |

## Accuracy
- All pre-existing symbols: 100% verified against source
- All new symbols: derived from existing patterns in the codebase
- No fabricated symbols
