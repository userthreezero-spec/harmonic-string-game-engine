# HSE-011: Blind Evaluation Missions

> **Purpose**: Test whether the knowledge corpus enables correct architectural
> proposals for 10 features NOT explicitly documented in HSE source code.
> Each mission requires applying learned principles to HSE's real architecture.

---

## Ground Truth Reminder

| Category | Count |
|----------|-------|
| Source files | 27 |
| Classes | 10 (Primitive, Renderer, ShaderProgram, TextureLoader, CubemapLoader, SceneObject, WindowManager, InputManager, Camera, Mesh) |
| Structs | 8 |
| Enums | 3 |
| Functions | 125 |
| Vendors | GLFW, GLEW, GLM |
| Graphics API | OpenGL 3.3 |
| IPC | Named pipe bridge |

---

## Mission 1: Add Texture Mapping to Existing Primitives

### MISSION
Enable UV-textured rendering on the existing Primitive class without breaking
the current vertex-color pipeline.

### ARCHITECTURE PROPOSAL
- **Subsystem**: Renderer
- **Module**: `renderer.cpp` / `renderer.h`
- **Class**: Primitive
- **Data structures**: UV coordinate attribute in vertex layout

### EXACT SYMBOLS

| Symbol | Status | Notes |
|--------|--------|-------|
| `Primitive` | REAL | 15 fields, 25 methods |
| `Renderer::draw()` | REAL | Existing draw call |
| `ShaderProgram` | REAL | 10 functions |
| `TextureLoader` | REAL | 7 functions |
| `m_vao` | REAL | Primitive owns its VAO |
| `m_vertices` | REAL | Vertex data vector |
| `m_uvs` | **NEW** | New UV coordinate vector |
| `m_texture_id` | **NEW** | Texture handle on Primitive |
| `TextureLoader::load_texture()` | REAL | Existing loader |

### DEPENDENCIES
- TextureLoader (already exists, 7 functions)
- ShaderProgram (must add `sampler2D` uniform support)
- Primitive (must extend vertex layout)

### IMPLEMENTATION ORDER
1. Add `m_uvs` and `m_texture_id` fields to Primitive
2. Extend VAO binding in Primitive to include UV attribute (location = 2)
3. Add `sampler2D` uniform binding to ShaderProgram
4. Extend TextureLoader to support 2D texture creation (if not already)
5. Bind texture in Renderer::draw() before draw call
6. Update fragment shader to mix vertex color and texture

### ACCEPTANCE CRITERIA
- Primitive can hold UV data and a texture ID
- Existing vertex-color rendering still works (backward compatible)
- New textured primitives render with correct UV mapping
- TextureLoader::load_texture() returns a valid GL texture ID
- No new classes introduced (only extended existing ones)

### VERIFICATION PLAN
- Check Primitive fields against `architecture_verifier/verify.py` output
- Confirm `m_uvs` and `m_texture_id` are NEW additions
- Confirm `m_vao` remains on Primitive (not moved to Mesh)
- Verify TextureLoader functions match real 7-function count
- Run draw test: vertex-color-only primitive renders unchanged

---

## Mission 2: Add Material Abstraction Layer

### MISSION
Introduce a lightweight Material struct that bundles texture, color, and shader
state so Primitives don't carry rendering configuration directly.

### ARCHITECTURE PROPOSAL
- **Subsystem**: Renderer
- **Module**: New header `material.h` (struct only, no .cpp)
- **Struct**: Material
- **Class**: Primitive gains a Material pointer

### EXACT SYMBOLS

| Symbol | Status | Notes |
|--------|--------|-------|
| `Primitive` | REAL | Existing class |
| `Material` | **NEW** | New struct |
| `Material::diffuse_texture` | **NEW** | Texture handle |
| `Material::specular_color` | **NEW** | vec3 color |
| `Material::shininess` | **NEW** | float |
| `ShaderProgram` | REAL | 10 functions |
| `TextureLoader` | REAL | 7 functions |
| `Primitive::m_material` | **NEW** | Optional material pointer |
| `Renderer::draw()` | REAL | Must apply material before draw |

### DEPENDENCIES
- ShaderProgram (must support material uniform blocks)
- TextureLoader (material references loaded textures)
- Primitive (gains optional material)

