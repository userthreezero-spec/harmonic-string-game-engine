# HSE-004 ARCHAEOLOGY

**Date:** 2026-08-17

---

## Current Architecture

Single-threaded render loop: `pollEvents → update → beginFrame → renderScene → endFrame → swapBuffers`

All OpenGL calls happen on the main thread. No IPC, no networking, no threading, no JSON parsing.

## What Can Be Reused

| Component | Reuse |
|-----------|-------|
| `Scene::addPrimitive()` | Runtime object insertion |
| `Primitive(type) + uploadGPU()` | Full GPU upload pipeline |
| `Camera::setPosition/lookAt/FOV` | Runtime camera manipulation |
| `SceneBuilder::exportState()` | JSON state export |
| `Vec3/Mat4` | Math sufficient |
| `PrimitiveType` enum | Triangle/Quad/Cube available |
| `hse_core` static library | All engine code reusable |

## What Must Be Built

| Component | Reason |
|-----------|--------|
| Command queue (SPSC ring buffer) | Thread-safe command delivery |
| Named pipe transport | Local IPC between WebOS and HSE |
| JSON parser for commands | No parser exists |
| Thread synchronization | No atomics/mutexes in codebase |
| Bridge main loop | New entry point with bridge |
| Object ID system | For update/remove by name |

## Bridge Architecture

```
WebOS (external)
    ↓ JSON commands via named pipe
Bridge Thread (pipe reader)
    ↓ SPSC ring buffer
Main Thread (command pump)
    ↓ execute GL calls
Scene mutation
    ↓
OpenGL render
    ↓
Visible result
    ↓
Scene state export
    ↓
WebOS polls state
```

## Transport: Named Pipe `\\.\pipe\hse_bridge`

- Same-machine, no network overhead
- Bidirectional (command in, state out)
- Win32 API, no external dependencies
- Line-delimited JSON protocol

## Thread Safety

- GL context owned by main thread exclusively
- Bridge thread only reads pipe and pushes to queue
- Main thread pops queue and executes GL commands
- SPSC ring buffer with atomics (no mutex)
