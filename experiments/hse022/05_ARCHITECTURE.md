# HSE-022 — Architecture

## 1. Constraint Representation
Constraints will be represented as a set of evaluation functions in the Orchestrator.
- `eval_stability(scene)`
- `eval_proximity(scene, object_a, object_b)`
- `eval_collision(scene, object_a, object_b)`
- `eval_doorway_access(scene)`

## 2. Planning Logic: Ordered Search
The system will use a "Search-based Planning" approach:
1. **Identify Violations**: Scan current world state against constraints.
2. **Propose Action**: Select an object and a transformation (e.g., move table to [2, -0.7, 0]).
3. **Simulate**: Predict the new world state after the action.
4. **Conflict Check**: Run all constraints against the predicted state.
5. **Score**: If conflicts exist (e.g., Chair hits wall), reject the action or propose a remedial action (e.g., move chair as well).
6. **Order**: Execute dependent moves in a sequence that minimizes intermediate violations.

## 3. Transformation Order Reasoning
- **Rule 1: Anchor First**. Move the central functional object (`table_top`) first.
- **Rule 2: Adjust Satellites**. Move associated objects (`chairs`) to satisfy proximity/facing relative to the new anchor position.
- **Rule 3: Collision Recovery**. If a satellite hits an environment boundary, adjust the anchor or the satellite offset until valid.

## 4. Visual Selection & Highlight
Reuse HSE-020's `uSelected` highlight during the transformation process if running interactively, to show which objects are being reasoned about.

## 5. Persistence
Reuse HSE-019's `.hsc` serialization via the Bridge `save_project` command.
