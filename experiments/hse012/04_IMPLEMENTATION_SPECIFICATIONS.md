# HSE-012 — Implementation Specifications

## Mission: TEXTURE MAPPING

### 1. New Component: `hse::Texture`
- **File**: `include/renderer/texture.h` / `src/renderer/texture.cpp`
- **Current Behavior**: N/A
- **Desired Behavior**: 
  - Load image using `stb_image`.
  - Create GL texture object with `glGenTextures`.
  - Set parameters: `GL_TEXTURE_WRAP_S/T = GL_REPEAT`, `GL_TEXTURE_MIN/MAG_FILTER = GL_LINEAR`.
  - Upload data using `glTexImage2D`.
  - Cleanup using `glDeleteTextures` in destructor.
- **Ownership**: Owns the OpenGL handle (`m_rendererID`).
- **Dependencies**: `GLEW`, `stb_image`.
- **Acceptance Criteria**: `m_rendererID > 0` after valid load; image dimensions correctly parsed.

### 2. Modify: `hse::Primitive`
- **File**: `include/scene/primitive.h` / `src/scene/primitive.cpp`
- **Symbols**: 
  - Add `std::shared_ptr<Texture> m_texture`.
  - Add `void setTexture(std::shared_ptr<Texture> texture)`.
- **Changes**:
  - `generateGeometry()`: Update to include UV coordinates (5 floats per vertex).
  - `uploadGPU()`: 
    - Update `glVertexAttribPointer` for `location 0` (stride 5*float).
    - Add `glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)))`.
    - `glEnableVertexAttribArray(1)`.
- **Data Flow**: `Texture` passed in by user/builder.
- **Acceptance Criteria**: VBO size increased by 66% (3 -> 5 floats per vertex); attribute 1 active.

### 3. Modify: `hse::Renderer`
- **File**: `src/renderer/renderer.cpp`
- **Symbols**:
  - Update `vertexShaderSource` to accept `layout (location = 1) in vec2 aUV`.
  - Update `fragmentShaderSource` to include `uniform sampler2D uTexture` and `uniform bool uHasTexture`.
- **Desired Behavior**:
  - During `renderScene()`, iterate through primitives.
  - If `primitive.getTexture()` exists:
    - `texture->bind(0)`.
    - Set `uHasTexture = true`.
  - Else:
    - Set `uHasTexture = false`.
  - Call `glDrawElements`.
- **Acceptance Criteria**: Shader compiles without error; uniforms correctly located.

### 4. Build System
- **File**: `CMakeLists.txt`
- **Change**: Add `src/renderer/texture.cpp` to `hse_core`.
- **Acceptance Criteria**: `hse_main` and `hse_room_demo` build successfully.

### 5. Runtime Verification
- **Method**: `capture_frame` and `observe_object`.
- **Requirement**: A textured quad/cube rendered on screen with distinct colors/patterns from the texture file.
