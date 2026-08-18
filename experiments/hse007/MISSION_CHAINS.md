# HSE-007 Architecture-to-Symbol Mission Chains

## Concept

An "architecture-to-symbol mission chain" traces a high-level architectural goal down to the exact symbols that must change. This is what separates "I understand the architecture" from "I can implement the feature."

## Mission Chain Template

```
ARCHITECTURE LEVEL: What needs to change conceptually
├── FILE LEVEL: Which files are affected
│   └── SYMBOL LEVEL: Which specific symbols change
└── VERIFICATION: How to confirm the chain is complete
```

---

## Chain 1: Add Texture Mapping

### Architecture Level
- Primitives need UV coordinates
- Shaders need texture sampling
- Renderer needs texture binding
- SceneBuilder needs texture serialization

### File Level
- `include/scene/primitive.h` — add `m_texCoords` field
- `src/scene/primitive.cpp` — add UV generation, modify `uploadGPU()`
- `src/renderer/renderer_state.cpp` — modify shader source strings
- `include/renderer/renderer.h` — add texture loading methods
- `src/renderer/renderer.cpp` — add texture binding
- `src/scene/scene_builder.cpp` — add texture to JSON serialization

### Symbol Level
| Symbol | Change | File:Line |
|--------|--------|-----------|
| `Primitive::m_texCoords` | ADD | primitive.h:~30 |
| `Primitive::generateGeometry()` | MODIFY — add UV generation | primitive.cpp:~60 |
| `Primitive::uploadGPU()` | MODIFY — add UV VBO | primitive.cpp:~130 |
| `vertex_shader_source` | MODIFY — add aTexCoord input | renderer_state.cpp:~20 |
| `fragment_shader_source` | MODIFY — add texture sampling | renderer_state.cpp:~38 |
| `Renderer::RendererState::m_program` | MODIFY — link UV attribute | renderer.cpp:~12 |
| `SceneBuilder::exportHSC()` | MODIFY — serialize texCoords | scene_builder.cpp:~350 |
| `SceneBuilder::importState()` | MODIFY — deserialize texCoords | scene_builder.cpp:~200 |

### Verification
- Can enumerate every symbol that needs to change ✓
- Can explain why each change is needed ✓
- Can predict the order of changes (header → source → shader → serializer) ✓
- Can identify dependencies (UV VBO depends on UV generation) ✓

---

## Chain 2: Add Point Lighting

### Architecture Level
- Scene needs light objects
- Shaders need light uniforms
- Primitive normals need to be used
- Renderer needs to bind light data

### File Level
- `include/scene/light.h` — NEW FILE: Light class
- `include/scene/scene.h` — add `m_lights` vector
- `src/renderer/renderer_state.cpp` — modify shaders for lighting
- `src/bridge/bridge.cpp` — add light commands
- `include/bridge/command_queue.h` — add light command types

### Symbol Level
| Symbol | Change | File:Line |
|--------|--------|-----------|
| `Light` | ADD CLASS | light.h:NEW |
| `Scene::m_lights` | ADD | scene.h:~30 |
| `Scene::addLight()` | ADD | scene.h:~60 |
| `vertex_shader_source` | MODIFY — pass normals | renderer_state.cpp:~20 |
| `fragment_shader_source` | MODIFY — Phong calculation | renderer_state.cpp:~38 |
| `Command::Type` | MODIFY — add LIGHT commands | command_queue.h:~10 |
| `Bridge::execute()` | MODIFY — handle light commands | bridge.cpp:~200 |

### Verification
- Can trace: "add light" → Scene needs lights → Shaders need uniforms → Bridge needs commands ✓
- Can identify that normals exist but are unused (m_normals in Primitive) ✓
- Can predict that lighting requires vertex shader to pass normals to fragment ✓

---

## Chain 3: Add Scene Hierarchy

### Architecture Level
- Flat vector of primitives → tree of nodes
- Transforms need to accumulate parent chain
- Scene needs root node concept
- JSON format needs parent references

### File Level
- `include/scene/scene_node.h` — NEW FILE: SceneNode class
- `include/scene/scene.h` — add hierarchy management
- `src/scene/primitive.cpp` — modify `getTransformationMatrix()`
- `src/scene/scene_builder.cpp` — modify JSON format
- `include/bridge/command_queue.h` — add hierarchy commands
- `src/bridge/bridge.cpp` — add hierarchy commands

