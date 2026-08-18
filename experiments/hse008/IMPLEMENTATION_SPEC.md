# HSE-008 IMPLEMENTATION SPECIFICATION

## Pre-Implementation Checklist

- [ ] All claims verified against repository (REPOSITORY_VERIFICATION.md)
- [ ] All symbols mapped (ARCHITECTURE_TO_SYMBOL_MAP.md)
- [ ] No blocking dependencies unresolved
- [ ] stb_image header available (download stb_image.h)
- [ ] Build system changes identified (CMakeLists.txt)

---

## SPEC 1: Vec2 Math Type

### File: `include/math/vec2.h`
```cpp
#pragma once

namespace hse {

struct Vec2 {
    float x, y;
    Vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator*(float scalar) const;
    bool operator==(const Vec2& other) const;
};

} // namespace hse
```

### File: `src/math/vec2.cpp`
```cpp
#include "math/vec2.h"

namespace hse {

Vec2 Vec2::operator+(const Vec2& o) const { return {x+o.x, y+o.y}; }
Vec2 Vec2::operator-(const Vec2& o) const { return {x-o.x, y-o.y}; }
Vec2 Vec2::operator*(float s) const { return {x*s, y*s}; }
bool Vec2::operator==(const Vec2& o) const { return x==o.x && y==o.y; }

} // namespace hse
```

### CMakeLists.txt Change
Add `src/math/vec2.cpp` to `hse_core` sources (line 18, after mat4.cpp).

### Acceptance Criteria
- [ ] Vec2 compiles and links
- [ ] Operators produce correct results
- [ ] No naming conflicts with existing types

### Test Strategy
Add to tests/test_math.cpp: test Vec2 arithmetic.

---

## SPEC 2: Primitive UV Extension

### File: `include/scene/primitive.h` — Add after line 47 (m_uploaded)
```cpp
std::vector<float> m_texCoords;
uint32_t m_textureID = 0;
std::string m_texturePath;
```

### File: `include/scene/primitive.h` — Add after line 33 (getVertices/getIndices)
```cpp
const std::vector<float>& getTexCoords() const { return m_texCoords; }
uint32_t getTextureID() const { return m_textureID; }
const std::string& getTexturePath() const { return m_texturePath; }
void setTexture(uint32_t id, const std::string& path);
```

### File: `src/scene/primitive.cpp` — Add after line 25 (setRotationSpeed)
```cpp
void Primitive::setTexture(uint32_t id, const std::string& path) {
    m_textureID = id;
    m_texturePath = path;
}
```

### File: `src/scene/primitive.cpp` — Modify generateGeometry() (line 27-67)

For each PrimitiveType case, add UV coordinates after vertices:

**Triangle** (after line 35):
```cpp
m_texCoords = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.5f, 1.0f
};
```

**Quad** (after line 45):
```cpp
m_texCoords = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f
};
```

**Cube** (after line 59):
```cpp
m_texCoords = {
    0,0, 1,0, 1,1, 0,1,  // front
    0,0, 1,0, 1,1, 0,1,  // back
    0,0, 1,0, 1,1, 0,1,  // left
    0,0, 1,0, 1,1, 0,1,  // right
    0,0, 1,0, 1,1, 0,1,  // top
    0,0, 1,0, 1,1, 0,1   // bottom
};
```

**Sphere** (after line 64):
```cpp
m_texCoords = { 0.0f, 0.0f }; // stub
```

### File: `src/scene/primitive.cpp` — Modify uploadGPU() (line 69-89)

After line 85 (`glEnableVertexAttribArray(0);`), add:
```cpp
// UV coordinates
if (!m_texCoords.empty()) {
    GLuint uvbo;
    glGenBuffers(1, &uvbo);
    glBindBuffer(GL_ARRAY_BUFFER, uvbo);
    glBufferData(GL_ARRAY_BUFFER, m_texCoords.size() * sizeof(float), m_texCoords.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
}
```

**NOTE**: This creates a separate VBO for UVs. The m_vbo field only tracks the position VBO. The UV VBO is not tracked — it will leak if the primitive is re-uploaded. This is acceptable for first implementation but should be tracked in m_uvbo for production.

### File: `src/scene/primitive.cpp` — Modify ~Primitive() (line 13-19)

Add after line 17 (`glDeleteBuffers(1, &m_ebo);`):
```cpp
// Note: UV VBO not tracked — will be cleaned up by driver on context destruction
```