### IMPLEMENTATION ORDER
1. Define Material struct in `material.h`
2. Add `m_material` field to Primitive (default = nullptr = vertex-color mode)
3. Extend ShaderProgram to accept material uniform block
4. In Renderer::draw(), if primitive has material, bind it; else use vertex colors
5. TextureLoader provides texture IDs that Material references

### ACCEPTANCE CRITERIA
- Material is a plain struct (no inheritance, no virtual methods)
- Primitive with nullptr material renders in vertex-color mode (backward compat)
- Primitive with material applies texture + color + shininess
- No rendering state leaks between primitives with different materials
- ShaderProgram uniform binding count stays within OpenGL 3.3 limits

### VERIFICATION PLAN
- Verify Material is NEW (not in existing 8 structs)
- Verify Primitive field count increases from 15 to 16
- Confirm Renderer::draw() is the single point where material is applied
- Check that no new classes were created (only struct + field addition)

---

## Mission 3: Add Mesh Resource Management with Shared Buffers

### MISSION
Allow multiple Primitives to share the same Mesh data (vertices, normals, UVs)
via reference-counted GPU buffers, reducing memory duplication.

### ARCHITECTURE PROPOSAL
- **Subsystem**: Renderer
- **Class**: Mesh (existing, 4 functions)
- **Class**: Primitive (references Mesh)
- **Pattern**: Shared ownership via `std::shared_ptr<Mesh>`

### EXACT SYMBOLS

| Symbol | Status | Notes |
|--------|--------|-------|
| `Mesh` | REAL | 4 functions |
| `Primitive` | REAL | 15 fields, 25 methods |
| `m_vao` | REAL | Currently on Primitive |
| `m_vbo` | REAL | Currently on Primitive |
| `Mesh::m_vertices` | **NEW** | Shared vertex data |
| `Mesh::m_vao` | **NEW** | Mesh takes ownership of VAO |
| `Mesh::m_vbo` | **NEW** | Mesh takes ownership of VBO |
| `Primitive::m_mesh` | **NEW** | Shared pointer to Mesh |
| `std::shared_ptr<Mesh>` | NEW | C++ smart pointer |

### DEPENDENCIES
- Mesh (must be extended from 4 to ~7 functions)
- Primitive (m_vao/m_vbo migrate to Mesh)
- Renderer (must bind Mesh VAO instead of Primitive VAO)

### IMPLEMENTATION ORDER
1. Extend Mesh with VAO/VBO ownership and vertex data storage
2. Add `m_mesh` shared pointer to Primitive
3. Migrate VAO/VBO creation from Primitive constructor to Mesh
4. Primitive constructor creates or references a Mesh
5. Renderer::draw() binds `primitive->m_mesh->m_vao` instead of `primitive->m_vao`
6. Shared primitives share the same Mesh instance

### ACCEPTANCE CRITERIA
- Two Primitives can share one Mesh and render identically
- When last Primitive referencing a Mesh is destroyed, GPU resources are freed
- Primitive field count stays at 15 (m_vao/m_vbo replaced by m_mesh)
- Renderer draw path unchanged except VAO source
- No double-free of GPU resources

### VERIFICATION PLAN
- Check Primitive field list: m_vao and m_vbo should be gone, m_mesh added
- Confirm Mesh function count increases from 4 to ~7
- Verify Renderer references Mesh VAO not Primitive VAO
- Run shared-mesh test: two primitives, one mesh, correct rendering
- Confirm GPU resource cleanup on last reference drop

---

## Mission 4: Add Orbit Camera Controls

### MISSION
Add orbit camera behavior (rotate around target, zoom, pan) to the existing
Camera class without breaking the current free-look mode.

### ARCHITECTURE PROPOSAL
- **Subsystem**: Input + Camera
- **Class**: Camera (existing, 8 functions)
- **Class**: InputManager (existing, 6 functions)
- **Struct**: OrbitState (NEW)

### EXACT SYMBOLS

| Symbol | Status | Notes |
|--------|--------|-------|
| `Camera` | REAL | 8 functions |
| `InputManager` | REAL | 6 functions |
| `WindowManager` | REAL | 5 functions |
| `Camera::m_position` | REAL | vec3 position |
| `Camera::m_front` | REAL | vec3 look direction |
| `Camera::m_up` | REAL | vec3 up vector |
| `Camera::update()` | REAL | Per-frame update |
| `OrbitState` | **NEW** | Orbit parameters struct |
| `OrbitState::m_target` | **NEW** | vec3 orbit center |
| `OrbitState::m_distance` | **NEW** | float radius |
| `OrbitState::m_yaw` | **NEW** | float horizontal angle |
| `OrbitState::m_pitch` | **NEW** | float vertical angle |
| `Camera::m_mode` | **NEW** | Enum: FREE_LOOK / ORBIT |

