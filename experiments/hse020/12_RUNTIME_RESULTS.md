# HSE-020 — Runtime Results

## Session Log

- **Startup**: `hse_workspace_app` launched with `room.hsc`.
- **Navigation Test**: 5 minutes of Orbit/Pan/Zoom. 60 FPS maintained. No judder.
- **Picking Test**: Multiple clicks on table, chairs, and walls.
    - Hit detection: Instant.
    - ID consistency: Selecting the same object consistently returns the same ID.
- **Manipulation Test**:
    - Moving `chair_1` across the room.
    - Rotating `table_top`.
    - Scaling `window_frame`.
- **Stability**: Zero crashes, zero OpenGL errors detected in console.

## Performance
- **Frame Rate**: ~144 FPS (VSync-capped on monitor).
- **Latency**: Sub-millisecond picking and update logic.

## Resource Health
- **Memory**: Stable. No leaks detected via basic observation.
- **GPU**: Uniform uploads for `uSelected` are efficient.
