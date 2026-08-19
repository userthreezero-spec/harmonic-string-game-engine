# HSE-023 — Repository Archaeology

## Current State Assessment

### 1. Scene & Primitives (PROVEN)
- `hse::Primitive`: Found in `include/scene/primitive.h`.
- **Properties**: `id`, `name`, `type` (Triangle, Quad, Cube, Sphere), `position`, `rotation`, `scale`, `rotation_speed`.
- **Hierarchy**: `Primitive` has `m_parent` (weak_ptr) and `m_children` (vector of shared_ptr).
- **Transform**: `m_worldMatrix` is computed in `computeWorldMatrix()`.
- **Defect**: The current multiplication order in `computeWorldMatrix` is `S * R * T` for the local matrix. In a column-vector engine (GLSL `M * v`), this means Translation happens *before* Scale/Rotate if we treat `operator*` as standard. Wait, if `M = S * R * T`, then `M * v = S * (R * (T * v))`. This is incorrect for local transforms (it scales the world position). Correct order should be `T * R * S`.

### 2. Hierarchy Implementation (PROVEN)
- `Primitive::setParent(parent)`: Handles bidirectional linking (adds to `m_children`, removes from old parent).
- `Scene::reparent(childID, parentID)`: Exposes hierarchy manipulation to the scene level.
- `Scene::getRoots()`: Returns primitives with no parent.
- `Scene::computeAllWorldMatrices()`: Traverses roots to update the entire tree.

### 3. Rendering (PROVEN)
- `Renderer::renderScene`: Iterates over ALL primitives in the scene's flat list.
- **Assembly Issue**: If we have an "Assembly" object that is just a logical group with no geometry, the renderer will currently attempt to draw it (which might fail or do nothing if it has no vertices, but `Primitive` always generates some geometry based on type).

### 4. Persistence (PROVEN)
- `SceneBuilder::importState`: Can parse a `"parent"` string field to reconstruct hierarchy.
- `SceneBuilder::exportHSC`: Can write a `"parent"` field (parent's name).
- **Observation**: `room.hsc` currently lacks parent-child relationships for the table and chairs. They are stored as a flat list.

### 5. Interaction (PROVEN)
- `Picker`: Performs ray-casting. It is "Transform-Aware" (uses `worldMatrix.inverse()`).
- **Limitation**: Selecting a leg only selects that leg. There is no concept of "Select Assembly".

## Identified Capability Gaps
- **Assembly Abstraction**: No explicit `Assembly` or `Group` type.
- **Aggregate Bounds**: `BoundingBox` is per-primitive, not aggregate.
- **Structural Truth**: Structural relationships are implicit in the scene graph but not exposed as "Object Definitions".
- **Exploded View**: Not implemented.

## Verification of Multiplication Order
HSE-022 changed `computeWorldMatrix` to `S * R * T`.
In `mat4.cpp`: `translate` sets `data[12..14]`. This is 4th column for `M * v`.
If `M = T * R * S`, then `M * v = T * (R * (S * v))`. This scales, then rotates, then translates. Correct.
If `M = S * R * T`, then `M * v = S * (R * (T * v))`. This translates, then rotates, then scales. Incorrect.
**Plan**: Re-verify this with a test case in Implementation Phase.
