# HSE-021 — World State Archaeology

## Initial Scene State (`room.hsc`)

### Objects (PROVEN)
| Name | Type | Position | Rotation (Y) | Note |
| :--- | :--- | :--- | :--- | :--- |
| floor | Quad | [0, -1.5, 0] | 0 | Ground plane |
| table_top | Cube | [0, -0.7, 0] | 0 | Functional surface |
| chair_1 | Cube | [-1.5, -1.0, 0] | 90 | Spinning (45 deg/s) |
| chair_1_back | Cube | [-1.8, -0.6, 0] | 90 | Component of Chair 1 |
| chair_2 | Cube | [1.5, -1.0, 0] | -90 | Stationary |
| chair_2_back | Cube | [1.8, -0.6, 0] | -90 | Component of Chair 2 |

### Semantic Roles (INFERRED)
- `Chair 1`: {`chair_1`, `chair_1_back`}. Seat at [-1.5, -1.0, 0].
- `Chair 2`: {`chair_2`, `chair_2_back`}. Seat at [1.5, -1.0, 0].
- `Table`: {`table_top`, `table_leg_1..4`}. Center at [0, -0.7, 0].

### Current Relationships
- **Proximity**: Chairs are 0.5m from table edge (Table width = 2.0m, Chairs at +/- 1.5m).
- **Facing**: Chair 1 faces +X (Table). Chair 2 faces -X (Table).
- **State**: Chair 1 is spinning (NOT suitable for sitting).
