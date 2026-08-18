# HSE-008 WEBOS ARCHITECTURE — Texture Mapping Implementation Architecture

## Architecture Overview

The texture mapping system extends HSE's existing per-primitive GPU resource pattern (VAO/VBO/EBO) with texture resources (texture object + UV coordinates). No new subsystems are created. All changes extend existing classes within existing ownership boundaries.

## Ownership Model (Current → Proposed)

```
CURRENT:
  Primitive owns: VAO, VBO, EBO (GPU resources)
  Renderer owns: shader program (GPU resource)
  Scene owns: Primitives (via shared_ptr)
  Bridge owns: PipeServer, CommandQueue

PROPOSED:
  Primitive owns: VAO, VBO, EBO + texture ID + UV data (GPU resources)
  Renderer owns: shader program (unchanged)
  Scene owns: Primitives (unchanged)
  Bridge owns: PipeServer, CommandQueue (unchanged)
  NEW: ResourceManager owns: loaded textures (GPU texture objects)
```

## Proposed Changes

### 1. Vec2 Math Type (NEW FILE)

**File**: `include/math/vec2.h`

**Rationale**: UV coordinates require 2D vectors. Vec3 wastes memory (1 float per UV) and misrepresents the data.

**Contents**:
```cpp
struct Vec2 {
    float x, y;
    Vec2(float x = 0.0f, float y = 0.0f);
    // operators: +, -, *, ==
};
```

**Classification**: NEW FILE — no existing file to modify.
**Confidence**: HIGH — standard math type needed for UV data.

### 2. Primitive UV Extension

**File**: `include/scene/primitive.h`

| Symbol | Change | Current | Required |
|--------|--------|---------|----------|
| `m_texCoords` | ADD | (none) | `std::vector<float>` — 2 floats per vertex |
| `m_textureID` | ADD | (none) | `uint32_t` — GPU texture handle (0 = no texture) |
| `m_texturePath` | ADD | (none) | `std::string` — path for serialization |
| `getTexCoords()` | ADD | (none) | `const std::vector<float>&` accessor |
| `getTextureID()` | ADD | (none) | `uint32_t` accessor |
| `setTexture()` | ADD | (none) | `void setTexture(uint32_t id, const std::string& path)` |
| `getTexturePath()` | ADD | (none) | `const std::string&` accessor |

**File**: `src/scene/primitive.cpp`

| Symbol | Change | Current | Required |
|--------|--------|---------|----------|
| `generateGeometry()` | MODIFY | Generates vertices + indices | Also generate UV coordinates per type |
| `uploadGPU()` | MODIFY | Creates VAO/VBO/EBO, sets attrib 0 | Also create UV VBO, set attrib 1 |
| `~Primitive()` | MODIFY | Deletes VAO/VBO/EBO | Also delete texture (if owned) |
| `bind()` | MODIFY | Binds VAO | Also binds texture to unit 0 |
| `unbind()` | MODIFY | Unbinds VAO | Also unbinds texture |

**Data Flow**:
```
generateGeometry() → fills m_vertices + m_indices + m_texCoords
uploadGPU() → creates VAO + VBO(position) + VBO(UV) + EBO
bind() → glBindVertexArray + glBindTexture(GL_TEXTURE_2D, m_textureID)
render() → shader uses UV to sample texture
```

**Classification**: MODIFY EXISTING — extends Primitive's GPU resource pattern.
**Confidence**: HIGH — follows established VAO/VBO/EBO pattern.

### 3. Texture Resource Manager (NEW FILE)

**File**: `include/resource/texture_manager.h`

**Rationale**: Texture loading/management is a distinct concern from Primitive (which is a scene object) and Renderer (which is a rendering pipeline). A resource manager handles:
- Loading textures from files (stb_image)
- Uploading to GPU (glGenTextures, glTexImage2D)
- Reference counting (multiple primitives can share a texture)
- Cache (load once, assign many times)
- Cleanup (delete GPU textures on shutdown)

