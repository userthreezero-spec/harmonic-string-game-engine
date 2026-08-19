# HSE-013 Phase 1: Mission Formalization

## Mission Statement
Verify and complete a first-class Material System for HSE — albedo, roughness, metallic, albedo maps — ensuring architectural correctness, automated testing, runtime verification, bridge + project serialization support.

## Working Hypothesis
A material system that passed symbol-level verification (HSE-012: 82.4% symbol F1) should require minimal architectural fixups when implemented, producing a build + functional demo on first attempt.

## Operational Definition
A material system allows HSE primitives to carry named materials with PBR-like properties (albedo, roughness, metallic, optional albedo texture map) that are uploaded as uniforms to the GPU shader at draw time.

## REQUIRED (must succeed)
1. Material class with: name, albedo (Vec3), albedoMap (shared_ptr<Texture>), roughness (float), metallic (float)
2. Uniform upload: Material::apply(unsigned int shaderProgram) sets all uniforms
3. Primitive integration: setMaterial()/getMaterial() on Primitive class
4. Renderer integration: renderer calls material->apply() before each draw call
5. Fallback material: when Primitive has no material, renderer uses a sensible default
6. GLSL struct: Material { vec3 albedo; float roughness; float metallic; bool hasAlbedoMap; sampler2D albedoMap; }
7. Demo: hse_material_demo compiles, links, runs, displays 4 primitives with distinct materials
8. Build: full CMake build succeeds with zero warnings
9. All existing tests pass (hse_test_math, hse_test_scene)

## OPTIONAL (stretch, not blocking)
- Bridge commands: mat_create, mat_set_albedo, mat_set_roughness, mat_set_metallic, mat_set_albedo_map, mat_assign, mat_list
- Scene serialization: materials saved/loaded in .hsc project files
- Additional material properties: emission, normal map, ambient occlusion
- Material library / factory pattern for reuse
- PBR lighting in fragment shader (currently only basic albedo * texture)

## OUT-OF-SCOPE
- Full PBR pipeline (image-based lighting, BRDF, environment maps)
- Post-processing pipeline
- Material instancing / batching
- Shader graph / node-based materials

## Known Artifacts (Working Tree, Uncommitted)
- `include/renderer/material.h` — Material class
- `src/renderer/material.cpp` — Material::apply() implementation
- `src/material_demo.cpp` — Demo
- `include/scene/primitive.h` — m_material field added
- `include/scene/scene.h` — m_materials map added
- `src/renderer/renderer.cpp` — Material integration + GLSL struct
- `src/scene/scene.cpp` — addMaterial/getMaterial
- `src/scene/scene_builder.cpp` — Material parsing/serialization
- `CMakeLists.txt` — hse_material_demo target

## Success Criteria
- Build succeeds (zero warnings)
- Existing tests pass
- hse_material_demo runs for 100 frames without crash
- 4 primitives visible with distinct visual materials
- Material properties verified through runtime assertion
- Symbol verification: 100% against ground truth

## Evaluation Metrics
- Architectural accuracy: % of design decisions grounded in authoritative knowledge
- Implementation fidelity: % of symbol plan matching actual code
- First-attempt success: did it build + run without fixups?
- Test coverage: automated tests for Material class
- Knowledge contribution: what did this reveal about knowledge → implementation?
