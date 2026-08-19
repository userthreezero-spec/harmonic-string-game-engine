# HSE-013 Phase 4: Architecture

## Material System Architecture (Working Tree - VERIFIED CORRECT)

### Component Diagram
```
Material (class)
├── Fields: m_name, m_albedo, m_albedoMap, m_roughness, m_metallic
├── Apply: uploadUniforms(shaderProgram) → GL calls
└── Ownership: shared_ptr (multiple Primitives can share same Material)

Primitive (class)
├── Field: m_material (shared_ptr<Material>, nullable)
├── Setter: setMaterial(shared_ptr<Material>)
├── Getter: getMaterial() → shared_ptr<Material>
└── Responsibility: stores geometry + material assignment

Scene (class)
├── Field: m_materials (unordered_map<string, shared_ptr<Material>>)
├── Management: addMaterial(), getMaterial(), getMaterials()
└── Responsibility: owns materials, provides lookup by name

Renderer (class)
├── Shader: single GLSL program with Material struct
├── Render loop: per-primitive material->apply(shaderProgram)
├── Fallback: static defaultMaterial when primitive has none
└── Responsibility: GPU rendering pipeline

SceneBuilder (class)
├── buildRoom(): creates 3 materials (floor, wall, ceiling)
├── importState(): parses materials from JSON
├── exportHSC(): serializes materials to JSON
└── Responsibility: scene construction + persistence
```

### Data Flow
```
SceneBuilder::buildRoom()
  → creates Material("mat_floor") with albedo + roughness
  → scene->addMaterial(matFloor)
  → Primitive::setMaterial(matFloor)
  
Renderer::renderScene(scene, camera)
  → glUseProgram(shaderProgram)
  → for each primitive:
      → compute model matrix
      → prim->getMaterial() → Material*
      → material->apply(shaderProgram) → glUniform calls
      → prim->bind() → glBindVertexArray
      → glDrawElements(...)
```

### GLSL Material Struct
```glsl
struct Material {
    vec3 albedo;           // uniform color
    float roughness;       // PBR roughness [0,1]
    float metallic;        // PBR metallic [0,1]
    bool hasAlbedoMap;     // texture presence flag
    sampler2D albedoMap;   // texture unit
};
uniform Material uMaterial;
```

### Key Design Decisions
1. **Shared ownership**: Material shared across primitives via shared_ptr (memory efficient)
2. **Per-primitive apply**: Each primitive uploads its own material uniforms (simple, correct)
3. **Single shader**: All materials use same shader (engine limitation, acceptable for v0.1)
4. **Name-based lookup**: Scene stores materials by name for JSON serialization
5. **Fallback material**: Renderer creates default "Default" material if primitive has none

### Architecture Assessment
- **Completeness**: All REQUIRED components present
- **Correctness**: Symbol-level implementation matches ground truth
- **Integration**: Primitive, Renderer, Scene, SceneBuilder all properly wired
- **Limitations acknowledged**: Single shader, hardcoded uniform names, no batching
