# HSE-019 — Complexity Assessment

## Scoring

| Category | Score (0-10) | Justification |
| :--- | :--- | :--- |
| **A. Subsystems** | 8 | Window, Renderer, Scene, Math, Bridge, Material, Texture, Camera. |
| **B. Symbol surface** | 7 | Implementing Lighting and Normal generation across multiple classes. |
| **C. Cross-subsystem dependencies** | 8 | Tight coupling between Renderer, Scene, and Shaders for lighting. |
| **D. Runtime state** | 6 | Persistent project loading and real-time animation. |
| **E. Visual verification difficulty** | 9 | Mandatory human visual inspection gate for first time. |
| **Total** | **38** | |

## Classification
- **Score**: 38 / 50
- **Classification**: **Very High**

## Analysis
The complexity is driven by the need to implement a missing fundamental system (Lighting) while simultaneously using it to build a non-trivial 3D scene that must pass human inspection. This requires cross-subsystem coordination that exceeds prior experiments.
