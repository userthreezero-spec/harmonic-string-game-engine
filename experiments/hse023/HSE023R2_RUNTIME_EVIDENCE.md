# HSE-023R2 — Runtime Evidence

## Observed Interaction Workflow (Corrected)

### 1. Selection
- **Input**: Left-click Table Top.
- **Result**: `TargetCube (ID: X)` is selected. Highlighting appears on the top.
- **Input**: Press 'G'.
- **Result**: Selection moves to `table (ID: Y)`. **ALL** table components (top and 4 legs) highlight in yellow.
- **Status**: **VERIFIED**.

### 2. Manipulation
- **Input**: Arrow keys with `table` group selected.
- **Result**: Table top and all legs move in perfect unison. No components left behind.
- **Status**: **VERIFIED**.

### 3. Explosion
- **Input**: Press 'E' with `table` selected.
- **Result**: Table parts move outward from the group center.
- **Input**: Press 'E' again.
- **Result**: Table returns exactly to its assembled configuration.
- **Status**: **VERIFIED**.

### 4. Persistence
- **Input**: Press 'S' to save. Close application. Relaunch with `--project`.
- **Result**: Hierarchical grouping is preserved. Child-to-parent relationships remain intact in the scene observation.
- **Status**: **VERIFIED**.
