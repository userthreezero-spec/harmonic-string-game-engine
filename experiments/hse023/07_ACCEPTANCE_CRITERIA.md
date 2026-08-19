# HSE-023 — Acceptance Criteria

## Automated Criteria (Machine)
| ID | Criterion | Verification Method |
| :--- | :--- | :--- |
| **AC-01** | Assembly Hierarchy | `test_assembly` confirms child world positions update when parent `Group` moves. |
| **AC-02** | Matrix Correctness | `test_transform_order` confirms rotation occurs around object origin, not world origin. |
| **AC-03** | Aggregate Bounds | `test_assembly_bounds` confirms parent box encompasses all children. |
| **AC-04** | Persistence | Save/Reload of a nested assembly results in identical world-space positions. |
| **AC-05** | Regression | HSE-019, HSE-020, HSE-021 tests remain GREEN. |

## Interactive Criteria (Human)
| ID | Category | Expectation |
| :--- | :--- | :--- |
| **VC-01** | Selection | Clicking a leg then pressing 'G' selects the entire table (all parts highlight). |
| **VC-02** | Manipulation | Moving the selected table assembly moves all 5 parts together. |
| **VC-03** | Rotation | Rotating the table assembly keeps legs correctly attached and oriented. |
| **VC-04** | Exploded View | Pressing 'E' causes the table parts to visibly separate. |
| **VC-05** | Restore | Pressing 'E' again returns the table to its perfectly assembled state. |
| **VC-06** | Inspection | Tree view (console) correctly displays: `TABLE` -> `[TOP, LEG1, LEG2, LEG3, LEG4]`. |

## Final Gate
Human confirmation that the table is now a single manipulable "Thing" rather than 5 unrelated boxes.
