# HSE-013 Phase 2: Repository Archaeology

## Material System Files (Working Tree)
| File | Status | Lines | Role |
|------|--------|-------|------|
| `include/renderer/material.h` | NEW (untracked) | 39 | Material class declaration |
| `src/renderer/material.cpp` | NEW (untracked) | 28 | Material::apply() implementation |
| `src/material_demo.cpp` | NEW (untracked) | 102 | Demo: 4 primitives, 3 materials |
| `include/scene/primitive.h` | MODIFIED | 81 | Added m_material, setMaterial/getMaterial |
| `include/scene/scene.h` | MODIFIED | 56 | Added m_materials map, addMaterial/getMaterial |
| `src/renderer/renderer.cpp` | MODIFIED | 215 | GLSL Material struct, material->apply() |
| `src/scene/scene.cpp` | MODIFIED | 101 | addMaterial/getMaterial implementations |
| `src/scene/scene_builder.cpp` | MODIFIED | 237 | Material parsing + serialization |
| `CMakeLists.txt` | MODIFIED | ~90 | hse_material_demo target |

## Symbol Ground Truth (Working Tree State)

### Classes
| Class | File | Fields | Methods |
|-------|------|--------|---------|
| Material | include/renderer/material.h | m_name, m_albedo, m_albedoMap, m_roughness, m_metallic | getName, setAlbedo/getAlbedo, setAlbedoMap/getAlbedoMap/hasAlbedoMap, setRoughness/getRoughness, setMetallic/getMetallic, apply |
| Primitive | include/scene/primitive.h | m_id, m_name, m_type, m_position, m_rotation, m_scale, m_rotationSpeed, m_material, m_vertices, m_indices, m_vao, m_vbo, m_ebo, m_uploaded | (25 methods) setMaterial/getMaterial added |
| Scene | include/scene/scene.h | m_name, m_primitives, m_idIndex, m_cameras, m_activeCameraIndex, m_materials | addMaterial/getMaterial/getMaterials added |
| Renderer | include/renderer/renderer.h | m_state, m_frameCount | (11 methods) |

### GLSL Struct (in renderer.cpp)
```glsl
struct Material {
    vec3 albedo;
    float roughness;
    float metallic;
    bool hasAlbedoMap;
    sampler2D albedoMap;
};
uniform Material uMaterial;
```

### Material::apply() Flow
1. glGetUniformLocation(shaderProgram, "uMaterial.albedo") → glUniform3f
2. glGetUniformLocation(shaderProgram, "uMaterial.roughness") → glUniform1f
3. glGetUniformLocation(shaderProgram, "uMaterial.metallic") → glUniform1f
4. if hasAlbedoMap: Texture::bind(0), glUniform1i("uMaterial.hasAlbedoMap", 1), glUniform1i("uMaterial.albedoMap", 0)
5. else: glUniform1i("uMaterial.hasAlbedoMap", 0)

### Renderer::renderScene() Flow
1. glUseProgram(shaderProgram)
2. Set view + projection uniforms
3. For each primitive: compute model matrix, set material uniforms via material->apply(), bind VAO, glDrawElements, unbind

### SceneBuilder Material Flow
- buildRoom(): creates mat_floor, mat_wall, mat_ceiling with albedo + roughness, adds to scene
- importState(): parses materials array from JSON, creates Material objects, loads albedo_map textures
- exportHSC(): serializes materials with id, albedo, roughness, metallic, albedo_map

## Architecture Observations
1. Material is a value-like class with shared_ptr ownership — lightweight, sharable across primitives
2. Material::apply() uses hardcoded uniform locations — works but brittle against shader changes
3. Renderer owns a single shader program — all materials use same shader (limitation, not bug)
4. Fragment shader has basic albedo * texture output — no lighting (limitation for PBR)
5. No material caching or deduplication in renderer — each apply() re-uploads all uniforms
6. Scene stores materials by name — allows lookup by string, shared_ptr prevents duplication
7. Sphere geometry is stub (1 vertex, 1 index) — not functional but not in scope
