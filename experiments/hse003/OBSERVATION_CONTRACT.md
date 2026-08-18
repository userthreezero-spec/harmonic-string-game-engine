# HSE-003 OBSERVATION CONTRACT

**Date:** 2026-08-17

---

## What Can Be Observed

| Mechanism | Available | Evidence |
|-----------|-----------|----------|
| Process survival | YES | PID created, survived 6+ seconds |
| OpenGL context | YES | GLEW init succeeded (no crash) |
| Window creation | YES | GLFW window created |
| Shader compilation | YES | Render loop active (no crash) |
| Scene state export | YES | JSON exported with 11 objects |
| Screenshot capture | NO | No framebuffer capture implemented |
| Pixel inspection | NO | No readpixels implementation |
| Depth buffer | NO | No depth readback |
| Visual observation | PARTIAL | Human can see screen; OpenCode cannot |

## What Cannot Be Observed Remotely

- Actual rendered image (requires physical screen or framebuffer capture)
- Whether quads are visible from camera angle
- Whether openings (doorway/window) create visible gaps
- Whether colors match specification
- Whether depth testing works correctly for overlapping quads

## Observation Method

For this experiment, observation is:
1. **Process evidence** — Did the executable run without crashing?
2. **Scene state** — Does the exported JSON match the world contract?
3. **Count verification** — Do primitive/camera counts match?
4. **Transform verification** — Are positions/rotations/scales correct?
5. **Visual verification** — Human observer (Rico) confirms rendering

## Known Limitations

- OpenCode cannot see the rendered image
- No automated visual testing
- Scene state export is the primary machine-readable observation