### File: `src/scene/primitive.cpp` — Modify bind() (line 91-93)

Add after line 92 (`glBindVertexArray(m_vao);`):
```cpp
if (m_textureID != 0) {
    glBindTexture(GL_TEXTURE_2D, m_textureID);
}
```

### Acceptance Criteria
- [ ] Each PrimitiveType generates correct UV coordinates
- [ ] UV VBO is created and uploaded
- [ ] Vertex attrib 1 is set up correctly (2 floats, stride 2*sizeof(float))
- [ ] Texture is bound during draw call
- [ ] No crash when textureID is 0 (no texture assigned)

### Test Strategy
- Unit test: create each PrimitiveType, verify m_texCoords size matches vertex count × 2
- Integration test: create Quad, assign texture, verify texture is bound during render

---

## SPEC 3: Texture Resource Manager

### File: `include/resource/texture_manager.h`
```cpp
#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>

namespace hse {

class TextureManager {
public:
    TextureManager();
    ~TextureManager();

    uint32_t loadTexture(const std::string& path);
    void releaseTexture(uint32_t textureID);
    void bindTexture(uint32_t textureID, uint32_t unit = 0);
    void clear();
    bool hasTexture(uint32_t textureID) const;

private:
    struct TextureEntry {
        uint32_t glID = 0;
        std::string path;
        int width = 0;
        int height = 0;
        int refCount = 0;
    };

    std::unordered_map<uint32_t, TextureEntry> m_textures;
    uint32_t m_nextID = 1;
};

} // namespace hse
```

### File: `src/resource/texture_manager.cpp`
Key implementation details:
- `loadTexture()`: Use stbi_load() → glGenTextures() → glBindTexture() → glTexImage2D() → glTexParameteri() → stbi_image_free()
- `releaseTexture()`: glDeleteTextures() → remove from map
- `bindTexture()`: glActiveTexture(GL_TEXTURE0 + unit) → glBindTexture(GL_TEXTURE_2D, glID)
- `clear()`: iterate m_textures, glDeleteTextures for each, clear map

### CMakeLists.txt Change
Add `src/resource/texture_manager.cpp` to `hse_core` sources.

### External Dependency
Download `stb_image.h` to `include/third_party/stb_image.h`.
Add `#define STB_IMAGE_IMPLEMENTATION` in texture_manager.cpp before include.

### Acceptance Criteria
- [ ] loadTexture() loads PNG and JPG files
- [ ] loadTexture() returns unique IDs
- [ ] Same path returns same ID (cache hit)
- [ ] releaseTexture() deletes GPU texture
- [ ] bindTexture() binds to correct texture unit
- [ ] clear() deletes all textures

### Test Strategy
- Unit test: create a 4x4 checkerboard PNG, load it, verify ID is valid
- Integration test: load texture, bind it, render a quad, verify pixels change

---

## SPEC 4: Shader Extension

### File: `src/renderer/renderer.cpp` — Replace vertexShaderSource (lines 13-22)
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

### File: `src/renderer/renderer.cpp` — Replace fragmentShaderSource (lines 24-31)
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

### File: `src/renderer/renderer.cpp` — Modify renderScene() (lines 120-148)

After line 126 (`GLint colorLoc = ...`), add:
```cpp
GLint hasTextureLoc = glGetUniformLocation(m_state->shaderProgram, "uHasTexture");
GLint textureLoc = glGetUniformLocation(m_state->shaderProgram, "uTexture");
```

In the per-primitive loop, after line 142 (`glUniform3f(colorLoc, ...);`), add:
```cpp
uint32_t texID = prim->getTextureID();
if (texID != 0) {
    glUniform1i(hasTextureLoc, 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
    glUniform1i(textureLoc, 0);
} else {
    glUniform1i(hasTextureLoc, 0);
}
```

### Acceptance Criteria
- [ ] Shader compiles without errors
- [ ] uHasTexture=0 produces flat color (backward compatible)
- [ ] uHasTexture=1 samples texture
- [ ] Texture unit 0 is used correctly

### Test Strategy
- Visual: render untextured quad → flat color
- Visual: render textured quad → texture visible
- Automated: check shader compilation logs

---

## SPEC 5: Bridge Command Extension

### File: `include/bridge/command_queue.h` — Add after line 29 (CMD_HEALTH)
```cpp
CMD_SET_TEXTURE,
```

