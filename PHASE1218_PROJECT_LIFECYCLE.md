# PHASE 1218 — Canonical Project Lifecycle Specification

## Deterministic State Transition Model

The canonical Harmonic String Game Engine (`hse_app.exe`) governs its lifecycle via an explicit state machine (`enum class ApplicationState`):

```
                       [ENGINE_HOME]
                             │
                             ▼
                     [PROJECT_BROWSER] ◄─────────────────┐
                             │ (User / Auto Select)       │
                             ▼                            │
                     [PROJECT_SELECTED]                   │
                             │                            │
                             ▼                            │
                     [PROJECT_LOADING]                    │
                             │                            │
                             ▼                            │
                      [PROJECT_OPEN]                      │
                             │                            │
                             ▼                            │
                      [SCENE_LOADING]                     │
                             │                            │
                             ▼                            │
                     [ENGINE_RUNTIME]                     │
                             │ (ESC Key)                  │
                             ▼                            │
                      [CHECKPOINTING]                     │
                             │                            │
                             ▼                            │
                     [PROJECT_CLOSING]                    │
                             │                            │
                             ▼                            │
                    [RETURNING_TO_HUB] ───────────────────┘
```

## State Definitions & Operational Rules

1. **ENGINE_HOME**: Startup state. Initializes GLFW window, OpenGL context, renderer uniforms, and logs forensic trace information.
2. **PROJECT_BROWSER**: Canonical Project Hub UI. Discovers `.json` and `.hsc` manifests in `projects/`, presents interactive options (`[1-N]` Open, `[C]` Create, `[Q]` Exit).
3. **PROJECT_SELECTED**: Validates path, sets up checkpoint filename (`<project>_checkpoint.hsc.json`).
4. **PROJECT_LOADING**: Loads project state. First checks for existing checkpoint file; falls back to native C++ scene builders (`buildTwoStoryHome()`, `buildRoom()`) or Python manifest bridge.
5. **PROJECT_OPEN**: Scene primitives, lights, materials, and relationship graph instantiated in memory.
6. **SCENE_LOADING**: Uploads primitive geometry to GPU (VBO/VAO), configures active perspective camera, sets ambient/animated lighting, and initializes IPC Pipe Bridge (`\\.\pipe\hse_bridge`).
7. **ENGINE_RUNTIME**: Interactive 3D simulation loop. Handles WASDQE fly navigation, mouse look, TAB cursor lock, F focus reset, G assembly selection, E exploded view, S checkpoint save, H structural propagation, SPACE impulse light.
8. **CHECKPOINTING**: Serializes active scene graph, camera position, and project manifest to checkpoint file (`.hsc.json`).
9. **PROJECT_CLOSING**: Stops IPC Pipe Bridge worker thread, releases active scene and camera pointers, clears object selection.
10. **RETURNING_TO_HUB**: Transitions cleanly back to `PROJECT_BROWSER`.
11. **APPLICATION_EXIT**: Shuts down renderer, closes GLFW window, terminates process with exit code 0.

## Hardware Specs Adaptations
- **Intel UHD 615 / Pentium 4425Y**: Minimal memory allocations during state transitions. Scene objects re-instantiated cleanly without memory leaks between `PROJECT_CLOSING` and `PROJECT_OPEN`.
