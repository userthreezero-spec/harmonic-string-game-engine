# HSE-023 — Structural Truth (Existing Room)

## HSE-022 Current Composition (PROVEN)

### Table "Assembly" (Implicit)
- `table_top`: Positioned at [1.5, -0.7, -1.8].
- `table_leg_1..4`: Positioned independently in world space.
- **Problem**: No parent link exists. Moving `table_top` via Bridge or Interaction leaves legs behind.

### Chair "Assemblies" (Implicit)
- `chair_1`: Seat.
- `chair_1_back`: Backrest.
- `chair_2`: Seat.
- `chair_2_back`: Backrest.
- **Problem**: No hierarchy. The "Backrest" is just another object in the scene.

## Proposed Reconstruction (PLANNED)

### 1. TABLE (Assembly)
- `TABLE` (New `Group` primitive)
  ├── `TABLE_TOP` (Primitive)
  ├── `LEG_1` (Primitive)
  ├── `LEG_2` (Primitive)
  ├── `LEG_3` (Primitive)
  └── `LEG_4` (Primitive)
- **Transform Semantics**: `TABLE` position is the world center of the table. Children have local offsets.

### 2. CHAIR_1 (Assembly)
- `CHAIR_1` (New `Group` primitive)
  ├── `CHAIR_1_SEAT` (Primitive)
  └── `CHAIR_1_BACK` (Primitive)

### 3. ROOM (Assembly)
- `ROOM` (New `Group` primitive)
  ├── `ENVIRONMENT` (Floor, Walls, Ceiling)
  └── `FURNITURE` (Table, Chairs)

## Semantic Metadata Requirements
- `id`: Unique stable identifier.
- `name`: Semantic name (e.g., "Engine Block").
- `role`: (Assembly, Component, Primitive).
