# HSE-022 — Constraint Graph

## Entities
- `TABLE` (`table_top` + legs)
- `CHAIR_1` (seat + back)
- `CHAIR_2` (seat + back)
- `DOOR` (volume at `z=3`, `x=[-1,1]`)
- `WALLS` (boundaries at `+/-3`)

## Relationships & Dependencies

```mermaid
graph TD
    CHAIR_1 -- "Must be near" --> TABLE
    CHAIR_2 -- "Must be near" --> TABLE
    CHAIR_1 -- "Must face" --> TABLE
    CHAIR_2 -- "Must face" --> TABLE
    CHAIR_1 -- "Must not block" --> DOOR
    CHAIR_2 -- "Must not block" --> DOOR
    TABLE -- "Must not block" --> DOOR
    CHAIR_1 -- "Must not hit" --> WALLS
    CHAIR_2 -- "Must not hit" --> WALLS
    TABLE -- "Must not hit" --> WALLS
    CHAIR_1 -- "Must not hit" --> CHAIR_2
```

## Conflict Evaluation
- If `TABLE` moves to `x=2`, `CHAIR_2` must move to `x=3.2` to maintain proximity.
- **Conflict**: `CHAIR_2` @ `x=3.2` intersects `WALL_RIGHT` @ `x=3.0`.
- **Resolution**: Adjust `TABLE` position or `CHAIR_2` offset.

- If `TABLE` moves to `z=2.8`, it intersects `DOOR`.
- **Conflict**: `DOOR` access violated.
- **Resolution**: Move `TABLE` back to `z <= 1.8` (allowing for table depth).
