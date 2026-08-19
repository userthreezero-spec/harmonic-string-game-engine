# HSE-022 — Transformation Plan (Search Result)

## Goal
Relocate dining setup to a "Window-side" configuration (`z = -2.0`).

## Initial Conflict Detection
1. **Move table to `z = -2.0`**: OK.
2. **Move chairs to `z = -2.0`**: OK.
3. **Move table to `x = 2.0` (Window/Wall corner)**:
   - Predicted: `Chair 2` @ `x = 3.2` hits `Wall Right` @ `x = 3.0`.
   - Result: **CONFLICT DETECTED**.

## Revised Strategy (Symmetric Z-axis setup)
Instead of placing chairs on X-sides (left/right), place them on Z-sides (front/back) to allow the table to be closer to the side wall without hitting the boundary.

### Steps
1. **Move Table**: `[1.5, -0.7, -2.0]`.
2. **Move Chair 1 (Front)**: `[1.5, -1.0, -1.3]`, Rotation `[0, 180, 0]`.
3. **Move Chair 2 (Back)**: `[1.5, -1.0, -2.7]`, Rotation `[0, 0, 0]`.
4. **Stabilize**: Ensure `rotation_speed` is [0, 0, 0].

## Transformation Order
1. `table_top` (Anchor).
2. `chair_1`, `chair_1_back` (Satellite 1).
3. `chair_2`, `chair_2_back` (Satellite 2).
4. `lights` (Optional illumination adjustment).

## Expected Validation
- Proximity to table: ~0.3m.
- Facing: Both face `z=-2.0`.
- Collision: `1.5 +/- 0.5` (Table) vs `1.5 +/- 0.3` (Chairs) -> OK.
- Wall: `1.5` is 1.5m from wall. OK.
- Doorway: `z=-2.0` is 5m from door. OK.
- Path: Clear path from `z=3` to `z=-2` along `x=[-1, 1]`. OK.
