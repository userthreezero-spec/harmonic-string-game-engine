# HSE-014 Repository Archaeology

## Current Symbol Inventory

### Primitive (include/scene/primitive.h)
- **Fields (14)**: m_id, m_name, m_type, m_position, m_rotation, m_scale, m_rotationSpeed, m_material, m_vertices, m_indices, m_vao, m_vbo, m_ebo, m_uploaded
- **Methods (20)**: constructor, destructor, getID, getName, setName, setPosition, setRotation, setScale, setRotationSpeed, setMaterial, getMaterial, getType, getPosition, getRotation, getScale, getRotationSpeed, getVertices, getIndices, getVAO/VBO/EBO, getIndexCount, uploadGPU, bind, unbind, nextID, generateGeometry
- **Missing**: parent pointer, children vector, world matrix, setParent, getChildren, getWorldMatrix, computeWorldMatrix

### Scene (include/scene/scene.h)
- **Fields (6)**: m_name, m_primitives, m_idIndex, m_cameras, m_activeCameraIndex, m_materials
- **Methods (14)**: constructor, destructor, getName, getPrimitiveCount, getCameraCount, addPrimitive, removePrimitive, removePrimitiveByID, update, findByID, findByName, findIndexByID, getPrimitives, addCamera, setActiveCamera, getActiveCamera, addMaterial, getMaterial, getMaterials, rebuildIDIndex
- **Missing**: reparent, getChildrenOf, getRoots, traverse

### Renderer (src/renderer/renderer.cpp)
- **renderScene()**: computes local model matrix per primitive, calls material->apply(), draws
- **Missing**: world matrix computation, hierarchy traversal

### Bridge (src/bridge/bridge.cpp)
- **Commands**: add_primitive, remove_primitive, set_position, set_rotation, set_scale, set_color, set_camera, set_clear_color, observe_*, save_project, load_project
- **Missing**: reparent, set_parent, get_world_transform, get_children

### SceneBuilder (src/scene/scene_builder.cpp)
- **importState()**: parses materials + objects from JSON
- **exportHSC()**: serializes materials + objects to JSON
- **Missing**: parent field in object serialization

### Camera (include/scene/camera.h)
- **Fields (8)**: m_type, m_position, m_target, m_up, m_fovDeg, m_nearPlane, m_farPlane, m_aspectRatio
- **Methods (11)**: constructor, setPosition, lookAt, setFOV, setNearPlane, setFarPlane, setAspectRatio, getProjectionType, getPosition, getTarget, getFOV, getNearPlane, getFarPlane, getAspectRatio, getViewMatrix, getProjectionMatrix, update

## Dependency Graph
```
Primitive → Material, Vec3, Mat4
Scene → Primitive, Camera, Material
Renderer → Scene, Camera, Primitive, Material, Vec3, Mat4
Bridge → Scene, Camera, Renderer, SceneBuilder, PipeServer, CommandQueue
SceneBuilder → Scene, Primitive, Camera, Material
```

## Files Affected by Transform Hierarchy
- include/scene/primitive.h (add parent/children/worldMatrix)
- src/scene/primitive.cpp (implement setParent, computeWorldMatrix)
- include/scene/scene.h (add hierarchy management)
- src/scene/scene.cpp (implement reparent, tree traversal)
- src/renderer/renderer.cpp (use world matrix)
- include/bridge/command_queue.h (add reparent command)
- src/bridge/bridge.cpp (add reparent command handling)
- src/scene/scene_builder.cpp (serialize parent field)
- tests/test_hierarchy.cpp (NEW: hierarchy tests)
- CMakeLists.txt (add hse_test_hierarchy)
