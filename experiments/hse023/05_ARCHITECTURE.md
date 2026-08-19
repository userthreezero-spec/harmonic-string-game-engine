# HSE-023 — Architecture

## 1. Assembly Abstraction: `Group` Primitives
- **Model**: `PrimitiveType::Group` will be added to the engine.
- **Behavior**: A `Group` primitive has no geometry (empty vertex buffer) and is skipped by the `Renderer`'s draw call, but its `computeWorldMatrix()` continues to propagate transforms to its children.
- **Identity**: Assemblies are defined as a `Group` node at the top of a logical hierarchy.

## 2. Hierarchical Transform Semantics
- **Multiplication Order**: `M_local = T * R * S`.
- **Propagation**: `M_world = M_parent_world * M_local`.
- **Verification**: This order ensures that scaling and rotation occur around the object's origin before being translated to the world position, and that parent transformations correctly rotate/scale/move the entire assembly.

## 3. Aggregate Bounding Boxes
- **Method**: `Primitive::getAggregateBoundingBox()`.
- **Algorithm**:
  1. Initialize with `getBoundingBox()` (local bounds transformed to world).
  2. For each child, merge its `getAggregateBoundingBox()` into the result.
- **Purpose**: Enables collision detection and selection for entire assemblies.

## 4. Interaction: Assembly-Level Selection
- **Workspace Logic**:
  - `m_selectedID`: The ID of the currently selected node.
  - **Selection Rule**: Clicking a primitive selects that primitive. A secondary command (e.g. key 'G') "Goes to Parent" or selects the assembly root.
- **Highlighting**: If an Assembly is selected, the `Renderer` will be updated to optionally highlight all descendants.

## 5. Visual Inspection: Exploded View
- **Implementation**: `m_explosionFactor` (float, 0.0 to 1.0).
- **Effect**: During `computeWorldMatrix()`, if explosion is active, the child's local translation is multiplied by a scale factor relative to the parent center.
- **Constraint**: This is a visual-only override and must not be saved to `.hsc`.

## 6. Symbol-Level Changes

### `math/bounding_box.h`
- `merge(const BoundingBox& other)`: Combine two boxes.

### `scene/primitive.h`
- `PrimitiveType::Group` (NEW)
- `m_explosionFactor` (NEW)
- `getAggregateBoundingBox()` (NEW)

### `renderer/renderer.cpp`
- Skip draw elements for `Group` types.
- Propagate selection highlight to children.

### `scene/scene_builder.cpp`
- Reconstruct the HSE-019 Table as a proper hierarchical assembly in `buildRoom()`.
