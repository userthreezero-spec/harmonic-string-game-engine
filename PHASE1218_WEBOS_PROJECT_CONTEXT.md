# PHASE 1218 — WebOS Project Context & Hardware Alignment

## Architectural Integration & Hardware Awareness

```
  [HSE UI / Project Hub]
            │
            ▼
  [WebOS Assistant Interface] ───► [Sovereign Gateway (HTTP 8725)]
                                                │
                                                ▼
                                    [WebOS Cognitive Architecture]
                                                │
                                                ▼
                                     [PrimeGate (Mutation)]
                                                │
                                                ▼
                                    [Build & Validation Loop]
```

## Conceptual Hierarchy
WebOS explicitly distinguishes between:
1. **ENGINE**: The core runtime executable (`hse_app.exe`), renderer, windowing, and state machine.
2. **PROJECT**: The top-level workspace container (e.g. `two_story_home.json`) with metadata, settings, and checkpoint state.
3. **SCENE**: The active primitive graph, materials, cameras, lights, and relationships loaded into memory.
4. **RUNTIME**: The active 3D simulation loop executing physics, updates, and rendering.
5. **WORKSPACE**: The developer tools layer (IPC bridge, picker, assembly selection, exploded view).

A natural language mission such as `"Open the two-story home project."` sent via Sovereign Gateway / PrimeGate routes through the Project Hub lifecycle:
`ENGINE_HOME -> PROJECT_BROWSER -> PROJECT_SELECTED -> PROJECT_LOADING -> PROJECT_OPEN -> SCENE_LOADING -> ENGINE_RUNTIME`.

## Target PC Hardware Profile Alignment
- **Processor**: Intel Pentium CPU 4425Y @ 1.70GHz (Dual-Core, 4 Threads).
- **Graphics**: Intel UHD Graphics 615 (1GB Shared VRAM, OpenGL 3.3 Core Profile).
- **RAM**: 8.0 GB Physical RAM.
- **Rendering & Performance Strategy**:
  - Shader pipeline remains lightweight OpenGL 3.3 Core with cached uniform locations.
  - VSync / 60 FPS frame time clamping prevents thermal throttling on Y-series CPU.
  - Non-blocking named pipe IPC (`CancelIoEx` unblocking) prevents thread contention on 2-core CPU.
  - Native C++ scene construction completes in < 1ms with < 45 MB RAM usage.
