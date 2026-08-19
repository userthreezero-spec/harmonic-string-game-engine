# HSE-023 — Complexity Assessment

## Complexity Scoring (0-10 per category)

| Category | Score | Justification |
| :--- | :--- | :--- |
| **A. Subsystems** | 8 | Touching Scene, Renderer, Math, Persistence, Interaction. |
| **B. Symbol Surface** | 7 | Introducing `PrimitiveType::Group`, `getAggregateBounds`, and `ExplodedView`. |
| **C. Cross-Subsystem Dependencies** | 9 | Hierarchical transforms affect Picking, Shaders (highlighting), and Persistence. |
| **D. Runtime State** | 7 | Maintaining deep nested hierarchies and temporary visual states. |
| **E. Visual Verification Difficulty** | 9 | Exploded view and assembly-level selection require precise interactive confirmation. |
| **Total** | **40 / 50** | |

## Classification: **Very High**

## Analysis
The primary driver of complexity is the transition from "Individual Primitives" to "Dependent Assemblies". Ensuring that a transformation on a parent propagates correctly through arbitrary depth while preserving picking accuracy and aggregate bounding boxes is a significant architectural challenge for the current minimal engine.