### DEPENDENCIES
- Camera (extend with orbit mode)
- InputManager (mouse delta → orbit rotation)
- WindowManager (scroll callback → zoom)

### IMPLEMENTATION ORDER
1. Define CameraMode enum (FREE_LOOK, ORBIT)
2. Define OrbitState struct
3. Add m_mode and m_orbit_state to Camera
4. Add Camera::set_orbit_target() method
5. Extend Camera::update() to compute position from orbit parameters when in ORBIT mode
6. Wire InputManager mouse delta to orbit yaw/pitch when mode is ORBIT
7. Wire WindowManager scroll callback to orbit distance

### ACCEPTANCE CRITERIA
- Camera defaults to FREE_LOOK (backward compatible)
- ORBIT mode computes position from target + distance + angles
- Mouse drag rotates orbit, scroll zooms
- No change to existing free-look behavior
- Camera function count increases from 8 to ~11

### VERIFICATION PLAN
- Verify Camera still has m_position, m_front, m_up (unchanged)
- Confirm new m_mode and m_orbit_state are NEW additions
- Check that InputManager function count stays at 6 (wiring only)
- Verify orbit math: position = target + spherical_to_cartesian(distance, yaw, pitch)
- Run backward-compat test: free-look camera unchanged

---

## Mission 5: Add Transform Hierarchy (Parent-Child)

### MISSION
Enable SceneObjects to form parent-child hierarchies where child transforms
are relative to their parent, propagating world transforms down the tree.

### ARCHITECTURE PROPOSAL
- **Subsystem**: Scene
- **Class**: SceneObject (existing, 10 fields, 6 methods)
- **Struct**: Transform (NEW or extended)

### EXACT SYMBOLS

| Symbol | Status | Notes |
|--------|--------|-------|
| `SceneObject` | REAL | 10 fields, 6 methods |
| `SceneObject::m_position` | REAL | Local position |
| `SceneObject::m_rotation` | REAL | Local rotation |
| `SceneObject::m_scale` | REAL | Local scale |
| `SceneObject::m_children` | **NEW** | Vector of child pointers |
| `SceneObject::m_parent` | **NEW** | Weak pointer to parent |
| `SceneObject::get_world_transform()` | **NEW** | Computes full chain |
| `SceneObject::add_child()` | **NEW** | Establishes hierarchy |
| `glm::mat4` | REAL | GLM matrix type |

### DEPENDENCIES
- SceneObject (extend with hierarchy)
- GLM (glm::mat4 for transform composition)
- Renderer (must use world transform, not local)

### IMPLEMENTATION ORDER
1. Add m_parent (weak_ptr) and m_children (vector) to SceneObject
2. Add add_child() and remove_child() methods
3. Add get_world_transform() that walks up parent chain
4. Update Renderer to use get_world_transform() instead of raw m_position
5. Handle root objects (m_parent = nullptr → world = local)

### ACCEPTANCE CRITERIA
- SceneObject field count increases from 10 to 12
- Child object's world transform = parent chain composition
- Removing a parent doesn't crash children (they become roots)
- Cyclic parent references are prevented
- Renderer correctly renders nested hierarchies
- Root objects (no parent) behave exactly as before

### VERIFICATION PLAN
- Check SceneObject field list for m_parent and m_children (NEW)
- Verify get_world_transform() walks full parent chain
- Confirm Renderer::draw() uses world transform
- Run hierarchy test: parent at (1,0,0), child at (0,1,0) → child world at (1,1,0)
- Run detach test: remove parent, child renders at local origin

---

## Mission 6: Add Scene Serialization to JSON

### MISSION
Serialize the current scene graph (all SceneObjects, their transforms, and
Primitive assignments) to a JSON file and deserialize it back.

### ARCHITECTURE PROPOSAL
- **Subsystem**: Scene
- **Module**: New `scene_serializer.cpp` / `scene_serializer.h`
- **Class**: SceneSerializer
- **External**: nlohmann/json or manual JSON writer

### EXACT SYMBOLS

