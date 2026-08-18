# HSE-012 — Mission Selection

## Chosen Mission: TEXTURE MAPPING

### Objective
Integrate 2D texture mapping support into the Harmonic String Engine (HSE).

### Why this mission?
1. **Multi-layer Reasoning**: Requires knowledge of C++ (memory management), OpenGL (API calls), Math (UV coordinates), and Engine Architecture (Resource management).
2. **GPU Lifecycle**: Textures introduce complex GPU resource lifetimes (creation, binding, destruction) that test RAII and ownership principles.
3. **Material System Foundation**: Textures are the first step towards a proper material system, moving beyond raw vertex colors/simple shading.
4. **Measurable Success**: Success can be verified visually (runtime rendering) and structurally (symbol accuracy).

### Requirements
1. **Resource Loading**: Load image files (e.g., `.png`, `.jpg`) into GPU memory.
2. **Class Abstraction**: Create or modify classes to represent a Texture.
3. **Vertex Layout**: Update the vertex structure to include UV coordinates.
4. **Shader Support**: Update shaders to sample from textures.
5. **Ownership**: Ensure deterministic destruction of GPU textures via RAII.

### Status: FROZEN
Mission definition is complete and locked for architecture generation.
