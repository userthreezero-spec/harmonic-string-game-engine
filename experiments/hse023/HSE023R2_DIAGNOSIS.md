# HSE-023R2 — Diagnosis

## Failure Observations

### 1. Selection Unreliability
- **Observation**: Clicking the table often selects the background wall or another object. Pressing 'G' jumps to the wrong root.
- **Hypothesis**: The picking ray is mathematically offset due to an incorrect matrix multiplication order in `screenToRay`.
- **Evidence**: `Picker::screenToRay` uses `(P * V).inverse()`. In a row-vector engine, the clip-space projection is `v_world * V * P`. The inverse of `(V * P)` is `P^-1 * V^-1`. The current code produces `V^-1 * P^-1`. This results in a ray that is rotated/translated incorrectly relative to the camera view.
- **Root Cause**: Matrix order discrepancy between `computeWorldMatrix` (corrected to row-major) and `screenToRay` (left as column-major).

### 2. Exploded View Failures
- **Observation**: Exploding the table displaces sub-groups but not internal parts consistently. Restoration is flaky if selection changes.
- **Hypothesis**: The explosion factor is not inherited or accumulated.
- **Evidence**: `Primitive::computeWorldMatrix` only looks at `parentPtr->getExplosionFactor()`. If a leaf primitive has a parent group that is NOT the selection target, it receives 0 explosion even if a grandparent is exploded.
- **Root Cause**: Lack of recursive explosion factor propagation.

### 3. Selection Root Resolution
- **Observation**: Pressing 'G' selects the `ROOM` instead of the `TABLE`.
- **Hypothesis**: The resolution logic is too greedy.
- **Root Cause**: `while(parent)` loop in `workspace_main.cpp` ignores semantic boundaries and always seeks the absolute root.

## Proposed Corrections

### 1. Correct Picking Math
- Change `Picker::screenToRay` to use `(V * P).inverse()`.
- Implement an integration test `hse_test_interaction` that verifies mouse clicking on a known object using a real Camera.

### 2. Correct Explosion Propagation
- Update `Primitive::computeWorldMatrix` to inherit the explosion factor from the parent if its own factor is 0.
- Ensure the displacement is applied relative to the parent's local space to maintain assembly coherence.

### 3. Refine Assembly Resolution
- Modify 'G' logic to select the "highest non-root" parent or provide a stepped traversal.

## Confidence: HIGH
The reported interaction defects are classic symptoms of projection order mismatch and non-recursive state propagation.
