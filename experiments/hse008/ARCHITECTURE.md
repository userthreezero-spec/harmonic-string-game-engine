# HSE-008 ARCHITECTURE

## Texture Mapping Architecture for HSE

### System Diagram

```
┌─────────────────────────────────────────────────────────┐
│                    EXTERNAL CONTROLLER                   │
│  send: {"cmd":"set_texture","object_id":1,"path":"tex"} │
└──────────────────────┬──────────────────────────────────┘
                       │ Named Pipe (NDJSON)
┌──────────────────────┴──────────────────────────────────┐
│                     BRIDGE (Main Thread)                  │
│  parseCommand() → CMD_SET_TEXTURE                        │
│  executeCommand() →                                      │
│    Scene.findByID(1) → Primitive                         │
│    TextureManager.loadTexture("tex") → uint32_t texID    │
│    Primitive::setTexture(texID, "tex")                    │
└──────────────────────┬──────────────────────────────────┘
                       │
┌──────────────────────┴──────────────────────────────────┐
│                    PRIMITIVE (GPU State)                  │
│  m_vao, m_vbo, m_ebo (position)                          │
│  m_uvbo (UV coordinates) ← NEW                          │
│  m_textureID (GPU texture handle) ← NEW                  │
│  m_texCoords (UV data) ← NEW                             │
│  m_texturePath (serialization) ← NEW                     │
└──────────────────────┬──────────────────────────────────┘
                       │
┌──────────────────────┴──────────────────────────────────┐
│                    RENDERER (GL Thread)                   │
│  renderScene():                                          │
│    glUseProgram(shaderProgram)                           │
│    for each Primitive:                                   │
│      if (textureID != 0):                                │
│        glActiveTexture(GL_TEXTURE0)                      │
│        glBindTexture(GL_TEXTURE_2D, textureID)           │
│        glUniform1i(uHasTexture, 1)                       │
│      else:                                               │
│        glUniform1i(uHasTexture, 0)                       │
│      prim->bind() → glBindVertexArray + glBindTexture    │
│      glDrawElements(...)                                 │
└──────────────────────┬──────────────────────────────────┘
                       │
┌──────────────────────┴──────────────────────────────────┐
│                    GPU (OpenGL 3.3)                       │
│  Vertex Shader:                                          │
│    layout(location=0) in vec3 aPos                       │
│    layout(location=1) in vec2 aTexCoord ← NEW            │
│    out vec2 vTexCoord                                    │
│                                                          │
│  Fragment Shader:                                        │
│    in vec2 vTexCoord                                     │
│    uniform sampler2D uTexture ← NEW                      │
│    uniform bool uHasTexture ← NEW                        │
│    if (uHasTexture):                                     │
│      FragColor = texture(uTexture, vTexCoord) * color    │
│    else:                                                 │
│      FragColor = vec4(uColor, 1.0)                       │
└─────────────────────────────────────────────────────────┘
```

### Data Flow

```
1. LOAD: stb_image → PNG/JPG → RGBA pixels
2. UPLOAD: glTexImage2D → GPU texture memory
3. ASSIGN: Primitive.setTexture(texID, path)
4. BIND: glActiveTexture(GL_TEXTURE0) + glBindTexture
5. SAMPLE: fragment shader reads texture via UV coordinates
6. OUTPUT: FragColor = texture * color
7. SERIALIZE: exportHSC writes texture path to JSON
8. DESERIALIZE: importState reads texture path from JSON
```

### Resource Lifecycle

```
CREATE: TextureManager.loadTexture() → glGenTextures → glTexImage2D
USE:    Renderer::renderScene() → glBindTexture → shader samples
DELETE: Primitive::~Primitive() → glDeleteTextures (if owned)
        TextureManager::clear() → delete all on shutdown
```

### Thread Safety

```
READER THREAD:
  - PipeServer::readLine() (blocking)
  - Bridge::parseCommand() (CPU only)
  - CommandQueue::push() (lock-free)

MAIN (GL) THREAD:
  - CommandQueue::pop() (lock-free)
  - Bridge::executeCommand() → TextureManager.loadTexture() (GL calls)
  - Renderer::renderScene() (GL calls)
  - Primitive::bind() (GL calls)

SAFE because:
  - TextureManager is only accessed from main thread
  - Primitive GPU state is only modified from main thread
  - SPSC queue provides thread boundary
```

### Build System

```
hse_core (STATIC LIB):
  src/math/vec2.cpp ← NEW
  src/resource/texture_manager.cpp ← NEW
  src/core/window.cpp
  src/renderer/renderer.cpp
  src/scene/scene.cpp
  src/scene/camera.cpp
  src/scene/primitive.cpp
  src/scene/scene_builder.cpp
  src/math/vec3.cpp
  src/math/mat4.cpp

External: stb_image.h (single header, include/third_party/)
```
