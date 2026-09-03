# HSE-023R2 — Repository Archaeology

## Current Subsystem State

### 1. Transform Convention (HSE-023 Correction)
- **Verified Order**: `v_world = v_local * M_local * M_parent` (Row-Vector).
- **Local Matrix**: `M_local = S * R * T`.
- **Implementation**: Found in `Primitive::computeWorldMatrix`. Corrected in HSE-023.

### 2. Picking Pipeline
- **Class**: `hse::Picker`.
- **Screen-to-Ray**: `Picker::screenToRay` uses `(P * V).inverse()`. 
- **Observation**: For row-vector vertex projection `v_clip = v_world * V * P`, the inverse should be `(V * P).inverse()`. The current implementation follows column-major logic (`P * V`) in a row-major engine.
- **Intersection**: `intersectPrimitive` transforms the ray into local space using `worldMatrix.inverse()`. This is correct if `worldMatrix` itself is correct.

### 3. Hierarchical Selection
- **Workspace Logic**: `workspace_main.cpp` handles Key 'G'.
- **Algorithm**: Iteratively calls `getParent()` until `nullptr`.
- **Defect**: Always resolves to the absolute scene root (e.g., `ROOM`). Does not distinguish between intermediate functional assemblies (e.g., `TABLE`).

### 4. Exploded View
- **State**: `m_explosionFactor` stored on `Primitive`.
- **Math**: `effectivePos = m_position * (1.0f + parent->getExplosionFactor())`.
- **Defect**: Only checks the immediate parent. In nested assemblies, the displacement is not cumulative or properly propagated to leaf primitives if an intermediate group is not "exploded".

### 5. Existing Tests
- `test_picking.cpp`: Uses manually constructed rays. Does not test `screenToRay` projection math.
- `test_assembly.cpp`: Verifies hierarchy math but not interaction state.
- **Gap**: No automated integration test for mouse-to-object selection.
