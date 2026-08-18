# HSE-008 MISSION — Bounded Texture Mapping

## Mission Statement

Add the smallest useful texture-mapping capability to HSE that can be architecturally justified, reaching symbol-level implementation specification without implementation.

## Explicit Scope

### IN SCOPE
1. UV coordinates on Primitive (Vec2 per vertex)
2. Texture resource loading (single 2D texture from file)
3. GPU texture object management (OpenGL texture)
4. Texture sampling in fragment shader
5. Per-primitive texture assignment
6. Bridge commands to set texture on a primitive
7. Serialization of texture references in .hsc format

### EXPLICITLY OUT OF SCOPE
- Material system (textures + lighting + normal maps + etc.)
- Texture atlas
- Texture compression (DXT, ASTC, ETC)
- Mipmaps
- Texture filtering modes (beyond default)
- Texture wrapping modes (beyond default)
- Multiple textures per primitive
- Procedural textures
- Harmonic materials
- PBR
- Editor UI for texture selection
- Asset pipeline / hot-reloading
- Skeletal animation with textures
- Terrain texturing
- Skybox / cubemap

## Target Behavior

After implementation, an external controller should be able to:

1. Add a Quad primitive
2. Assign a PNG/JPG texture file to it
3. See the texture rendered on the Quad (not just color)
4. Save the scene with texture references
5. Reload the scene and see textures restored

## Repository Evidence

| Capability | Current State | Evidence |
|-----------|---------------|----------|
| UV coordinates | NONE | Primitive has no m_texCoords field |
| Texture loading | NONE | No image loading code exists |
| GPU texture objects | NONE | No glGenTextures calls |
| Texture shader | NONE | Fragment shader outputs flat uColor only |
| Per-primitive state | YES | Primitive has m_color, m_vao, m_vbo, m_ebo |
| Bridge commands | YES | 18 command types, extensible |
| Serialization | YES | SceneBuilder exports/imports JSON |
| File I/O | YES | std::fstream used throughout |

## Knowledge Requirements

### Rendering Domain (CRITICAL)
- What UV coordinates are (0,0 to 1,1 mapping)
- What a GPU texture object is (glGenTextures, glBindTexture)
- What texture sampling is (sampler2D uniform)
- What texture units are (GL_TEXTURE0)
- What stb_image is (single-header image loader)

### 3D Math Domain (HIGH)
- Vec2 (not currently in HSE — needs creation or use float pair)
- UV mapping: vertex position → 2D texture coordinate

### Engine Architecture Domain (HIGH)
- Resource lifecycle (create GPU texture → assign to primitive → destroy on cleanup)
- Per-primitive GPU state (extend existing m_vao/m_vbo/m_ebo pattern)
- Serialization extension (add texCoords to JSON format)

## Implementation Constraints

1. **C++20** (CMakeLists.txt:4)
2. **OpenGL 3.3 Core Profile** (window.cpp:27-29)
3. **GLEW** for extension loading
4. **stb_image** for image loading (single header, no new dependency)
5. **No new CMake targets** (extend existing hse_core)
6. **Single shader program** (currently one program for all primitives)
7. **GL thread only** (all GL calls on main thread)

## Acceptance Criteria

### AC1: UV Coordinates Exist
- Primitive has `m_texCoords` field (vector of float, 2 floats per vertex)
- `generateGeometry()` produces UV coordinates for each PrimitiveType
- UV coordinates are uploaded to GPU in `uploadGPU()`

### AC2: Texture Loading Works
- `stb_image` loads PNG/JPG files
- Image data is uploaded to GPU as OpenGL texture
- Texture is associated with a Primitive

### AC3: Texture Sampling Works
- Vertex shader passes UV coordinates to fragment shader
- Fragment shader samples texture and multiplies with color
- Textured primitives display texture instead of flat color

### AC4: Bridge Integration Works
- New command: `set_texture` (assigns texture file to primitive)
- New observation: texture state visible in object observation
- External controller can texture a primitive via pipe

### AC5: Serialization Works
- .hsc format includes texture references
- Scene save/load preserves texture assignments
- Backward compatible (no texture = no change)

### AC6: Resource Cleanup Works
- GPU texture is deleted when Primitive is destroyed
- GPU texture is deleted when Renderer shuts down
- No GPU memory leaks

## Verification Strategy

### Unit Tests
- Test UV coordinate generation for each PrimitiveType
- Test texture loading from file
- Test GPU texture creation/deletion

### Integration Tests
- Test full pipeline: create primitive → load texture → render → verify pixels
- Test serialization: save scene with texture → reload → verify texture reference

### Visual Verification
- Create a textured quad (checkerboard pattern)
- Human verifies texture appears correctly
- Compare with color-only quad to confirm texture override

### External Controller Verification
- Bridge command sets texture on primitive
- Observation confirms texture assignment
- Save/load round-trip preserves texture
