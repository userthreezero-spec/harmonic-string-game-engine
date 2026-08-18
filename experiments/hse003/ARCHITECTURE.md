# HSE-003 ARCHITECTURE

**Date:** 2026-08-17

---

## Current HSE Architecture

```
Application (main.cpp / room_demo.cpp)
    │
    ▼
Window (GLFW) ──── input, context
    │
    ▼
Renderer (OpenGL 3.3) ──── shaders, draw calls
    │
    ▼
Scene (flat container)
    ├── Primitive[] ──── geometry, transforms, color
    ├── Camera[] ──── view/projection
    └── SceneBuilder ──── constructs scene from specification
    │
    ▼
Math (Vec3, Mat4) ──── transforms, projection
```

## New Components (HSE-003)

| Component | File | Purpose |
|-----------|------|---------|
| SceneBuilder | `include/scene/scene_builder.h`, `src/scene/scene_builder.cpp` | Constructs scenes from specifications |
| RoomDemo | `src/room_demo.cpp` | Demonstrates room construction |
| WorldContract | `assets/worlds/small_room.json` | Structured scene specification |
| SceneState | `assets/worlds/scene_state.json` | Machine-readable scene export |

## Data Flow

```
WorldContract (JSON)
    ↓
SceneBuilder::buildRoom()
    ↓
Scene (11 Primitives + 1 Camera)
    ↓
Renderer::renderScene()
    ↓
OpenGL output
    ↓
SceneBuilder::exportState()
    ↓
SceneState (JSON)
```

## Extension Points

The architecture supports extension through:
1. **New PrimitiveTypes** — Add to enum, implement generateGeometry()
2. **New SceneBuilder methods** — buildFromContract(), buildFromHarmonicString()
3. **New renderers** — Implement Renderer interface for different backends
4. **New exporters** — Add export methods to SceneBuilder

## What Is NOT in the Architecture

- No scene hierarchy (parent-child transforms)
- No material system (beyond flat color)
- No lighting system
- No harmonic model integration
- No serialization/deserialization
- No asset pipeline