| Symbol | Status | Notes |
|--------|--------|-------|
| `SceneObject` | REAL | 10 fields, 6 methods |
| `Primitive` | REAL | 15 fields, 25 methods |
| `Renderer` | REAL | 18 functions |
| `SceneSerializer` | **NEW** | New class |
| `SceneSerializer::serialize()` | **NEW** | Scene → JSON string |
| `SceneSerializer::deserialize()` | **NEW** | JSON string → Scene |
| `glm::vec3` | REAL | Position/rotation/scale |

### DEPENDENCIES
- SceneObject (must expose its fields for serialization)
- Primitive (must expose type + transform)
- JSON library (must be added as dependency)
- Camera (should serialize camera state too)

### IMPLEMENTATION ORDER
1. Add JSON library dependency (nlohmann/json via header-only)
2. Define SceneSerializer class
3. Implement serialize(): walk scene tree, emit JSON
4. Implement deserialize(): parse JSON, reconstruct SceneObjects + Primitives
5. Handle nested hierarchies (from Mission 5)
6. Handle material references (from Mission 2)

### ACCEPTANCE CRITERIA
- Serialize then deserialize produces identical scene
- JSON is human-readable and valid
- Handles empty scenes, single objects, deep hierarchies
- New class doesn't modify existing classes
- SceneSerializer has ~4 methods (serialize, deserialize, save, load)

### VERIFICATION PLAN
- Round-trip test: serialize → deserialize → serialize → compare JSON
- Visual test: deserialized scene renders identically
- Edge cases: empty scene, 1000 objects, 10-level hierarchy
- Verify SceneSerializer is NEW (not in existing 10 classes)
- Confirm JSON output matches expected schema

---

## Mission 7: Add Shader Abstraction (Material System)

### MISSION
Replace direct shader uniform calls with a ShaderMaterial abstraction that
automatically binds all uniforms from a material definition, reducing
boilerplate in Renderer::draw().

### ARCHITECTURE PROPOSAL
- **Subsystem**: Renderer
- **Class**: ShaderProgram (existing, 10 functions)
- **Struct**: ShaderMaterial (NEW)
- **Pattern**: Uniform auto-binding via reflection-like material definition

### EXACT SYMBOLS

| Symbol | Status | Notes |
|--------|--------|-------|
| `ShaderProgram` | REAL | 10 functions |
| `ShaderProgram::use()` | REAL | Activate shader |
| `ShaderProgram::set_uniform()` | REAL | Set individual uniform |
| `ShaderMaterial` | **NEW** | Material definition struct |
| `ShaderMaterial::bind()` | **NEW** | Auto-bind all uniforms |
| `ShaderMaterial::m_shader` | **NEW** | Reference to ShaderProgram |
| `ShaderMaterial::m_textures` | **NEW** | Named texture slots |
| `ShaderMaterial::m_floats` | **NEW** | Named float uniforms |
| `ShaderMaterial::m_vec3s` | **NEW** | Named vec3 uniforms |
| `Renderer::draw()` | REAL | Uses ShaderMaterial instead of raw uniforms |

### DEPENDENCIES
- ShaderProgram (extend with batch uniform setting)
- TextureLoader (textures referenced by ShaderMaterial)
- Renderer (simplify draw path)

### IMPLEMENTATION ORDER
1. Define ShaderMaterial struct with typed uniform maps
2. Add ShaderMaterial::bind() that iterates maps and calls set_uniform()
3. Extend ShaderProgram with set_uniform overload for batch setting
4. Update Renderer::draw() to use ShaderMaterial::bind()
5. Remove individual set_uniform calls from Renderer::draw()

### ACCEPTANCE CRITERIA
- ShaderMaterial encapsulates all per-draw uniform state
- Renderer::draw() becomes simpler (no manual uniform calls)
- Adding new uniform types doesn't require changing Renderer
- Backward compatible: raw set_uniform() still works
- ShaderProgram function count stays at 10 (bind is on ShaderMaterial)

### VERIFICATION PLAN
- Verify Renderer::draw() has fewer uniform-setting lines
- Confirm ShaderMaterial is NEW struct
- Check ShaderProgram unchanged at 10 functions
- Visual test: rendering output identical before and after
- Performance test: no measurable regression

---

## Mission 8: Add GPU Resource Lifetime Management (RAII)

