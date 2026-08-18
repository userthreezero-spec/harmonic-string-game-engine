# HSE-008 ARCHITECTURE TO SYMBOL MAP

## Tracing: Architecture → File → Symbol → Change → Verification

---

### ARCHITECTURE CHANGE 1: UV Coordinate Support

**Architecture Level**: Primitives need 2D texture coordinates.

**File**: `include/math/vec2.h` (NEW)
| Symbol | Type | Purpose |
|--------|------|---------|
| `Vec2` | struct | 2D vector for UV coordinates |
| `Vec2::x` | float | X coordinate |
| `Vec2::y` | float | Y coordinate |
| `Vec2::operator+` | method | Vector addition |
| `Vec2::operator-` | method | Vector subtraction |
| `Vec2::operator*` | method | Scalar multiplication |

**File**: `src/math/vec2.cpp` (NEW)
| Symbol | Purpose |
|--------|---------|
| Implementation of Vec2 methods | Standard math operations |

**File**: `include/scene/primitive.h`
| Symbol | Change | Current State | Required State |
|--------|--------|---------------|----------------|
| `m_texCoords` | ADD FIELD | (none) | `std::vector<float>` |
| `m_textureID` | ADD FIELD | (none) | `uint32_t` |
| `m_texturePath` | ADD FIELD | (none) | `std::string` |
| `getTexCoords()` | ADD METHOD | (none) | `const std::vector<float>& getTexCoords() const` |
| `getTextureID()` | ADD METHOD | (none) | `uint32_t getTextureID() const` |
| `setTexture()` | ADD METHOD | (none) | `void setTexture(uint32_t id, const std::string& path)` |
| `getTexturePath()` | ADD METHOD | (none) | `const std::string& getTexturePath() const` |

**File**: `src/scene/primitive.cpp`
| Symbol | Change | Current State | Required State |
|--------|--------|---------------|----------------|
| `generateGeometry()` | MODIFY | Lines 27-67: generates m_vertices + m_indices only | Also generates m_texCoords (2 floats per vertex) |
| `uploadGPU()` | MODIFY | Lines 69-89: creates VAO/VBO/EBO, sets attrib 0 | Also creates UV VBO, sets attrib 1 |
| `~Primitive()` | MODIFY | Lines 13-19: deletes VAO/VBO/EBO | Also deletes texture (if m_textureID != 0) |
| `bind()` | MODIFY | Lines 91-93: glBindVertexArray | Also glBindTexture (if m_textureID != 0) |

**Verification**:
- generateGeometry() switch statement at line 28 — each case must add UV coords
- uploadGPU() attrib pointer at line 84 — location 0 is position, need location 1 for UV
- Destructor at line 14 — check m_uploaded, need to also check m_textureID
- bind() at line 92 — only binds VAO, need to also bind texture

---

### ARCHITECTURE CHANGE 2: Texture Resource Management

**Architecture Level**: Centralized texture loading and GPU management.

**File**: `include/resource/texture_manager.h` (NEW)
| Symbol | Type | Purpose |
|--------|------|---------|
| `TextureManager` | class | Manages texture lifecycle |
| `loadTexture(path)` | method | Load image → GPU texture |
| `releaseTexture(id)` | method | Delete GPU texture |
| `bindTexture(id, unit)` | method | Bind to texture unit |
| `clear()` | method | Delete all textures |
| `m_textures` | field | `unordered_map<uint32_t, TextureEntry>` |
| `m_nextID` | field | `uint32_t` auto-increment |

**File**: `src/resource/texture_manager.cpp` (NEW)
| Symbol | Purpose |
|--------|---------|
| `TextureManager::loadTexture()` | stb_image load → glTexImage2D |
| `TextureManager::releaseTexture()` | glDeleteTextures |
| `TextureManager::bindTexture()` | glActiveTexture + glBindTexture |
| `TextureManager::clear()` | Delete all, clear map |

**Verification**: New file — no existing symbols to verify against.

---

### ARCHITECTURE CHANGE 3: Shader Extension

**Architecture Level**: Vertex shader passes UV, fragment shader samples texture.

**File**: `src/renderer/renderer.cpp`
| Symbol | Change | Current State | Required State |
|--------|--------|---------------|----------------|
| `vertexShaderSource` | MODIFY | Lines 13-22: only aPos + MVP | Add `layout(location=1) in vec2 aTexCoord;` + `out vec2 vTexCoord;` |
| `fragmentShaderSource` | MODIFY | Lines 24-31: only uColor | Add `in vec2 vTexCoord;` + `uniform sampler2D uTexture;` + `uniform bool uHasTexture;` |
| `renderScene()` | MODIFY | Lines 120-148: sets uColor only | Also gets hasTextureLoc, textureLoc uniforms; binds texture if present |

