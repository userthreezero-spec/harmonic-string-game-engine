# HSE-019 — Persistence Verification

## Format: .hsc (JSON)

## Test Case: CREATE -> SAVE -> CLOSE -> REOPEN -> OBSERVE

### 1. Create & Save
- Created via `hse019_build_room.py`.
- Saved to `experiments/hse019/room.hsc`.

### 2. Reopen & Observe
- Verified using `tests/test_hsc_persistence.cpp`.
- Logic: `importState` loads the file and compares values against known constants.

### Verified Properties
- [x] Project Name
- [x] Primitive Count (18)
- [x] Light Count (1)
- [x] Light Intensity (1.5)
- [x] Camera Position (4.5, 2.0, 4.5)
- [x] Object IDs & Names
- [x] Material IDs
- [x] Rotation Speed (for animation)

## Conclusion
Persistence is **PROVEN** and reliable for the HSE-019 mission scope.
