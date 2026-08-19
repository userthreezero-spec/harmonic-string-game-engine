# HSE-019 — Symbol Plan

## Proposed Changes

### Subsystem: Scene (Lighting & Normals)

#### [NEW] [light.h](file:///C:/Users/Rico/HarmonicStringEngine/include/scene/light.h)
- `hse::LightType` (enum)
- `hse::Light` (class)
- `hse::Light::setPosition(Vec3)`
- `hse::Light::setColor(Vec3)`
- `hse::Light::setIntensity(float)`

#### [MODIFY] [scene.h](file:///C:/Users/Rico/HarmonicStringEngine/include/scene/scene.h)
- `hse::Scene::addLight(std::shared_ptr<Light>)`
- `hse::Scene::getLights() const`
- `std::vector<std::shared_ptr<Light>> m_lights`

#### [MODIFY] [primitive.h](file:///C:/Users/Rico/HarmonicStringEngine/include/scene/primitive.h)
- Update vertex layout to include normals.
- `std::vector<float> m_vertices` (now 8 floats per vertex: pos[3], uv[2], normal[3])

#### [MODIFY] [primitive.cpp](file:///C:/Users/Rico/HarmonicStringEngine/src/scene/primitive.cpp)
- `generateGeometry()`: Add normal vectors for Triangle, Quad, Cube.
- `uploadGPU()`: Update `glVertexAttribPointer` for normal attribute (location 2).

### Subsystem: Renderer (Shader Logic)

#### [MODIFY] [renderer.cpp](file:///C:/Users/Rico/HarmonicStringEngine/src/renderer/renderer.cpp)
- `vertexShaderSource`: Add `layout (location = 2) in vec3 aNormal`, pass `vNormal` to fragment.
- `fragmentShaderSource`: Add `uLightPos`, `uLightColor`, implement diffuse (Lambert) and specular.
- `renderScene()`: Bind light uniforms.

### Subsystem: Persistence

#### [MODIFY] [scene_builder.cpp](file:///C:/Users/Rico/HarmonicStringEngine/src/scene/scene_builder.cpp)
- `exportHSC()`: Add `rotation` and `scale` to JSON output.
- `importState()`: Ensure `rotation` and `scale` are correctly applied.

## Verification Gate

### Predicted Symbols
- `hse::Light`
- `hse::Scene::addLight`
- `hse::Renderer::renderScene` (modified)
- `hse::Primitive::generateGeometry` (modified)

### Build Inclusion
- All files included in `CMakeLists.txt` via `hse_core` target.