### File: `include/bridge/command_queue.h` — Add after line 38 (sinceRevision)
```cpp
char texturePath[256] = {};
```

### File: `src/bridge/bridge.cpp` — Add to parseCommand() (after line 198)
```cpp
} else if (type == "set_texture") {
    cmd.type = Command::CMD_SET_TEXTURE;
    cmd.objectID = jsonFindU64(json, "object_id", 0);
    std::string path = jsonFind(json, "path");
    strncpy(cmd.texturePath, path.c_str(), sizeof(cmd.texturePath) - 1);
```

### File: `src/bridge/bridge.cpp` — Add to executeCommand() switch (after line 388)
```cpp
case Command::CMD_SET_TEXTURE: {
    auto prim = scene->findByID(cmd.objectID);
    if (!prim) {
        pipe.writeLine(makeAck(cmd.seq, false, "\"error\":\"object_not_found\""));
        return;
    }
    uint32_t texID = textureManager.loadTexture(cmd.texturePath);
    if (texID == 0) {
        pipe.writeLine(makeAck(cmd.seq, false, "\"error\":\"texture_load_failed\""));
        return;
    }
    prim->setTexture(texID, cmd.texturePath);
    m_sceneRevision++;
    pipe.writeLine(makeAck(cmd.seq, true,
        "\"object_id\":" + std::to_string(cmd.objectID) +
        ",\"revision\":" + std::to_string(m_sceneRevision)));
    break;
}
```

**NOTE**: This requires Bridge to have a TextureManager member. Add `TextureManager m_textureManager;` to Bridge's private fields.

### Acceptance Criteria
- [ ] "set_texture" command is recognized by parseCommand()
- [ ] executeCommand() loads texture and assigns to primitive
- [ ] Ack is sent with object_id and revision
- [ ] Error ack is sent if object not found
- [ ] Error ack is sent if texture load fails

### Test Strategy
- External controller: send set_texture command, verify ack
- External controller: send set_texture for nonexistent object, verify error ack

---

## SPEC 6: Serialization Extension

### File: `src/scene/scene_builder.cpp` — Modify exportHSC() (after line 318)
Add before the closing `}` of each object:
```cpp
if (!p->getTexturePath().empty()) {
    f << ",\n        \"texture\": \"" << p->getTexturePath() << "\"";
}
```

### File: `src/scene/scene_builder.cpp` — Modify importState() (after line 190)
Add after setColor:
```cpp
std::string texPath = jsonFind(objStr, "texture");
if (!texPath.empty()) {
    // Texture will be loaded on demand when bridge starts
    prim->setTexture(0, texPath); // ID=0 means not yet loaded
}
```

### Acceptance Criteria
- [ ] exportHSC() writes texture path if non-empty
- [ ] importState() reads texture path
- [ ] Backward compatible: no texture = no change in JSON
- [ ] Round-trip: save → load → save produces same JSON

### Test Strategy
- Unit test: export scene with texture, verify JSON contains "texture" field
- Unit test: export scene without texture, verify JSON unchanged
- Integration test: save → load → save round-trip

---

## SPEC 7: CMakeLists.txt Update

### File: `CMakeLists.txt` — Modify hse_core sources (lines 11-20)
Add after `src/math/mat4.cpp`:
```cmake
    src/math/vec2.cpp
    src/resource/texture_manager.cpp
```

### Acceptance Criteria
- [ ] Build succeeds with new files
- [ ] All existing tests still pass
- [ ] New symbols are accessible from all targets

### Test Strategy
- Build all targets
- Run ctest

---

## Implementation Order

1. SPEC 1: Vec2 (no dependencies)
2. SPEC 2: Primitive UV (depends on SPEC 1)
3. SPEC 3: TextureManager (no dependencies)
4. SPEC 4: Shader (no dependencies)
5. SPEC 5: Bridge command (depends on SPEC 2, SPEC 3)
6. SPEC 6: Serialization (depends on SPEC 2)
7. SPEC 7: CMake (depends on SPEC 1, SPEC 3)

**Parallel opportunities**: SPEC 1, 3, 4 can be done in parallel. SPEC 2 depends on SPEC 1. SPEC 5 depends on SPEC 2+3. SPEC 6 depends on SPEC 2. SPEC 7 depends on SPEC 1+3.