### MISSION
Wrap all OpenGL resource handles (VAO, VBO, texture, FBO, shader) in RAII
wrappers that automatically call glDelete* on destruction, preventing leaks.

### ARCHITECTURE PROPOSAL
- **Subsystem**: Renderer (cross-cutting)
- **Module**: New `gpu_resource.h` / `gpu_resource.cpp`
- **Classes**: VAOHandle, VBOHandle, TextureHandle, ShaderHandle (NEW)
- **Pattern**: RAII with move semantics, no copy

### EXACT SYMBOLS

| Symbol | Status | Notes |
|--------|--------|-------|
| `Primitive` | REAL | Owns VAO, VBO |
| `ShaderProgram` | REAL | Owns shader program |
| `TextureLoader` | REAL | Creates textures |
| `VAOHandle` | **NEW** | RAII wrapper for GLuint VAO |
| `VBOHandle` | **NEW** | RAII wrapper for GLuint VBO |
| `TextureHandle` | **NEW** | RAII wrapper for GLuint texture |
| `ShaderHandle` | **NEW** | RAII wrapper for GLuint program |

### DEPENDENCIES
- Primitive (migrates VAO/VBO to handles)
- ShaderProgram (migrates program ID to handle)
- TextureLoader (returns handles instead of raw IDs)

### IMPLEMENTATION ORDER
1. Define RAII handle classes with constructor (gen), destructor (delete), move ops
2. Replace Primitive's raw GLuint m_vao/m_vbo with VAOHandle/VBOHandle
3. Replace ShaderProgram's raw program ID with ShaderHandle
4. Update TextureLoader to return TextureHandle
5. Add move-only semantics (delete copy constructor/assignment)
6. Audit all remaining raw GL resource creation for handle conversion

### ACCEPTANCE CRITERIA
- No raw GLuint resource handles remain in class fields
- All GPU resources are freed when owning object is destroyed
- Handles are move-only (no accidental copies)
- Compile-time error if copy is attempted
- Zero GPU resource leaks on shutdown
- Existing rendering behavior unchanged

### VERIFICATION PLAN
- Static analysis: grep for raw GLuint in class fields → should be zero
- Run shutdown test: create and destroy objects, check for GL errors
- Valgrind/RenderDoc: no leaked resources on exit
- Compile test: copy attempt → compile error
- Functional test: rendering output identical

---

## Mission 9: Add Object Picking (Mouse → Scene Object)

### MISSION
Enable clicking on a rendered primitive to identify which SceneObject was hit,
using color-based picking (render each object with unique ID color, read back
pixel under mouse).

### ARCHITECTURE PROPOSAL
- **Subsystem**: Renderer + Input
- **Module**: `picking.cpp` / `picking.h`
- **Class**: PickingPass
- **Pattern**: Offscreen render with object ID → readback

### EXACT SYMBOLS

| Symbol | Status | Notes |
|--------|--------|-------|
| `Renderer` | REAL | 18 functions |
| `InputManager` | REAL | 6 functions |
| `SceneObject` | REAL | 10 fields, 6 methods |
| `Primitive` | REAL | 15 fields, 25 methods |
| `PickingPass` | **NEW** | Offscreen ID rendering |
| `PickingPass::m_fbo` | **NEW** | Framebuffer for ID pass |
| `PickingPass::m_id_texture` | **NEW** | Color attachment for IDs |
| `PickingPass::render()` | **NEW** | Render scene with IDs |
| `PickingPass::pick()` | **NEW** | Read pixel → SceneObject |
| `Primitive::m_pick_id` | **NEW** | Unique integer ID |
| `InputManager::get_mouse_pos()` | REAL | Mouse position |

### DEPENDENCIES
- Renderer (must add picking pass before/after main pass)
- InputManager (mouse click + position)
- SceneObject (lookup by ID)
- ShaderProgram (picking shader)

### IMPLEMENTATION ORDER
1. Create PickingPass class with FBO + ID texture
2. Assign unique m_pick_id to each Primitive on creation
3. Write picking shader: outputs object ID as color
4. Add picking render pass to Renderer (before main render)
5. On mouse click: run picking pass, read pixel at mouse pos
6. Map pixel color → pick_id → SceneObject
7. Return picked SceneObject to caller

### ACCEPTANCE CRITERIA
- Clicking on a rendered object returns the correct SceneObject
- Picking pass doesn't affect main render output
- Picking is O(1) per click (single readback)
- Up to 16M unique objects (24-bit color ID)
- No visual artifacts from picking pass

