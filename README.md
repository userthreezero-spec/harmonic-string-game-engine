# Harmonic String Engine - Rendering Foundation v0.1

## Overview

Minimal rendering foundation for the Harmonic String Engine project.

## Architecture

```
Window (GLFW)
    ↓
Renderer (OpenGL 3.3)
    ↓
Scene
    ├── Camera (Perspective/Orthographic)
    └── Primitive (Triangle, Quad, Cube)
```

## Dependencies

- CMake 3.20+
- OpenGL 3.3+
- GLFW 3.3+
- GLEW

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Components

### Window
- GLFW-based windowing
- Input handling
- Resize callbacks

### Renderer
- OpenGL 3.3 core profile
- Shader compilation
- Basic render pipeline

### Scene
- Container for primitives and cameras
- Active camera management

### Camera
- Perspective and orthographic projection
- View matrix computation

### Primitive
- Geometry generation (Triangle, Quad, Cube)
- GPU upload (VAO/VBO/EBO)

### Math
- Vec3, Mat4 types
- Transform operations
- Projection matrices
