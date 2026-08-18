# HSE-012 — WebOS Engineering Architecture

## Mission: TEXTURE MAPPING

### 1. Proposed File Changes
| File Path | Action | Description |
| :--- | :--- | :--- |
| `include/renderer/texture.h` | **NEW** | Interface for the `Texture` class. |
| `src/renderer/texture.cpp` | **NEW** | Implementation of texture loading and OpenGL object management. |
| `include/scene/primitive.h` | **MODIFY** | Add texture reference and UV data storage. |
| `src/scene/primitive.cpp` | **MODIFY** | Update `generateGeometry` and `uploadGPU` for UV support. |
| `include/renderer/renderer.h` | **MODIFY** | Update state to handle texture uniforms. |
| `src/renderer/renderer.cpp` | **MODIFY** | Update hardcoded shaders and rendering loop to support textures. |
| `CMakeLists.txt` | **MODIFY** | Add `src/renderer/texture.cpp` to `hse_core` library. |

### 2. Detailed Symbol Architecture

#### `class Texture` [PLANNED]
- **Namespace**: `hse`
- **Fields**:
  - `unsigned int m_rendererID`: OpenGL texture ID. [PROVEN: OpenGL Spec]
  - `int m_width, m_height, m_channels`: Metadata from image file.
- **Methods**:
  - `Texture(const std::string& path)`: Constructor for RAII loading. [PROVEN: C++ Core Guidelines R.1]
  - `~Texture()`: Destructor calling `glDeleteTextures`. [PROVEN: RAII]
  - `void bind(unsigned int slot = 0) const`: Wrapper for `glBindTexture`.
  - `void unbind() const`: Wrapper for `glBindTexture(GL_TEXTURE_2D, 0)`.

#### `class Primitive` [MODIFY]
- **New Fields**:
  - `std::shared_ptr<Texture> m_texture`: Optional texture asset. [INFERRED: Asset sharing]
  - `bool m_hasTexture`: Flag for shader logic.
- **New Methods**:
  - `void setTexture(std::shared_ptr<Texture> texture)`: Setter for texture asset.

#### `Renderer::RendererState` [MODIFY]
- **New Fields**:
  - `int uHasTextureLoc`: Cached uniform location for `uHasTexture`.
  - `int uTextureLoc`: Cached uniform location for `uTexture`.

### 3. Data Flow & Vertex Layout
- **Vertex Layout**: Interleaved `vec3 Pos, vec2 UV`.
- **Stride**: `5 * sizeof(float)`. [PROVEN: docs.gl]
- **Attributes**:
  - `location 0`: Position (offset 0)
  - `location 1`: UV (offset 12)
- **Shader Pipeline**:
  - Vertex shader passes `aUV` to Fragment shader as `vUV`.
  - Fragment shader samples from `uTexture` if `uHasTexture` is true.

### 4. Resource Ownership
- **Strategy**: **RAII with Shared Ownership**.
- **Details**: `Texture` class owns the GL handle. `Primitive` holds a `shared_ptr` to allow multiple primitives to use the same texture without redundant loads or double-frees.

### 5. Classification
- **PROVEN**: OpenGL API behavior, C++ RAII rules, Stride/Offset logic for VBOs.
- **INFERRED**: Use of `std::shared_ptr` for assets, uniform caching strategy.
- **PLANNED**: Specific method names (`bind`, `setTexture`) and field integration.
