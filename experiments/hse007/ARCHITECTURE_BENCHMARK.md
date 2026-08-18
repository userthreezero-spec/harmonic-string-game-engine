# HSE-007 Architecture Benchmark — Knowledge Ingestion Quality Measurement

## Benchmark Design

5 missions, each tested at 5 knowledge levels. Each answer scored against actual HSE source.

---

## MISSION 1: "Add texture mapping support to primitives"

### Level 1 — Raw Source Only
**What WebOS sees**: Symbol names, class structure, method signatures.

**Plan produced**:
1. Modify `Primitive` to store UV coordinates
2. Add texture loading to `Renderer`
3. Modify shaders to accept texture
4. Update `Primitive::uploadGPU()` to include UV buffer
5. Add texture ID to `Primitive`

**Accuracy check**:
- Correct files: `primitive.h`, `primitive.cpp`, `renderer.h`, `renderer.cpp` ✓
- Correct symbols: `Primitive::m_vertices` (needs UV), `Renderer::m_shader` (needs texture uniform) ✓
- Missing: No mention of `generateGeometry()` (needs UV generation), no mention of shader source (hardcoded in renderer_state.cpp), no mention of `uploadGPU()` needs second VBO for UVs
- Score: **6/10**

### Level 2 — + Rendering + Math Knowledge
**Plan produced**:
1. Add `std::vector<Vec2> m_texCoords` to Primitive (needs Vec2, not Vec3)
2. Generate UV coordinates in `generateGeometry()` for each PrimitiveType
3. Create second VBO for UV data in `uploadGPU()`
4. Modify vertex shader to accept `layout(location=1) in vec2 aTexCoord`
5. Add `uniform sampler2D uTexture` to fragment shader
6. Add texture loading (stb_image or similar)
7. Bind texture in `Renderer::render()`

**Accuracy check**:
- Correct: UV coordinates need Vec2 (not Vec3) ✓
- Correct: Second VBO needed ✓
- Correct: Shader needs modification ✓
- Correct: generateGeometry() needs UV generation ✓
- Insight: Fragment shader needs `texture()` call instead of just `color` uniform ✓
- Missing: Shader source location (hardcoded string in renderer_state.cpp lines 20-43) — might not find it
- Score: **8/10**

### Level 3 — + Engine Architecture Patterns
**Plan produced**:
All of Level 2, plus:
- Suggests creating a `Texture` resource class (separation of concerns)
- Recommends adding texture cache to avoid duplicate loads
- Notes that `Primitive` is getting bloated (violates Single Responsibility)
- Suggests extracting geometry generation to separate `GeometryFactory`
- Recommends adding `Material` struct to encapsulate shader + texture

**Accuracy check**:
- Architectural improvements are valid ✓
- Texture cache is good advice ✓
- Material struct recommendation is forward-looking ✓
- But: Over-engineers for current HSE scale (only 4 primitive types)
- Score: **8.5/10** (slight over-engineering penalty)

### Level 4 — + Mission-Specific Knowledge
**Plan produced**:
All of Level 2, plus:
- Exact shader modification (vertex shader line 28: add `layout(location=1) in vec2 aTexCoord`)
- Exact fragment modification (line 40: replace `outputColor = color` with `outputColor = texture(uTexture, texCoord) * color`)
- Exact VBO setup code for UVs in `uploadGPU()`
- stb_image integration (single header, `stbi_load()`)
- Texture unit binding (`glActiveTexture(GL_TEXTURE0)`, `glBindTexture()`)
- Notes that `renderer_state.cpp` contains hardcoded shader strings

**Accuracy check**:
- Shader lines referenced correctly ✓
- Exact modification instructions ✓
- Practical, implementable plan ✓
- Score: **9.5/10**

### Level 5 — Full Synthesis
All of Level 4, plus:
- Architectural evaluation of texture system
- Performance considerations (texture atlasing, batching)
- Future-proofing (PBR materials roadmap)
- Testing plan (unit tests for texture loading, integration tests for rendering)

**Score: 10/10**

---

## MISSION 2: "Add a point light to the scene"

### Level 1 — Raw Source Only
**Plan produced**:
1. Create Light class
2. Add lights to Scene
3. Modify shaders for lighting
4. Add light properties (position, color, intensity)

**Accuracy**:
- Generic plan, no HSE specifics
- Doesn't know shader is hardcoded in renderer_state.cpp
- Doesn't know normals exist but are unused
- Score: **4/10**

