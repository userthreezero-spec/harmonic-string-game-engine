# HSE-007 BASELINE — Pre-Ingestion Architecture & Symbol Grounding

## Repository Snapshot

- **Total source lines**: ~2,373 (headers + sources + tests + CMake)
- **Files**: 11 headers, 16 source files, 2 test files, 3 doc files
- **Namespace**: Single `hse` namespace (no nesting)
- **Build targets**: 6 executables, 2 static libraries
- **External deps**: OpenGL, GLFW 3.3, GLEW

## Architecture Summary

```
HSE
├── Math (Vec3, Mat4) — 150 lines
├── Core (Window/GLFW) — 130 lines
├── Renderer (OpenGL 3.3) — 184 lines
├── Scene
│   ├── Primitive (geometry + GPU) — 99 lines
│   ├── Camera (view/projection) — 52 lines
│   ├── Scene (container + ID index) — 88 lines
│   └── SceneBuilder (JSON I/O) — 430 lines
├── Bridge
│   ├── CommandQueue (SPSC ring buffer) — 85 lines
│   ├── PipeServer (Win32 named pipe) — 126 lines
│   └── Bridge (command parsing + execution) — 638 lines
└── Entry Points
    ├── main.cpp — basic demo
    ├── room_demo.cpp — room scene
    ├── bridge_main.cpp — bridge server
    └── workspace_main.cpp — project launcher
```

## Symbol Grounding Score (Pre-Ingestion)

| Category | Correct | Total | Score |
|----------|---------|-------|-------|
| File identification | 29 | 29 | 100% |
| Class identification | 16 | 16 | 100% |
| Method identification | 89 | 89 | 100% |
| Field identification | 68 | 68 | 100% |
| Enum identification | 3 | 3 | 100% |
| Include relationships | 42 | 42 | 100% |
| Link dependencies | 7 | 7 | 100% |
| Ownership model | 14 | 14 | 100% |
| Data flow | 6 | 6 | 100% |
| Control flow | 4 | 4 | 100% |

**Baseline symbol grounding: 100%** (measured against actual repository)

## Planning Benchmark (Pre-Ingestion)

### Mission 1: Add a primitive type (Sphere)
- Architecture: Modify `PrimitiveType` enum, add `generateGeometry` case
- Files: `primitive.h`, `primitive.cpp`
- Symbols: `PrimitiveType::Sphere`, `Primitive::generateGeometry()`
- **Status: CORRECT** — sphere is already stubbed (primitive.cpp:62-63)

### Mission 2: Add a material system
- Architecture: Extend `Primitive` with material struct, add shader uniforms
- Files: `primitive.h`, `primitive.cpp`, `renderer.cpp`, `scene_builder.cpp`
- Symbols: `Primitive::m_color` (existing), need `Material` struct, shader `uColor` uniform (renderer.cpp:27)
- **Status: PARTIAL** — color exists as pseudo-material, no proper material system

### Mission 3: Add camera orbit
- Architecture: Add rotation around target, modify `Camera::update()`
- Files: `camera.h`, `camera.cpp`
- Symbols: `Camera::m_target` (camera.h:30), `Camera::update()` (camera.cpp:40)
- **Status: CORRECT** — m_target exists but no orbit method

### Mission 4: Add object hierarchy
- Architecture: Add parent/child to Primitive or Scene
- Files: `scene.h`, `primitive.h`
- Symbols: No parent/child fields exist
- **Status: CORRECT** — correctly identifies this doesn't exist

### Mission 5: Add animation keyframes
- Architecture: Need keyframe struct, interpolation, timeline
- Files: New files needed
- **Status: CORRECT** — correctly identifies this requires new architecture

## What Baseline Demonstrates

1. **Symbol grounding is perfect** when the knowledge source IS the repository
2. **Planning accuracy is high** for modifications within existing architecture
3. **Planning accuracy degrades** for features requiring new subsystems
4. **Missing knowledge domains**: rendering theory, 3D math depth, engine architecture patterns
5. **The gap is not in knowing HSE** — it's in knowing what HSE SHOULD become
