# HSE-019 — Acceptance Criteria

## Automated Criteria (Machine)

| ID | Criterion | Verification Method |
| :--- | :--- | :--- |
| AC-01 | Build Success | `cmake --build build` returns 0. |
| AC-02 | Regression Tests | `ctest` returns 100% pass. |
| AC-03 | Persistence | `exportHSC` -> `importState` -> `compare` succeeds. |
| AC-04 | Animation | `Scene::update` results in changing rotation values over time. |
| AC-05 | Lighting | Shader compiles with light uniforms; Renderer passes them. |

## Visual Criteria (Human)

| ID | Category | Expectation |
| :--- | :--- | :--- |
| VC-01 | Structure | Floor, 4 walls, ceiling visible and connected. |
| VC-02 | Openings | Doorway and Window recognizable as holes in walls. |
| VC-03 | Furniture | Table and 2 chairs positioned on floor. |
| VC-04 | Materials | Distinct colors for floor vs walls vs furniture. |
| VC-05 | Lighting | Surfaces show light/shadow gradients from point light. |
| VC-06 | Camera | View provides a clear overview of the room interior. |
| VC-07 | Animation | At least one chair or object visibly spinning. |

## Final Gate
Human operator must explicitly approve the visual result before mission completion.
