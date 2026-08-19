# HSE-023 — Engineering Report

## Mission Statement
Evolve HSE to support structural assemblies, correct hierarchical transform semantics, and provide deep structural inspection capabilities.

## Repository Archaeology
- **Baseline**: HSE-022 (Multi-Constraint Planning).
- **Subsystems**: Scene, Renderer, Math, Persistence, Interaction.
- **Transformation Order Issue**: Identified that `S * R * T` multiplication order (with Row-Vector convention) was being used inconsistently across systems.

## Architecture Evolution: Structural Truth

### 1. Assembly Model: `Group` Primitives
- Introduced `PrimitiveType::Group`. 
- **Behavior**: Acts as a logical transform parent with no geometry. It propagates all transformations to its descendants but is skipped by the renderer.

### 2. Transformation Semantics (Row-Vector)
- **Convention**: `v_world = v_local * M_local * M_parent`.
- **Local Matrix**: `S * R * T` (Scale then Rotate then Translate).
- **Surgical Correction**: Updated `computeWorldMatrix` to strictly follow this order. This ensures hierarchical propagation (moving a parent moves the whole assembly) works correctly and is consistent with the `inverse()` math used in picking.

### 3. Aggregate Bounding Boxes
- Implemented `getAggregateBoundingBox()`.
- **Algorithm**: Recursively merges the world-space bounds of all descendants.
- **Verification**: `test_aggregate_bounds` confirms parent assemblies correctly encompass children.

### 4. Interactive Inspection
- **Selection Root (Key 'G')**: Allows selecting the top-level assembly of any clicked primitive.
- **Exploded View (Key 'E')**: Multiplies local child translations by an explosion factor.
- **Selection Propagation**: If an assembly is selected, the renderer highlights all descendants.

## Reference Assembly Reconstruction
- **TABLE**: Reconstructed as a `Group` containing `table_top` and `leg_1..4`.
- **CHAIR**: Reconstructed as a `Group` containing `seat` and `back`.
- **Result**: Moving the `table` group now moves all legs and the top together, maintaining structural integrity.

## Conformance Results
- **Hierarchical Propagation**: PASS (Verified by `AssemblyTests`).
- **Aggregate Bounds**: PASS (Verified by `AssemblyTests`).
- **Exploded View Math**: PASS (Verified by `AssemblyTests`).
- **Persistence**: PASS (Verified by `AssemblyPersistenceTests` and `PersistenceTests`).

## Regression Results
- **HSE-019/020/021/022**: All 13 tests GREEN.
- **Table/Leg Defect**: RESOLVED.

## Final Classification: **PROVEN**
HSE can now represent and manipulate complex real-world objects as coherent hierarchical assemblies. Structural truth is preserved across transformation, inspection, and persistence.
