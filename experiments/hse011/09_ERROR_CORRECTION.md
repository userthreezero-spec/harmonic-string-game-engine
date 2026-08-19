# HSE-011: Error Correction Test

> **Purpose**: Test whether increased knowledge reduces known errors from
> HSE-008 and whether the corpus can catch new deliberately plausible-but-wrong
> claims about HSE's architecture.

---

## Part A: HSE-008 Known Errors (Regression Test)

### Error A: m_normals Hallucination

**Claim (HSE-008)**: "Primitive has m_normals (std::vector<glm::vec3>) field"

**Why It's Wrong**: Primitive stores normals as part of its interleaved vertex
data (m_vertices), not as a separate m_normals field. The vertex structure
contains position, color, and potentially other attributes packed together.
There is no standalone m_normals vector.

**How Knowledge Helps**:
- Architecture verifier output clearly lists Primitive's 15 fields
- m_normals does NOT appear in the field list
- Source code inspection shows vertices are interleaved, not split
- The vertex layout in Primitive's constructor binds interleaved attributes

**Correct Answer**: Primitive has no m_normals field. Normals are part of the
interleaved vertex data in m_vertices (std::vector<Vertex> or similar).

**Expected Behavior with Full Corpus**:
- ✅ Field list from verify.py shows 15 fields without m_normals
- ✅ Source code shows interleaved vertex binding
- ✅ No grep hits for "m_normals" in Primitive class
- **CORRECTION LIKELIHOOD: HIGH** (verifier output is explicit)

---

### Error B: renderer_state.cpp Hallucination

**Claim (HSE-008)**: "renderer_state.cpp contains the renderer's state management code"

**Why It's Wrong**: The file renderer_state.cpp does not exist in HSE's source
tree. Renderer state is managed directly in renderer.cpp and renderer.h. There
is no separate state management file.

**How Knowledge Helps**:
- File listing from verify.py shows all 27 source files
- renderer_state.cpp is not among them
- Grep for "renderer_state" in the codebase yields no hits
- Renderer state variables are declared in renderer.h

**Correct Answer**: No such file exists. Renderer state is managed in
renderer.cpp / renderer.h directly.

**Expected Behavior with Full Corpus**:
- ✅ File listing from verify.py doesn't include renderer_state.cpp
- ✅ Source tree enumeration confirms 27 files, none named renderer_state
- ✅ Renderer state is found in renderer.cpp
- **CORRECTION LIKELIHOOD: HIGH** (file existence is binary)

---

### Error C: UV VBO Lifecycle Not Analyzed

**Claim (HSE-008)**: "UV coordinates are stored in a separate VBO with their own lifecycle"

**Why It's Wrong**: UV coordinates, when present, are part of the same
interleaved vertex buffer as positions and colors. There is no separate UV VBO.
The VBO lifecycle is shared across all vertex attributes because they're in the
same buffer object.

**How Knowledge Helps**:
- Primitive's vertex layout shows interleaved attributes (position + color at minimum)
- Single VBO creation in Primitive's constructor
- VAO binding sets up attribute pointers into the same VBO at different offsets
- No second VBO creation for UVs

**Correct Answer**: UVs (if present) share the main vertex VBO with other
attributes via interleaved layout. There is no separate UV VBO.

**Expected Behavior with Full Corpus**:
- ✅ Primitive's VBO creation is single (one glGenBuffers call)
- ✅ VAO attribute pointers reference offsets into the same VBO
- ✅ No separate UV buffer creation code
- **CORRECTION LIKELIHOOD: HIGH** (VBO creation is explicit in source)

---

### Error D: TextureManager Over-Engineered

**Claim (HSE-008)**: "HSE has a TextureManager class that handles texture caching, reference counting, and async loading"

**Why It's Wrong**: HSE does not have a TextureManager class. Textures are
loaded directly via TextureLoader (7 functions), which is a stateless utility
that creates GL textures. There is no caching, no reference counting, and no
async loading — just direct texture creation.