**Contents**:
```cpp
class TextureManager {
public:
    uint32_t loadTexture(const std::string& path);
    void releaseTexture(uint32_t textureID);
    void bindTexture(uint32_t textureID, uint32_t unit);
    void clear();
private:
    struct TextureEntry {
        uint32_t glID;
        std::string path;
        int width, height;
        int refCount;
    };
    std::unordered_map<uint32_t, TextureEntry> m_textures;
    uint32_t m_nextID = 1;
};
```

**File**: `src/resource/texture_manager.cpp`

**Contents**: Implementation of loadTexture (stb_image + glTexImage2D), releaseTexture (glDeleteTextures), bindTexture (glActiveTexture + glBindTexture), clear (delete all).

**Classification**: NEW FILE — no existing resource management exists.
**Confidence**: HIGH — standard resource manager pattern.

**Dependencies**: stb_image (single header), OpenGL (already linked).

### 4. Shader Extension

**File**: `src/renderer/renderer.cpp`

| Symbol | Change | Current | Required |
|--------|--------|---------|----------|
| `vertexShaderSource` | MODIFY | Has aPos + MVP uniforms | Add `in vec2 aTexCoord`, `out vec2 vTexCoord` |
| `fragmentShaderSource` | MODIFY | Has uColor output | Add `uniform sampler2D uTexture`, `uniform bool uHasTexture` |
| `renderScene()` | MODIFY | Sets uColor uniform | Also binds texture, sets uHasTexture + uTexture uniform |

**Modified Vertex Shader**:
```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
out vec2 vTexCoord;
void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    vTexCoord = aTexCoord;
}
```

**Modified Fragment Shader**:
```glsl
#version 330 core
in vec2 vTexCoord;
out vec4 FragColor;
uniform vec3 uColor;
uniform sampler2D uTexture;
uniform bool uHasTexture;
void main() {
    if (uHasTexture) {
        FragColor = texture(uTexture, vTexCoord) * vec4(uColor, 1.0);
    } else {
        FragColor = vec4(uColor, 1.0);
    }
}
```

**Classification**: MODIFY EXISTING — extends existing shader.
**Confidence**: HIGH — standard texture mapping shader pattern.

**Note**: The `uHasTexture` uniform enables backward compatibility — primitives without textures continue to render with flat color.

### 5. Bridge Command Extension

**File**: `include/bridge/command_queue.h`

| Symbol | Change | Current | Required |
|--------|--------|---------|----------|
| `CMD_SET_TEXTURE` | ADD | (none) | New command type (value 18) |
| `Command::texturePath` | ADD | (none) | `char[256]` — texture file path |

**File**: `src/bridge/bridge.cpp`

| Symbol | Change | Current | Required |
|--------|--------|---------|----------|
| `parseCommand()` | MODIFY | 18 command parsers | Add "set_texture" parser |
| `executeCommand()` | MODIFY | 18-case switch | Add CMD_SET_TEXTURE case |

**Command Protocol**:
```json
{"cmd":"set_texture","object_id":1,"path":"assets/texture.png","seq":100}
```

**Response**:
```json
{"type":"command_ack","status":"ok","object_id":1,"revision":5}
```

**Classification**: MODIFY EXISTING — extends existing command protocol.
**Confidence**: HIGH — follows established command pattern.

### 6. Serialization Extension

**File**: `src/scene/scene_builder.cpp`

| Symbol | Change | Current | Required |
|--------|--------|---------|----------|
| `exportHSC()` | MODIFY | Writes position/rotation/scale/color | Also writes texCoords + texturePath |
| `importState()` | MODIFY | Reads position/rotation/scale/color | Also reads texCoords + texturePath |

**JSON Format Extension** (backward compatible):
```json
{
  "id": "floor",
  "type": "Quad",
  "position": [0, -1.5, 0],
  "rotation": [-90, 0, 0],
  "scale": [8, 8, 1],
  "color": [0.3, 0.25, 0.2],
  "tex_coords": [0,0, 1,0, 1,1, 0,1],
  "texture": "assets/wood.png"
}
```

