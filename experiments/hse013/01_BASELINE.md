# HSE-013 — Baseline & Archaeology

## Current State Assessment

### 1. Texture Mapping (HSE-012)
- **Texture Class**: `hse::Texture` manages the OpenGL texture object via RAII. It uses `stb_image` for loading.
- **Primitive Class**:
    - Stores `std::shared_ptr<Texture> m_texture`.
    - Generates geometry with interleaved UV coordinates (stride 5: `x,y,z, u,v`).
    - Handles GPU upload for these 5 floats.
    - Has `setColor(Vec3)` which is used as a tint.
- **Renderer Class**:
    - Contains a hardcoded shader program in `renderer.cpp`.
    - Shader supports optional texturing via `uHasTexture` uniform.
    - If `uHasTexture` is true, fragment color is `texture(uTexture, vUV) * vec4(uColor, 1.0)`.
    - Otherwise, it uses `vec4(uColor, 1.0)`.

### 2. Responsibilities
- **Primitive**: Owns its geometry, color, and a *usage* reference to a texture.
- **Renderer**: Owns the shader program and the logic for binding uniforms.
- **Texture**: Owns the lifecycle of the OpenGL texture resource.
- **Scene**: A container for primitives and cameras.
- **SceneBuilder**: Handles `.hsc` serialization. **Crucially**, it currently ignores texture data during import/export.

### 3. Material Concepts
- **Currently exists**: Albedo color (tint), 2D Texture (optional).
- **Missing**: A formal `Material` abstraction. Currently, "material" state is a mix of `Primitive` members and `Renderer` uniform-setting logic.
- **Missing**: Parameters like roughness, metallic, transparency.
- **Missing**: Shader permutations (multiple shaders for different material types).

### 4. Code Inventory
- `include/renderer/texture.h`: `class Texture`
- `include/scene/primitive.h`: `class Primitive` with `m_texture`, `m_color`.
- `src/renderer/renderer.cpp`: Hardcoded shaders and draw loop.
- `src/scene/scene_builder.cpp`: Minimal JSON parser for `.hsc`.

## Conclusion
HSE has a functional but "naive" material implementation. Properties are tightly coupled to the `Primitive` class, and the `Renderer` is not yet generic enough to handle complex material state transitions without manual uniform location lookups every frame.