**Verification**:
- vertexShaderSource at line 13 — current has location 0 only
- fragmentShaderSource at line 24 — current has uColor only
- renderScene() at line 126 — gets uColor uniform location, need to also get uHasTexture, uTexture

---

### ARCHITECTURE CHANGE 4: Bridge Command Extension

**Architecture Level**: External controller can assign textures.

**File**: `include/bridge/command_queue.h`
| Symbol | Change | Current State | Required State |
|--------|--------|---------------|----------------|
| `CMD_SET_TEXTURE` | ADD ENUM | CMD_HEALTH=17 is last | CMD_SET_TEXTURE=18 |
| `Command::texturePath` | ADD FIELD | (none) | `char[256]` |

**File**: `src/bridge/bridge.cpp`
| Symbol | Change | Current State | Required State |
|--------|--------|---------------|----------------|
| `parseCommand()` | MODIFY | Lines 63-204: parses 18 command types | Add "set_texture" parser |
| `executeCommand()` | MODIFY | Lines 215-395: 18-case switch | Add CMD_SET_TEXTURE case |

**Verification**:
- parseCommand() at line 63 — hand-rolled JSON parser, need to add new string match
- executeCommand() at line 215 — switch statement, need to add new case

---

### ARCHITECTURE CHANGE 5: Serialization Extension

**Architecture Level**: Texture references persist in .hsc files.

**File**: `src/scene/scene_builder.cpp`
| Symbol | Change | Current State | Required State |
|--------|--------|---------------|----------------|
| `exportHSC()` | MODIFY | Lines 286-338: writes position/rotation/scale/color | Also writes texCoords array + texture path |
| `importState()` | MODIFY | Lines 136-237: reads position/rotation/scale/color | Also reads texCoords + texture |

**Verification**:
- exportHSC() at line 310 — loop writes per-primitive JSON, add texCoords + texture
- importState() at line 181 — reads per-primitive JSON, add texCoords + texture parsing

---

## Complete Symbol Change Matrix

| File | Symbol | Action | Lines | Dependencies |
|------|--------|--------|-------|-------------|
| vec2.h | Vec2 | NEW | NEW FILE | (none) |
| vec2.cpp | Vec2 impl | NEW | NEW FILE | vec2.h |
| primitive.h | m_texCoords | ADD | ~42 | vec2.h |
| primitive.h | m_textureID | ADD | ~44 | (none) |
| primitive.h | m_texturePath | ADD | ~47 | (none) |
| primitive.h | getTexCoords() | ADD | ~50 | (none) |
| primitive.h | getTextureID() | ADD | ~51 | (none) |
| primitive.h | setTexture() | ADD | ~52 | (none) |
| primitive.h | getTexturePath() | ADD | ~53 | (none) |
| primitive.cpp | generateGeometry() | MODIFY | 27-67 | vec2.h |
| primitive.cpp | uploadGPU() | MODIFY | 69-89 | (none) |
| primitive.cpp | ~Primitive() | MODIFY | 13-19 | (none) |
| primitive.cpp | bind() | MODIFY | 91-93 | (none) |
| texture_manager.h | TextureManager | NEW | NEW FILE | (none) |
| texture_manager.cpp | TextureManager impl | NEW | NEW FILE | texture_manager.h, stb_image |
| renderer.cpp | vertexShaderSource | MODIFY | 13-22 | (none) |
| renderer.cpp | fragmentShaderSource | MODIFY | 24-31 | (none) |
| renderer.cpp | renderScene() | MODIFY | 120-148 | texture_manager.h |
| command_queue.h | CMD_SET_TEXTURE | ADD | ~29 | (none) |
| command_queue.h | Command::texturePath | ADD | ~38 | (none) |
| bridge.cpp | parseCommand() | MODIFY | 63-204 | (none) |
| bridge.cpp | executeCommand() | MODIFY | 215-395 | texture_manager.h |
| scene_builder.cpp | exportHSC() | MODIFY | 286-338 | (none) |
| scene_builder.cpp | importState() | MODIFY | 136-237 | (none) |
| CMakeLists.txt | hse_core sources | MODIFY | 11-20 | (none) |

**Total**: 2 new files, 9 modified files, 25 symbol changes.
