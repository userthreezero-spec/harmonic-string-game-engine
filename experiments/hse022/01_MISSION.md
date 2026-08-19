# HSE-022 — Multi-Constraint Goal Planning & World Transformation

## Mission Description
Starting from the existing HSE-021 room state, satisfy the following high-level objective:

"Prepare this room for a two-person dinner conversation while keeping the doorway accessible, preserving the existing furniture, keeping both chairs usable, maintaining a clear path through the room, and avoiding collisions or impossible object placements."

## Core Objectives
1. **Multi-Constraint Reasoning**: Interpret simultaneous goals (Dinner, Accessibility, Usability, Collision-Free).
2. **Conflict Detection**: Identify if satisfying one constraint (e.g., proximity to table) violates another (e.g., doorway access).
3. **Ordered Planning**: Determine the sequence of transformations.
4. **Autonomous Execution**: Transform the world state using derived plans.
5. **Validation**: Verify all constraints are met in the final state.

## Success Criteria
- [ ] Room layout supports two-person dinner conversation.
- [ ] Doorway remains accessible (unblocked).
- [ ] Clear path through the room is maintained.
- [ ] No object collisions introduced.
- [ ] Furniture preserved and remains within room bounds.
- [ ] Results persist through save/reload.