### Symbol Level
| Symbol | Change | File:Line |
|--------|--------|-----------|
| `SceneNode` | ADD CLASS | scene_node.h:NEW |
| `SceneNode::m_parent` | ADD | scene_node.h:~15 |
| `SceneNode::m_children` | ADD | scene_node.h:~16 |
| `Scene::m_rootNode` | ADD | scene.h:~30 |
| `Primitive::getTransformationMatrix()` | MODIFY — accumulate parent | primitive.cpp:~180 |
| `SceneBuilder::exportHSC()` | MODIFY — serialize hierarchy | scene_builder.cpp:~350 |
| `SceneBuilder::importState()` | MODIFY — deserialize hierarchy | scene_builder.cpp:~200 |
| `Command::Type` | MODIFY — add HIERARCHY commands | command_queue.h:~10 |

### Verification
- Can trace: "parent/child" → SceneNode needs parent pointer → Matrix accumulation needs parent chain → JSON needs parent IDs ✓
- Can identify that `findByID()` works with flat map — needs to traverse hierarchy ✓
- Can predict that `removePrimitiveByID()` needs to handle children ✓

---

## Chain 4: Add Animation System

### Architecture Level
- Need keyframe data structure
- Need interpolation math (lerp, slerp)
- Need timeline/clip concept
- Need update loop integration
- Need bridge commands for animation control

### File Level
- `include/animation/keyframe.h` — NEW FILE
- `include/animation/clip.h` — NEW FILE
- `include/animation/animator.h` — NEW FILE
- `include/scene/scene.h` — add animation list
- `src/core/main.cpp` — add animation update in loop
- `include/bridge/command_queue.h` — add animation commands
- `src/bridge/bridge.cpp` — add animation commands

### Symbol Level
| Symbol | Change | File:Line |
|--------|--------|-----------|
| `Keyframe` | ADD CLASS | keyframe.h:NEW |
| `AnimationClip` | ADD CLASS | clip.h:NEW |
| `Animator` | ADD CLASS | animator.h:NEW |
| `Scene::m_animations` | ADD | scene.h:~30 |
| `main()` | MODIFY — add animation update | main.cpp:~40 |
| `Command::Type` | MODIFY — add ANIM commands | command_queue.h:~10 |

### Verification
- Can identify this requires NEW subsystem (not modification of existing) ✓
- Can predict that animation needs to update Primitive transforms per frame ✓
- Can identify that the update loop is in main.cpp, not bridge.cpp ✓

---

## Chain 5: Add Undo/Redo

### Architecture Level
- Need command history (already partially exists in Bridge::m_history)
- Need snapshot or inverse-command approach
- Scene revision counter already exists
- JSON serialization already exists

### File Level
- `include/bridge/command_history.h` — NEW FILE
- `src/bridge/bridge.cpp` — integrate history
- `include/bridge/command_queue.h` — add undo/redo commands

### Symbol Level
| Symbol | Change | File:Line |
|--------|--------|-----------|
| `CommandHistory` | ADD CLASS | command_history.h:NEW |
| `Bridge::m_history` | MODIFY — use CommandHistory | bridge.h:~40 |
| `Bridge::execute()` | MODIFY — record before execute | bridge.cpp:~200 |
| `Bridge::observeScene()` | REUSE — for snapshot approach | bridge.cpp:~400 |
| `Scene::m_revision` | REUSE — for delta detection | scene.h:~20 |

### Verification
- Can identify that snapshot approach leverages existing `observeScene()` ✓
- Can identify that `m_revision` enables efficient delta undo ✓
- Can predict that inverse commands are harder than snapshots ✓

---

## What Mission Chains Reveal

1. **Architecture understanding is necessary but not sufficient** — you must trace down to symbols
2. **Symbol grounding enables verification** — you can check if the plan is complete
3. **File-level plans miss details** — symbol-level plans catch missing changes
4. **Existing symbols constrain new features** — you must work with what exists
5. **The hardest part is knowing what ALREADY EXISTS** — not what needs to be created