**How Knowledge Helps**:
- Class listing from verify.py shows 10 classes, no TextureManager
- TextureLoader is listed as a class with 7 functions
- No "Manager" suffix on any texture-related class
- Source code shows TextureLoader is a simple utility

**Correct Answer**: No TextureManager exists. TextureLoader is a simple 7-function
utility that creates GL textures directly.

**Expected Behavior with Full Corpus**:
- ✅ Class listing doesn't include TextureManager
- ✅ TextureLoader is listed with 7 functions (not 20+)
- ✅ No reference counting, caching, or async in TextureLoader code
- **CORRECTION LIKELIHOOD: HIGH** (class listing is explicit)

---

## Part B: New Deliberately Wrong Claims (Detection Test)

### Error E: "Primitive has m_material_id (int)"

**Why It's Plausible**: Many engines assign material IDs to primitives. If you
know about material systems (from other engine docs), you'd expect Primitive to
reference a material somehow.

**Why It's Wrong**: Primitive has no material field of any kind. Primitive is a
pure geometry class — it holds vertex data, VAO/VBO handles, and draw
parameters. Material assignment is a renderer-level concern, not a Primitive
field.

**Evidence Against**:
- Primitive's 15 fields (from verify.py) include no material-related field
- No "material" string appears in Primitive class definition
- Renderer handles material binding externally before draw calls

**How Knowledge Helps**:
- Field list from verify.py is exhaustive (15 fields listed)
- No m_material_id, m_material, or similar field exists
- Renderer::draw() binds materials externally, not via Primitive reference

**Correct Answer**: Primitive has no material field. Material binding is
handled by the Renderer before each draw call.

**Detection Difficulty**: MEDIUM — requires checking field list carefully

---

### Error F: "Renderer uses render queue pattern"

**Why It's Plausible**: Render queues are common in game engines (deferred
rendering, command buffers). If you know about modern rendering patterns, you'd
expect a queue.

**Why It's Wrong**: HSE's Renderer draws directly — SceneObjects are iterated
and drawn immediately. There is no intermediate queue, command buffer, or
batching step. The renderer is a simple forward renderer.

**Evidence Against**:
- Renderer has 18 functions — none named "queue", "submit", "flush", "batch"
- No render queue data structure in Renderer's fields
- Draw calls happen immediately in the render loop
- No deferred rendering pipeline

**How Knowledge Helps**:
- Renderer function list shows direct draw calls
- No queue-related data structures in Renderer
- Render loop in main.cpp shows immediate iteration and drawing
- 18 functions is too few for a full render queue system

**Correct Answer**: Renderer draws directly without a queue. SceneObjects are
iterated and drawn immediately in a forward rendering pass.

**Detection Difficulty**: MEDIUM — requires understanding the render loop structure

---

### Error G: "ShaderProgram stores uniform cache in unordered_map"

**Why It's Plausible**: Uniform caching via hash map is a common optimization
(pattern seen in many engines). It avoids repeated glGetUniformLocation calls.

**Why It's Wrong**: HSE's ShaderProgram uses a simpler approach — uniforms are
set by name each time without caching. There is no unordered_map for uniform
locations. The uniform lookup happens fresh each set_uniform call.

**Evidence Against**:
- ShaderProgram has 10 functions — no "cache" function
- No unordered_map or map in ShaderProgram's fields
- set_uniform implementations call glGetUniformLocation directly
- No caching data structure in ShaderProgram

**How Knowledge Helps**:
- ShaderProgram's 10 functions are listed (no cache management)
- Field list doesn't include any map/hash structure
- Source code shows direct glGetUniformLocation calls
- GL 3.3 doesn't require caching (driver handles this)

**Correct Answer**: ShaderProgram does not cache uniform locations. Each
set_uniform call uses glGetUniformLocation directly.

**Detection Difficulty**: HIGH — requires reading ShaderProgram source to confirm

---

### Error H: "Camera inherits from SceneObject"

**Why It's Plausible**: In many engines, Camera is a SceneObject with special
rendering behavior. This is the component-based pattern.

**Why It's Wrong**: Camera is an independent class with no inheritance from
SceneObject. Camera manages its own position, orientation, and projection
matrix. It does not participate in the scene graph hierarchy.

