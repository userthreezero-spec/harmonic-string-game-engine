# HSE-020 — Complexity Assessment

## 40-Point Complexity Model

| Category | Score (0-4) | Justification |
| :--- | :---: | :--- |
| **1. Number of Subsystems** | 4 | Window, Renderer, Scene, Math, Bridge, Persistence, Input, Camera. |
| **2. New Public APIs** | 3 | Extending Window for input and Camera for manual control. |
| **3. Input Complexity** | 4 | Drag-to-orbit and Pan state machines are non-trivial to feel "right". |
| **4. Rendering Complexity** | 2 | Simple uniform-based highlighting. |
| **5. Mathematical Complexity** | 4 | Ray-casting from mouse screen space to 3D primitive intersection. |
| **6. Persistence Impact** | 2 | Updating existing save logic to include workspace state if needed. |
| **7. Bridge Impact** | 2 | Adding "select" observation to protocol. |
| **8. State Management** | 3 | Managing "selected object" across Workspace and Scene. |
| **9. Regression Risk** | 3 | Changes to Camera and Window are core-critical. |
| **10. Visual Verification** | 4 | First experiment requiring interactive human confirmation. |
| **Total Score** | **31** | |

## Classification
- **Score**: 31 / 40
- **Classification**: **High**

## Analysis
The primary driver of complexity is the "Input-to-3D-Math" bridge. Specifically, converting 2D mouse coordinates into a 3D ray and performing intersection tests against the scene graph. While the rendering changes are minor, the interaction loop introduces many new stateful variables that increase regression risk.
