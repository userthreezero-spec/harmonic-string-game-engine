# HSE-021 — Semantic Model & Goal Interpretation

## Semantic Layer (Minimum Required)

### 1. Functional Grouping
- `TableGroup`: `table_top` + legs.
- `SeatingGroup1`: `chair_1` + `chair_1_back`.
- `SeatingGroup2`: `chair_2` + `chair_2_back`.

### 2. Derived Constraints
For "comfortably sit at the table and have a conversation":
- **Constraint A (Stability)**: `rotation_speed` must be zero for all seating.
- **Constraint B (Proximity)**: Seating distance from table edge should be ~0.2m.
- **Constraint C (Facing)**: Seating should face the Table center.
- **Constraint D (Conversation)**: People should face each other (opposite sides of table).

## Transformation Targets
- **Primary**: `SeatingGroup1`, `SeatingGroup2`.
- **Secondary**: `TableGroup` (Stationary reference).
