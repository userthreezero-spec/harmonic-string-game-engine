# HSE-022 — World State Baseline

## Current World State (HSE-021)

### Fixed Environment (PROVEN)
- **Floor**: Center [0, -1.5, 0], Scale [6, 6, 1], Rotation [-90, 0, 0].
- **Doorway**: Location at `z=3`, `x` range `[-1, 1]`. Formed by `wall_front_left`, `wall_front_right`, and `wall_front_top`.
- **Window**: Located on the back wall (`z=-3`) at center `x=0`.

### Furniture (PROVEN)
- **Table**: `table_top` at `[0, -0.7, 0]`.
- **Chair 1**: Seat `chair_1` at `[-1.2, -1.0, 0]`, rotation `[0, 90, 0]`, rotation_speed `[0, 0, 0]`.
- **Chair 2**: Seat `chair_2` at `[1.2, -1.0, 0]`, rotation `[0, -90, 0]`, rotation_speed `[0, 0, 0]`.

### Semantic Interpretation (INFERRED)
- **Dinner Conversation Role**: Satisfied by current chair/table proximity and facing.
- **Doorway Access**: The table is currently at `z=0`, door is at `z=3`. The path is clear.
- **Clear Path**: A direct line from Doorway (`z=3`) to Table/Window area is unobstructed.

### Dynamic State (PROVEN)
- Both chairs are stable (0 rotation speed).

## Multi-Constraint Reasoning Requirements
- Goal: Maintain the "Dinner" setup while ensuring new constraints like "Clear Path" and "Doorway Access" are not violated by any future moves (e.g. if the human asks to move the table).
- Conflict Potential: Moving the table towards the front of the room (`+Z`) will eventually block the door.