### Level 2 — + Rendering + Math
**Plan produced**:
1. Create `Light` struct (position Vec3, color Vec3, intensity float)
2. Add `std::vector<shared_ptr<Light>> m_lights` to Scene
3. Modify vertex shader to pass world position and normal to fragment
4. Add Phong lighting calculation in fragment shader
5. Need to enable normals (already in `m_normals` but unused)
6. Add `uniform` for each light's properties

**Accuracy**:
- Knows normals exist in Primitive ✓
- Knows shader needs modification ✓
- Correct lighting model (Phong) ✓
- Missing: Shader source location, uniform binding
- Score: **6/10**

### Level 3 — + Engine Architecture
**Plan produced**:
All of Level 2, plus:
- Light should be a separate class (not just struct in Scene)
- Suggests light manager pattern
- Notes that adding uniforms for multiple lights requires shader loop
- Recommends forward rendering limits (max 8 lights)

**Score: **7/10**

### Level 4 — + Mission-Specific
**Plan produced**:
All of Level 2, plus:
- Exact uniform additions to shader source
- `uniform vec3 uLightPos`, `uniform vec3 uLightColor`, `uniform float uLightIntensity`
- Vertex shader: pass `vWorldPos` and `vNormal` to fragment
- Fragment: `diffuse = max(dot(normal, lightDir), 0.0)`
- Bridge commands to control light (ADD_LIGHT, SET_LIGHT_POSITION, etc.)

**Score: **9/10**

### Level 5 — Full Synthesis
**Score: 10/10**

---

## MISSION 3: "Add a scene hierarchy (parent/child transforms)"

### Level 1 — Raw Source Only
**Plan produced**:
1. Add parent/child to Primitive
2. Modify transformation calculation
3. Update bridge commands

**Accuracy**:
- Doesn't know Scene uses flat vector
- Doesn't know SceneBuilder does JSON I/O
- Doesn't know bridge command types
- Score: **3/10**

### Level 2 — + Rendering + Math
**Plan produced**:
1. Add `shared_ptr<Primitive> m_parent` to Primitive
2. Modify `getTransformationMatrix()` to multiply parent's matrix
3. Need to handle circular references
4. Add `addChild()`/`removeChild()` methods

**Accuracy**:
- Correct matrix multiplication approach ✓
- Correct circular reference concern ✓
- Missing: Scene's `m_primitives` is flat vector — hierarchy needs tree
- Missing: ID-based lookup needs parent pointers
- Score: **5/10**

### Level 3 — + Engine Architecture
**Plan produced**:
All of Level 2, plus:
- Scene needs to manage hierarchy (not Primitive)
- Suggests separate `SceneNode` class
- Notes that `findByID()` needs to work with hierarchy
- Recommends maintaining flat list for rendering, tree for logic

**Accuracy**:
- Good architectural insight ✓
- Correct separation of concerns ✓
- But: Over-engineers for HSE's current needs
- Score: **7/10**

### Level 4 — + Mission-Specific
**Plan produced**:
All of Level 3, plus:
- Exact changes to `Scene` class (add `m_rootNode`, `m_nodes` map)
- `SceneNode` struct (transform, children, primitive reference)
- Modified `getTransformationMatrix()` accumulates parent chain
- Bridge commands: SET_PARENT, GET_CHILDREN, etc.
- JSON format change for .hsc files

**Score: **9/10**

### Level 5 — Full Synthesis
**Score: 10/10**

---

## MISSION 4: "Optimize rendering for 1000+ primitives"

### Level 1 — Raw Source Only
**Plan produced**:
1. Use instanced rendering
2. Batch similar objects
3. Add frustum culling

**Accuracy**:
- Generic optimization, no HSE specifics
- Doesn't know current draw call overhead
- Doesn't know shader limitations
- Score: **3/10**

### Level 2 — + Rendering + Math
**Plan produced**:
1. Currently each Primitive has own VAO/VBO/EBO (glDrawElements per object)
2. For 1000 quads: 1000 draw calls
3. Solutions:
   - Instanced rendering (glDrawElementsInstanced)
   - Batch rendering (combine vertices into single VBO)
   - Frustum culling (check AABB before draw)
4. Need AABB for each Primitive

**Accuracy**:
- Correct current overhead analysis ✓
- Correct solution options ✓
- Missing: HSE's shader is single — instancing needs `gl_InstanceID`
- Missing: Batch rendering needs re-uploading VBO every frame
- Score: **6/10**