**Evidence Against**:
- Camera class definition shows no inheritance (no ": public SceneObject")
- Camera has its own m_position, m_front, m_up (duplicated from SceneObject)
- Camera is not stored in SceneObject's hierarchy
- Renderer references Camera directly, not through scene graph

**How Knowledge Helps**:
- Class hierarchy from verify.py shows Camera as standalone
- No inheritance relationship between Camera and SceneObject
- Camera's 8 functions are independent of SceneObject's 6 methods
- Camera is passed to Renderer separately from SceneObject list

**Correct Answer**: Camera is an independent class, not a SceneObject. It
manages its own transform and is passed to the Renderer separately.

**Detection Difficulty**: MEDIUM — requires checking class hierarchy

---

### Error I: "Mesh owns its VAO"

**Why It's Plausible**: In many engines, Mesh owns GPU resources (VAO, VBO).
Mesh is the logical owner of geometry data.

**Why It's Wrong**: In HSE, Primitive owns its own VAO and VBO. Mesh is a
lightweight class (4 functions) that doesn't own GPU resources. Primitive
creates and manages its VAO/VBO directly in its constructor and destructor.

**Evidence Against**:
- Primitive's field list includes m_vao and m_vbo (direct ownership)
- Mesh has only 4 functions — no VAO/VBO management
- Primitive's constructor creates VAO/VBO (not Mesh)
- Primitive's destructor deletes VAO/VBO (not Mesh)

**How Knowledge Helps**:
- Primitive field list explicitly shows m_vao and m_vbo
- Mesh field list (4 functions) shows no VAO/VBO ownership
- Constructor/destructor code in Primitive handles VAO/VBO lifecycle
- Mesh is referenced by Primitive but doesn't own GPU resources

**Correct Answer**: Primitive owns its VAO and VBO directly. Mesh is a
lightweight class without GPU resource ownership.

**Detection Difficulty**: HIGH — requires understanding the Primitive-Mesh
relationship and checking field ownership

---

## Part C: Summary

### Error Detection Matrix

| Error | Type | HSE-008? | Detection Difficulty | Expected with Full Corpus |
|-------|------|----------|---------------------|--------------------------|
| A. m_normals hallucination | Field hallucination | Yes | LOW | ✅ CATCH |
| B. renderer_state.cpp | File hallucination | Yes | LOW | ✅ CATCH |
| C. UV VBO lifecycle | Analysis gap | Yes | MEDIUM | ✅ CATCH |
| D. TextureManager over-engineered | Class hallucination | Yes | LOW | ✅ CATCH |
| E. m_material_id | Field hallucination | No | MEDIUM | ✅ CATCH |
| F. Render queue pattern | Pattern hallucination | No | MEDIUM | ⚠️ MAYBE |
| G. Uniform cache in map | Implementation hallucination | No | HIGH | ⚠️ MAYBE |
| H. Camera inherits SceneObject | Hierarchy hallucination | No | MEDIUM | ✅ CATCH |
| I. Mesh owns VAO | Ownership hallucination | No | HIGH | ⚠️ MAYBE |

### Key Findings

1. **HSE-008 errors (A-D)**: All 4 should be caught with full corpus. The
   verifier output provides explicit field/file/function lists.

2. **New errors (E-I)**: 3 of 5 should be caught reliably (E, H, and partially
   F). The other 2 (G, I) require deep source reading that may not be in the
   corpus summary.

3. **Detection pattern**: Errors about **existence** (file exists? field exists?)
   are easiest to catch. Errors about **implementation details** (how does
   uniform caching work?) are hardest.

4. **Corpus weakness**: The corpus is strong at structural verification (what
   exists) but weaker at implementation verification (how things work
   internally).

### Recommendation

To improve error correction:
- Include implementation details in the corpus (not just structure)
- Add negative examples ("Primitive does NOT have m_normals")
- Include field-by-field documentation for key classes
- Add common misconception warnings ("Mesh does NOT own VAO")

---

*Generated as part of HSE-011 Knowledge Saturation Experiment*
