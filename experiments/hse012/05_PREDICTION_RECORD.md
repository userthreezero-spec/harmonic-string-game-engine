# HSE-012 — Prediction Record

## Expected Changes

### Files Changed
- `include/scene/primitive.h`
- `src/scene/primitive.cpp`
- `include/renderer/renderer.h`
- `src/renderer/renderer.cpp`
- `CMakeLists.txt`

### New Files
- `include/renderer/texture.h`
- `src/renderer/texture.cpp`
- `include/stb_image.h` (copied from local source)

### Expected Symbol Accuracy
- Predicted: `Texture`, `Primitive::setTexture`, `Primitive::m_texture`, `RendererState::uHasTextureLoc`, `RendererState::uTextureLoc`.
- Target Accuracy: > 90%.

### Expected Runtime Behavior
- Primitives without textures will still render with their base color.
- Primitives with textures will show the mapped image.
- Memory usage will increase by texture size + slightly larger VBOs.

### Expected Failure Modes
- Image file not found (handled by `stb_image` returning null).
- UV coordinates incorrectly mapped (visual stretching/warping).
- Shader compilation failure due to GLSL syntax errors in the modified strings.
- Z-fighting if textures are applied to overlapping faces.
