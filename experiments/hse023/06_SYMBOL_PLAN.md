# HSE-023 — Symbol Plan

## Proposed Changes

### Subsystem: Math
#### [MODIFY] `include/math/bounding_box.h`
- `void merge(const BoundingBox& other)`: Update `min`/`max` to encompass both boxes.

### Subsystem: Scene
#### [MODIFY] `include/scene/primitive.h`
- `enum class PrimitiveType`: Add `Group`.
- `void setExplosionFactor(float f)`: NEW.
- `BoundingBox getAggregateBoundingBox() const`: NEW.

#### [MODIFY] `src/scene/primitive.cpp`
- `generateGeometry()`: Handle `Group` type (set empty vertices/indices).
- `computeWorldMatrix()`: Correct multiplication order to `T * R * S`. Implement `m_explosionFactor` logic for local translation.
- `getAggregateBoundingBox()`: Implement recursive merge.

### Subsystem: Renderer
#### [MODIFY] `src/renderer/renderer.cpp`
- `renderScene()`: Add check `if (prim->getType() == PrimitiveType::Group) continue;`.
- Update selection highlight logic: If the `selectedID` is a parent of the current primitive, set `uSelected = true`.

### Subsystem: Workspace
#### [MODIFY] `src/workspace_main.cpp`
- Add key command 'E' to toggle Exploded View on the selected assembly.
- Add key command 'G' to select the root assembly of the current selection.

### Subsystem: Persistence
#### [MODIFY] `src/scene/scene_builder.cpp`
- Update `buildRoom()` to use hierarchical grouping for `table` and `chairs`.
- Ensure `exportHSC` and `importState` handle `Group` types and parent links correctly.

## Verification Gate
### Predicted Symbols
- `hse::PrimitiveType::Group`
- `hse::Primitive::getAggregateBoundingBox`
- `uSelected` (multi-object propagation)
- `m_explosionFactor`
