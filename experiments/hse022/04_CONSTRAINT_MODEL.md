# HSE-022 — Multi-Constraint Model

## Mission Constraints

### 1. Functional: Dinner Conversation
- **Proximity**: Chairs must be within [0.1, 0.4] meters of the table edge.
- **Orientation**: Chairs must face the table center (within +/- 15 degrees).
- **Count**: Exactly two chairs must be associated with the table.

### 2. Usability: Chair Access
- **Stability**: `rotation_speed` must be zero.
- **Non-Overlap**: Chairs must not intersect the table or other chairs.
- **Sitting Space**: There should be at least 0.6m clear space behind each chair for a person to sit.

### 3. Spatial: Doorway Access
- **Constraint**: No object can intersect the "Door Volume".
- **Door Volume**: A box at `z=[2.5, 3.5]`, `x=[-1.1, 1.1]`, `y=[-1.5, 1.0]`.

### 4. Spatial: Clear Path
- **Constraint**: A corridor of at least 1.0m width must exist from the Doorway to the functional area (Table).

### 5. Preservation: Furniture
- **Constraint**: `table_top`, `chair_1`, `chair_2` must remain in the scene.

### 6. Physical: Collision & Bounds
- **Constraint**: Objects must not intersect walls or each other.
- **World Bounds**: All objects must have `y >= -1.5` and remain within `x=[-3, 3]`, `z=[-3, 3]`.

## Representability in HSE
- **Transforms**: Native.
- **Distances**: Computed from `position`.
- **Collisions**: Must be implemented via AABB or geometric distance checks in the orchestrator.
- **Pathing**: Simplified as "Line-of-sight" or "Volume clearance".