**Classification**: MODIFY EXISTING — extends serialization format.
**Confidence**: HIGH — follows existing JSON pattern.

### 7. Renderer Texture Binding

**File**: `src/renderer/renderer.cpp`

| Symbol | Change | Current | Required |
|--------|--------|---------|----------|
| `renderScene()` | MODIFY | Sets uColor, draws | Also sets uHasTexture, binds texture, sets uTexture |
| `m_state` | NO CHANGE | RendererState | (unchanged) |

**Texture Binding in renderScene**:
```cpp
for (const auto& prim : scene.getPrimitives()) {
    // ... existing model matrix setup ...
    
    // Texture handling
    uint32_t texID = prim->getTextureID();
    if (texID != 0) {
        glUniform1i(hasTextureLoc, 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);
        glUniform1i(textureLoc, 0);
    } else {
        glUniform1i(hasTextureLoc, 0);
    }
    
    // ... existing color + draw call ...
}
```

**Classification**: MODIFY EXISTING — extends render loop.
**Confidence**: HIGH — minimal change to existing loop.

## Dependency Graph

```
vec2.h (NEW)
  └── primitive.h (uses Vec2 for texCoords)

primitive.h/cpp (MODIFY)
  ├── adds m_texCoords, m_textureID, m_texturePath
  ├── generateGeometry() produces UVs
  ├── uploadGPU() creates UV VBO
  └── bind() binds texture

texture_manager.h/cpp (NEW)
  ├── uses stb_image for loading
  ├── uses OpenGL for GPU upload
  └── manages texture lifecycle

renderer.cpp (MODIFY)
  ├── vertex shader adds aTexCoord
  ├── fragment shader adds texture sampling
  └── renderScene() binds textures

command_queue.h (MODIFY)
  └── adds CMD_SET_TEXTURE, Command::texturePath

bridge.cpp (MODIFY)
  ├── parseCommand() handles "set_texture"
  └── executeCommand() loads + assigns texture

scene_builder.cpp (MODIFY)
  ├── exportHSC() writes texCoords + texturePath
  └── importState() reads texCoords + texturePath
```

## Data Flow

```
1. External controller sends: {"cmd":"set_texture","object_id":1,"path":"texture.png"}
2. Bridge::parseCommand() → Command{type=CMD_SET_TEXTURE, objectID=1, texturePath="texture.png"}
3. Bridge::executeCommand() → 
   a. Scene.findByID(1) → shared_ptr<Primitive>
   b. TextureManager.loadTexture("texture.png") → uint32_t texID
   c. Primitive::setTexture(texID, "texture.png")
4. Next frame: Renderer::renderScene() →
   a. For each primitive: check getTextureID()
   b. If != 0: bind texture, set uHasTexture=1, set uTexture=0
   c. If == 0: set uHasTexture=0, use flat color
5. GPU: vertex shader passes UV to fragment shader
6. GPU: fragment shader samples texture, multiplies with color
```

## Build System Changes

**CMakeLists.txt**: Add `src/resource/texture_manager.cpp` to `hse_core` sources.
**New dependency**: stb_image.h (single header, placed in `include/third_party/stb_image.h`).

## Uncertainty Report

| Claim | Confidence | Evidence |
|-------|-----------|----------|
| UV coordinates per vertex are needed | HIGH | Standard texture mapping pattern |
| Vec2 is needed | HIGH | UV coords are 2D, Vec3 wastes memory |
| stb_image is appropriate | HIGH | Industry standard, single header, no build system changes |
| TextureManager is needed | MEDIUM | Could inline in Primitive, but separation is cleaner |
| uHasTexture uniform for backward compat | HIGH | Ensures untextured primitives still work |
| CMD_SET_TEXTURE follows existing pattern | HIGH | 18 command types exist, adding 19th is trivial |
| texCoords in JSON format | HIGH | Extends existing serialization |
| No mipmap generation needed | HIGH | Mission scope excludes mipmaps |
| No texture filtering changes needed | HIGH | GL_LINEAR default is fine for first implementation |