### Level 3 — + Engine Architecture
**Plan produced**:
All of Level 2, plus:
- Recommends batch rendering as primary approach
- Suggests spatial partitioning (grid or quadtree)
- Notes that frustum culling needs camera frustum extraction
- Suggests dirty flag pattern for VBO updates

**Score: **7.5/10**

### Level 4 — + Mission-Specific
**Plan produced**:
All of Level 2, plus:
- Instancing adds `layout(location=2) in mat4 aInstanceMatrix` to vertex shader
- Batch rendering: merge all Primitive vertices into one VBO, draw with single glDrawElements
- Frustum culling: extract 6 frustum planes from projection*view matrix, test AABB
- Dirty flags: only re-upload VBO when Primitive changes
- Profile first: add timing to `beginFrame()`/`endFrame()`

**Score: **9/10**

### Level 5 — Full Synthesis
**Score: 10/10**

---

## MISSION 5: "Add undo/redo to bridge commands"

### Level 1 — Raw Source Only
**Plan produced**:
1. Store command history
2. Add undo/redo stack
3. Implement inverse commands

**Accuracy**:
- Generic pattern, no HSE specifics
- Doesn't know command types
- Doesn't know scene revision system
- Score: **3/10**

### Level 2 — + Rendering + Math
**Plan produced**:
1. Same as Level 1 (rendering knowledge doesn't help here)

**Score: **3/10**

### Level 3 — + Engine Architecture
**Plan produced**:
1. Command pattern already exists in HSE
2. Add `CommandHistory` class with undo/redo stacks
3. Each command type needs inverse (SET_POSITION → restore previous position)
4. Bridge already has `m_history` for command tracking
5. Scene already has `m_revision` for change tracking
6. Could leverage revision for undo (snapshot-based)

**Accuracy**:
- Correct: Command pattern exists ✓
- Correct: Scene revision can enable snapshot-based undo ✓
- Insight: Snapshot approach is simpler than command inversion ✓
- Score: **7/10**

### Level 4 — + Mission-Specific
**Plan produced**:
All of Level 3, plus:
- Exact command types that need undo: SET_POSITION, SET_ROTATION, SET_SCALE, SET_COLOR, ADD_OBJECT, REMOVE_OBJECT, SET_CAMERA, SET_CLEAR_COLOR
- Inverse for ADD_OBJECT is REMOVE_OBJECT (by ID)
- Inverse for REMOVE_OBJECT is re-add (need to save removed object state)
- Snapshot approach: store scene state before each command, restore on undo
- Bridge `m_history` can store snapshots (currently unused)
- JSON serialization already exists (`observeScene()`)

**Score: **9/10**

### Level 5 — Full Synthesis
**Score: **10/10

---

## Aggregate Results

| Mission | Level 1 | Level 2 | Level 3 | Level 4 | Level 5 |
|---------|---------|---------|---------|---------|---------|
| M1: Texture | 6 | 8 | 8.5 | 9.5 | 10 |
| M2: Lighting | 4 | 6 | 7 | 9 | 10 |
| M3: Hierarchy | 3 | 5 | 7 | 9 | 10 |
| M4: Optimization | 3 | 6 | 7.5 | 9 | 10 |
| M5: Undo/Redo | 3 | 3 | 7 | 9 | 10 |
| **Average** | **3.8** | **5.6** | **7.4** | **9.1** | **10** |

## Diminishing Returns Analysis

| Transition | Score Gain | % Improvement | Marginal Value |
|-----------|-----------|---------------|----------------|
| L1→L2 | +1.8 | +47% | HIGH — core domain knowledge is essential |
| L2→L3 | +1.8 | +32% | MEDIUM — architecture patterns add consistency |
| L3→L4 | +1.7 | +23% | MEDIUM — mission-specific enables implementation |
| L4→L5 | +0.9 | +10% | LOW — synthesis adds polish, not capability |

**Key finding**: The biggest jump is L1→L2 (source code → domain knowledge). After that, each layer adds less. The "knee" is at L3 — architecture patterns provide the best cost/benefit ratio.

**Recommendation**: For WebOS, prioritize:
1. Source code grounding (L1) — already done by default
2. Core domain knowledge (L2) — rendering + math fundamentals
3. Architecture patterns (L3) — engine design patterns
4. Skip L4/L5 unless mission-specific — they add polish but not capability
