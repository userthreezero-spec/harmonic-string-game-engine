# HSE-006 Architecture

## System Overview

```
┌─────────────────────────────────────────────────────────┐
│                    USER / WEBOS                           │
│                                                          │
│  ┌──────────────┐    ┌──────────────────────────────┐   │
│  │  .hsc file    │    │  External Controller          │   │
│  │  (project)    │    │  (PowerShell/Python)          │   │
│  └──────┬───────┘    └────────────┬───────────────────┘  │
│         │                         │                      │
│         └──────────┬──────────────┘                      │
│                    │                                     │
├────────────────────┼─────────────────────────────────────┤
│                    │  INTEGRATION BOUNDARY                │
├────────────────────┼─────────────────────────────────────┤
│                    │                                     │
│  ┌─────────────────▼────────────────────────────────┐   │
│  │           HSE WORKSPACE LAUNCHER                  │   │
│  │                                                    │   │
│  │  1. Parse .hsc project file                       │   │
│  │  2. Create project directories                     │   │
│  │  3. Import scene from .hsc                        │   │
│  │  4. Upload GPU resources                          │   │
│  │  5. Start bridge (internal service)               │   │
│  │  6. Run render loop                               │   │
│  │  7. Save state on exit                            │   │
│  └─────────────────┬────────────────────────────────┘   │
│                    │                                     │
│  ┌─────────────────▼────────────────────────────────┐   │
│  │           HSE ENGINE (hse_core)                    │   │
│  │                                                    │   │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────────┐   │   │
│  │  │ Window    │  │ Renderer │  │ Scene         │   │   │
│  │  │ (GLFW)    │  │ (OpenGL) │  │ (Primitives + │   │   │
│  │  │           │  │          │  │  Cameras)     │   │   │
│  │  └──────────┘  └──────────┘  └──────────────┘   │   │
│  └───────────────────────────────────────────────────┘   │
│                                                          │
│  ┌───────────────────────────────────────────────────┐   │
│  │           HSE BRIDGE (hse_bridge)                  │   │
│  │                                                    │   │
│  │  Named Pipe: \\.\pipe\hse_bridge                  │   │
│  │  Protocol: NDJSON (13 command types)              │   │
│  │  Queue: SPSC ring buffer (256 slots)              │   │
│  │  Observations: scene, object, camera, health,     │   │
│  │                delta, frame_capture               │   │
│  │  Project: save_project, load_project              │   │
│  └───────────────────────────────────────────────────┘   │
└──────────────────────────────────────────────────────────┘
```

## Key Design Decisions

1. **Single .hsc file** — Simple, self-contained, matches existing JSON patterns
2. **Bridge is internal service** — No separate executable needed
3. **SceneBuilder handles I/O** — importState/exportHSC centralized
4. **Object IDs mapped to names** — String IDs in .hsc, uint64 at runtime
5. **No rebuild on reopen** — .hsc is data, not code
6. **Incremental modification** — Commands modify existing scene, no restart needed

## Data Flow

### Open Project
```
.hsc file → SceneBuilder::importState() → Scene + Camera → GPU upload → Bridge starts → Render loop
```

### Modify
```
External command → Bridge queue → GL thread executes → Scene mutates → Next frame renders
```

### Save
```
Bridge save_project command → SceneBuilder::exportHSC() → .hsc file written
```

### Reopen
```
.hsc file → importState() → Same scene restored → GPU upload → Bridge starts
```
