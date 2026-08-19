# HSE-021 — Engineering Report

## Mission Statement
Satisfy the high-level goal: "Arrange this room so that two people can comfortably sit at the table and have a conversation."

## Repository Archaeology
- **Baseline**: HSE-020 (Interactive Workspace).
- **Capabilities**: Scene graph, transforms, rotation speed, Bridge, and .hsc persistence.
- **Extensions**: Added `CMD_SET_ROTATION_SPEED` to Bridge for seating stabilization.

## Existing World Model (HSE-019 Room)
- **Table**: Located at center [0, -0.7, 0]. Functional surface.
- **Seating**: Two chairs positioned at +/- 1.5m.
- **Defect**: `chair_1` was spinning (animation speed 45), making it unsuitable for sitting.

## Semantic Model
- **Roles**: 
    - `chair_1`, `chair_2` -> Seating units.
    - `table_top` -> Target functional surface.
- **Inferred Relationship**: Seating must be stationary and in proximity to the table to facilitate conversation.

## Goal Interpretation & Constraints
1. **Stability**: Seat rotation speed must be [0, 0, 0].
2. **Proximity**: Seats moved from 0.5m gap to 0.2m gap from table edge.
3. **Facing**: Seats face each other across the table (already satisfied by baseline rotation).

## Transformation Plan
1. **Move `chair_1`**: [-1.5, -1.0, 0] -> [-1.2, -1.0, 0].
2. **Move `chair_1_back`**: [-1.8, -0.6, 0] -> [-1.5, -0.6, 0].
3. **Move `chair_2`**: [1.5, -1.0, 0] -> [1.2, -1.0, 0].
4. **Move `chair_2_back`**: [1.8, -0.6, 0] -> [1.5, -0.6, 0].
5. **Stabilize `chair_1`**: Set `rotation_speed` to zero.

## Conformance Results
- **Stability**: PASS (Verified by `GoalTests`).
- **Proximity**: PASS (Chair distance to table edge = 0.2m).
- **Facing**: PASS (Y-rotations +/- 90 degrees maintained).
- **Persistence**: PASS (Verified via project reload in `GoalTests`).

## Regression Results
- **Math/Scene/Material/Picking**: All 9 tests GREEN.
- **HSE-020 Functionality**: Manual navigation and selection remains functional.

## Human Visual Verification
- **Instructions**: Run `.\launch_hse019.bat`.
- **Observations**:
    - Both chairs are stationary.
    - Chairs are visibly closer to the table.
    - Conversation setup is coherent.

## Final Classification: **PROVEN**
WebOS successfully reasoned over the world's semantic roles and transformed the scene to satisfy a high-level intent.
