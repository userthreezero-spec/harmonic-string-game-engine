# Harmonic String Engine — Architecture

## Overview

HSE is a minimal rendering foundation for a game engine. The current v0.1 provides:

- Window creation (GLFW)
- OpenGL 3.3 rendering
- Basic scene/camera/primitive hierarchy
- Math library (Vec3, Mat4)

## Component Diagram

```
Application (main.cpp)
    │
    ▼
Window (GLFW)
    │
    ▼
Renderer (OpenGL 3.3)
    ├── Shader (GLSL 330)
    └── Draw Calls
    │
    ▼
Scene (flat container)
    ├── Camera (perspective/orthographic)
    └── Primitive (Triangle, Quad, Cube)
    │
    ▼
Math (Vec3, Mat4)
```

## Modules

### core/window
- **Responsibility:** OS window creation, input handling
- **Dependencies:** GLFW 3.3, OpenGL
- **Key Types:** `hse::Window`, `hse::WindowProps`
- **Thread Safety:** Single-threaded

### renderer/renderer
- **Responsibility:** OpenGL initialization, draw calls, shader management
- **Dependencies:** GLEW, OpenGL 3.3, scene/camera, scene/primitive
- **Key Types:** `hse::Renderer`
- **Thread Safety:** Single-threaded

### scene/scene
- **Responsibility:** Container for primitives and cameras
- **Dependencies:** scene/primitive, scene/camera
- **Key Types:** `hse::Scene`

### scene/camera
- **Responsibility:** View/projection matrix computation
- **Dependencies:** math/vec3, math/mat4
- **Key Types:** `hse::Camera`, `hse::ProjectionType`

### scene/primitive
- **Responsibility:** Geometry generation and GPU upload
- **Dependencies:** math/vec3, math/mat4, OpenGL
- **Key Types:** `hse::Primitive`, `hse::PrimitiveType`
- **Resource Ownership:** Owns VAO/VBO/EBO (non-copyable)

### math/vec3
- **Responsibility:** 3D vector operations
- **Dependencies:** None
- **Key Types:** `hse::Vec3`

### math/mat4
- **Responsibility:** 4x4 matrix operations, transforms
- **Dependencies:** math/vec3
- **Key Types:** `hse::Mat4`

## Rendering Pipeline

1. **Window** creates GLFW context
2. **Renderer** initializes GLEW, compiles shaders
3. **Scene** holds primitives and camera
4. **Camera** computes view/projection matrices
5. **Primitive** uploads geometry to GPU
6. **Render Loop:**
   - `beginFrame()` — clear buffers
   - `renderScene()` — bind shader, set uniforms, draw primitives
   - `endFrame()` — (placeholder)
   - `swapBuffers()` — present

## Build System

CMake 3.20+ with C++20 standard. Static library `hse_core` linked by `hse_main` and test executables.

## Known Defects

1. `PrimitiveType::Sphere` is a stub (single vertex)
2. No texture support
3. No lighting
4. No material system
5. Orthographic projection uses hardcoded bounds
6. No resource management beyond basic GPU upload
7. Single-threaded rendering

## Design Goals (v0.1)

- Minimal rendering foundation
- Clean separation of concerns
- Evidence-grounded architecture
- Testable components
- Extensible for future development

## Architectural Constraints

- OpenGL 3.3 core profile
- Single-threaded
- No external dependencies beyond GLFW/GLEW/OpenGL
- Windows primary target
