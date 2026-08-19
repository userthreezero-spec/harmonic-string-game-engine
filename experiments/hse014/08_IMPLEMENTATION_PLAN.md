# HSE-014 Implementation Plan

## Step 1: Primitive Header (include/scene/primitive.h)
Add fields and methods:
- `#include <memory>` (already present)
- `std::weak_ptr<Primitive> m_parent;` (NEW field)
- `std::vector<std::shared_ptr<Primitive>> m_children;` (NEW field)
- `Mat4 m_worldMatrix;` (NEW field)
- `void setParent(std::shared_ptr<Primitive> parent);` (NEW method)
- `std::shared_ptr<Primitive> getParent() const;` (NEW getter)
- `const std::vector<std::shared_ptr<Primitive>>& getChildren() const;` (NEW getter)
- `const Mat4& getWorldMatrix() const;` (NEW getter)
- `void computeWorldMatrix();` (NEW method)
- `void addChild(std::shared_ptr<Primitive> child);` (NEW method)
- `void removeChild(uint64_t childID);` (NEW method)

## Step 2: Primitive Implementation (src/scene/primitive.cpp)
Implement:
- `setParent()`: removes self from old parent's children, adds to new parent's children, sets m_parent
- `getParent()`: returns m_parent.lock()
- `getChildren()`: returns m_children
- `getWorldMatrix()`: returns m_worldMatrix
- `computeWorldMatrix()`: local matrix = translate * rotate * scale; if parent, m_worldMatrix = parent->m_worldMatrix * local; else m_worldMatrix = local
- `addChild()`: pushes to m_children, sets child's m_parent to this
- `removeChild()`: erases from m_children, clears child's m_parent

## Step 3: Scene Header (include/scene/scene.h)
Add methods:
- `bool reparent(uint64_t childID, uint64_t parentID);` (NEW)
- `std::vector<std::shared_ptr<Primitive>> getRoots() const;` (NEW)
- `void computeAllWorldMatrices();` (NEW)

## Step 4: Scene Implementation (src/scene/scene.cpp)
Implement:
- `reparent()`: find child and parent by ID, call child->setParent(parent)
- `getRoots()`: returns primitives where getParent() == nullptr
- `computeAllWorldMatrices()`: get roots, for each root call computeWorldMatrix() recursively through children

## Step 5: Renderer (src/renderer/renderer.cpp)
Modify `renderScene()`:
- Before render loop: `scene.computeAllWorldMatrices()`
- In render loop: use `prim->getWorldMatrix()` instead of computing local model matrix

## Step 6: Bridge (include/bridge/command_queue.h + src/bridge/bridge.cpp)
- Add `CMD_REPARENT` to Command::Type enum
- Add `reparent` command parsing in parseCommand()
- Add `CMD_REPARENT` case in executeCommand()
- Add "parent" field to observation JSON

## Step 7: SceneBuilder (src/scene/scene_builder.cpp)
- exportHSC(): add "parent" field to object serialization (parent's name)
- importState(): parse "parent" field, establish hierarchy after all primitives created
- exportState(): add "parent" field

## Step 8: Tests (tests/test_hierarchy.cpp + CMakeLists.txt)
Create test_hierarchy.cpp:
- test_parent_child_creation
- test_world_matrix_root
- test_world_matrix_child
- test_reparent
- test_get_roots
- test_compute_all_world_matrices
- test_remove_child_on_parent_delete
- test_multi_level_hierarchy

## Step 9: Demo (optional: update room_demo or material_demo to show hierarchy)

## Files Modified
1. include/scene/primitive.h
2. src/scene/primitive.cpp
3. include/scene/scene.h
4. src/scene/scene.cpp
5. src/renderer/renderer.cpp
6. include/bridge/command_queue.h
7. src/bridge/bridge.cpp
8. src/scene/scene_builder.cpp
9. CMakeLists.txt

## Files NOT Modified
- include/renderer/material.h
- src/renderer/material.cpp
- include/renderer/texture.h
- src/renderer/texture.cpp
- include/core/window.h
- src/core/window.cpp
- include/math/vec3.h
- src/math/vec3.cpp
- include/math/mat4.h
- src/math/mat4.cpp
- include/bridge/pipe_transport.h
- src/bridge/pipe_transport.cpp
- include/bridge/bridge.h
- src/main.cpp
- src/bridge_main.cpp
- src/workspace_main.cpp
