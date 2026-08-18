# HSE-005 Architecture

## System Overview

```
┌─────────────────────────────────────────────────────┐
│                  EXTERNAL LAYER                      │
│                                                      │
│  ┌──────────────┐    ┌──────────────────────────┐   │
│  │   WebOS       │    │  Deterministic Controller │   │
│  │  (Future)     │    │  (HSE-005)                │   │
│  └──────┬───────┘    └────────────┬───────────────┘  │
│         │                         │                   │
│         └──────────┬──────────────┘                   │
│                    │                                  │
│         ┌──────────▼──────────┐                       │
│         │   HSE_EXTERNAL_     │                       │
│         │   PROTOCOL.md       │                       │
│         └──────────┬──────────┘                       │
│                    │                                  │
├────────────────────┼──────────────────────────────────┤
│                    │  INTEGRATION BOUNDARY             │
├────────────────────┼──────────────────────────────────┤
│                    │                                  │
│         ┌──────────▼──────────┐                       │
│         │   Named Pipe         │                       │
│         │   \\.\pipe\hse_bridge│                       │
│         └──────────┬──────────┘                       │
│                    │                                  │
│  ┌─────────────────▼────────────────────────────┐    │
│  │              HSE ENGINE                       │    │
│  │                                               │    │
│  │  ┌─────────────┐  ┌────────────────────┐    │    │
│  │  │ Bridge       │  │ Command Queue      │    │    │
│  │  │ (Thread)     │──│ (SPSC Ring Buffer) │    │    │
│  │  │ reads pipe   │  │ 256 commands       │    │    │
│  │  └─────────────┘  └────────┬───────────┘    │    │
│  │                            │                  │    │
│  │  ┌─────────────────────────▼──────────┐     │    │
│  │  │ Main Thread (GL Context)            │     │    │
│  │  │                                     │     │    │
│  │  │  pollEvents()                       │     │    │
│  │  │  pumpCommands() ← queue pop         │     │    │
│  │  │  scene.update(dt)                   │     │    │
│  │  │  renderer.beginFrame()              │     │    │
│  │  │  renderer.renderScene()             │     │    │
│  │  │  renderer.endFrame()                │     │    │
│  │  │  renderer.incrementFrameCount()     │     │    │
│  │  │  window.swapBuffers()               │     │    │
│  │  └─────────────────────────────────────┘     │    │
│  │                                               │    │
│  │  ┌─────────────────────────────────────┐     │    │
│  │  │ Scene                                │     │    │
│  │  │  ├── primitives (vector)             │     │    │
│  │  │  │    └── each has stable ID         │     │    │
│  │  │  ├── idIndex (unordered_map)         │     │    │
│  │  │  └── cameras (vector)                │     │    │
│  │  └─────────────────────────────────────┘     │    │
│  │                                               │    │
│  │  ┌─────────────────────────────────────┐     │    │
│  │  │ Observation API                      │     │    │
│  │  │  ├── getObservation()                │     │    │
│  │  │  ├── getObjectObservation()          │     │    │
│  │  │  ├── getCameraObservation()          │     │    │
│  │  │  ├── getHealthObservation()          │     │    │
│  │  │  ├── getDeltaObservation()           │     │    │
│  │  │  └── getCaptureFrame()               │     │    │
│  │  └─────────────────────────────────────┘     │    │
│  └───────────────────────────────────────────────┘    │
└────────────────────────────────────────────────────────┘
```

## Thread Model

| Thread | Role | GL Access |
|--------|------|-----------|
| Main/GL Thread | Render loop, command execution, observation generation | YES |
| Bridge Reader Thread | Reads pipe, parses JSON, pushes to queue | NO |

## Data Flow

1. External sends JSON command via pipe
2. Reader thread parses and pushes to SPSC queue
3. Main thread pops queue (up to 32 per frame)
4. Command executes on GL thread (safe)
5. Scene mutates, renderer draws
6. External queries observation via pipe
7. Main thread generates observation from live scene state
8. Response sent back via pipe

## Key Invariants

- All GL calls happen on main thread
- Command queue is lock-free (single producer, single consumer)
- Object IDs are stable and monotonically increasing
- Scene revision increments only on successful mutations
- Snapshots stored for delta computation (max 128)
- Frame capture reads actual framebuffer (glReadPixels)
