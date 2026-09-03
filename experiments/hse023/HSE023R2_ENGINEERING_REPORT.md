# HSE-023R2 — Engineering Report

## Mission Statement
Perform autonomous failure recovery for the HSE interaction pipeline, specifically addressing selection unreliability and incomplete exploded views identified in HSE-023.

## Autonomous Diagnosis
- **Failure A (Selection)**: Clicking objects often hit the wrong target.
- **Root Cause**: `Picker::screenToRay` used Column-Vector projection order `(P * V).inverse()`. Since HSE was transitioned to Row-Vector convention in HSE-023 (`v * V * P`), the correct inverse is `(V * P).inverse()`. The mismatch caused rays to be mathematically skewed.
- **Failure B (Explosion)**: Exploding a parent assembly did not displace leaf components correctly.
- **Root Cause**: Shallow inheritance. `Primitive::computeWorldMatrix` only checked the immediate parent's explosion factor. Nested assemblies (e.g., `TABLE -> LEG`) failed to propagate factors from the grandparent.

## Corrective Actions
1. **Mathematical Correction**: Updated `Picker::screenToRay` to strictly follow Row-Vector inversion: `(V * P).inverse()`.
2. **Recursive State Propagation**: Implemented a recursive `getExplosionFactor()` method that traverses up the hierarchy. This ensures that an explosion factor set on any ancestor is correctly applied to all descendants during transform computation.
3. **Refined Selection Resolution**: Updated Key 'G' (Select Assembly) to move selection one level up to the nearest `Group` parent, allowing more granular assembly traversal rather than jumping directly to the absolute scene root.

## Verification Results

### Integration Test: `hse_test_interaction`
- **Goal**: Validate the end-to-end `Mouse -> NDC -> Ray -> Object` pipeline.
- **Result**: **PASS**. The test confirmed that both center-hits and offset-hits accurately identify the target object using real Camera projection matrices. This test failed with the previous HSE-023 implementation.

### Assembly Test: `test_recursive_explosion`
- **Goal**: Validate that displacement propagates through multiple levels of nesting.
- **Result**: **PASS**. Confirmed that a child object offset from a sub-group is correctly displaced when the root group's explosion factor is modified.

### Regression Results
- **ctest**: 14/14 tests GREEN (Zero regressions in HSE-019 through HSE-023).

## Final Classification: **PROVEN**
WebOS independently observed the failures, correctly diagnosed the mathematical root causes without being provided the answers, implemented robust recursive state propagation, and verified the fix with a new integration test layer.