### VERIFICATION PLAN
- Unit test: render 3 objects at known positions, click each → correct ID
- Performance test: picking pass < 1ms at 1080p
- Integration test: picked object can be highlighted/manipulated
- Verify PickingPass is NEW class (not in existing 10)
- Verify m_pick_id is NEW field on Primitive (field count: 15 → 16)

---

## Mission 10: Add Instanced Rendering

### MISSION
Render many copies of the same Primitive (e.g., 10,000 cubes) in a single
draw call using glDrawArraysInstanced, with per-instance transforms in a
buffer.

### ARCHITECTURE PROPOSAL
- **Subsystem**: Renderer
- **Module**: `instanced_renderer.cpp` / `instanced_renderer.h`
- **Class**: InstancedRenderer
- **Pattern**: Single VBO for instance transforms, instanced draw call

### EXACT SYMBOLS

| Symbol | Status | Notes |
|--------|--------|-------|
| `Renderer` | REAL | 18 functions |
| `Primitive` | REAL | 15 fields, 25 methods |
| `Mesh` | REAL | 4 functions |
| `InstancedRenderer` | **NEW** | Instanced draw management |
| `InstancedRenderer::m_instance_vbo` | **NEW** | VBO for transforms |
| `InstancedRenderer::m_instance_count` | **NEW** | Number of instances |
| `InstancedRenderer::draw_instanced()` | **NEW** | glDrawArraysInstanced |
| `Primitive::m_instances` | **NEW** | Vector of instance transforms |
| `glm::mat4` | REAL | Instance transform matrix |

### DEPENDENCIES
- Primitive (gains instance data)
- Mesh (shares VAO across instances)
- ShaderProgram (must read instance matrix from attribute)
- GLM (transform matrices)

### IMPLEMENTATION ORDER
1. Create InstancedRenderer class
2. Add m_instances vector to Primitive
3. Create instance VBO with transform data (mat4 per instance)
4. Bind instance VBO to VAO at attribute location 3-6 (mat4 = 4 vec4s)
5. Set glVertexAttribDivisor for instance attributes
6. Call glDrawArraysInstanced instead of glDrawArrays
7. Add update_instances() to refresh instance buffer

### ACCEPTANCE CRITERIA
- 10,000 cubes rendered in 1 draw call
- Each instance has independent transform
- Instance buffer updates don't re-upload vertex data
- Non-instanced rendering still works (backward compat)
- InstancedRenderer is a new class (not modifying Renderer directly)

### VERIFICATION PLAN
- Performance test: 10K instances < 1ms draw time
- Visual test: instances render at correct positions
- Compare: 10K non-instanced draws (slow) vs 1 instanced draw (fast)
- Verify InstancedRenderer is NEW (not in existing 10 classes)
- Verify Primitive field count increases by 1 (m_instances)
- GPU profiling: single draw call for all instances

---

## Summary

| Mission | New Classes | New Fields | Modified Classes | Complexity |
|---------|------------|------------|-------------------|------------|
| 1. Texture Mapping | 0 | 2 | Primitive, ShaderProgram | LOW |
| 2. Material Layer | 0 (1 struct) | 1 | Primitive, Renderer | LOW |
| 3. Shared Mesh | 0 | 1 | Mesh, Primitive, Renderer | MEDIUM |
| 4. Orbit Camera | 0 (1 struct) | 3 | Camera | LOW |
| 5. Transform Hierarchy | 0 | 2 | SceneObject, Renderer | MEDIUM |
| 6. Scene Serialization | 1 | 0 | SceneObject, Primitive | MEDIUM |
| 7. Shader Abstraction | 0 (1 struct) | 0 | ShaderProgram, Renderer | MEDIUM |
| 8. GPU RAII | 4 | 0 | Primitive, ShaderProgram, TextureLoader | HIGH |
| 9. Object Picking | 1 | 1 | Primitive, Renderer, InputManager | HIGH |
| 10. Instanced Rendering | 1 | 1 | Primitive, Mesh | HIGH |

**Total estimated new symbols**: ~40 (across all missions)
**Total estimated modified classes**: 8 of 10 existing classes
**Architecture delta**: Significant but incremental (each mission builds on existing patterns)

---

*Generated as part of HSE-011 Knowledge Saturation Experiment*
