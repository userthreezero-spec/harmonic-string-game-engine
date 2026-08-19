# HSE-021 — Transformation Plan

## Objective
Satisfy the "Comfortable Conversation" goal.

## Planned Moves

### 1. Stabilize Chair 1
- **Target**: `chair_1`
- **Action**: Set `rotation_speed` to [0, 0, 0].
- **Reason**: Spinning chairs are unsuitable for conversation.

### 2. Reposition Chair 1 (Closer to Table)
- **Target**: `chair_1`, `chair_1_back`
- **New Position (chair_1)**: [-1.2, -1.0, 0] (Moved from -1.5)
- **New Position (chair_1_back)**: [-1.5, -0.6, 0] (Maintained offset of 0.3)
- **Reason**: Proximity to table edge for comfort.

### 3. Reposition Chair 2 (Closer to Table)
- **Target**: `chair_2`, `chair_2_back`
- **New Position (chair_2)**: [1.2, -1.0, 0] (Moved from 1.5)
- **New Position (chair_2_back)**: [1.5, -0.6, 0] (Maintained offset of 0.3)
- **Reason**: Symmetry and proximity.

### 4. Verify Facing
- **Status**: Already facing center (Chair 1: 90, Chair 2: -90). No rotation needed.

## Execution Method
Use a Python script `hse021_execute_transformation.py` to send commands via the HSE Bridge to the running `hse_workspace_app`.
