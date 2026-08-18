# HSE-005 Findings

## What Works

1. **Stable Object IDs**: Every primitive gets a monotonically increasing uint64 ID on creation. IDs survive scene updates. Objects can be referenced by ID in all subsequent commands.

2. **Bidirectional Communication**: Named pipe is full-duplex. HSE receives commands AND sends observations back. No transport changes needed from HSE-004.

3. **Command Correlation**: Every command has a `seq` number. Every ack includes the same `seq` for matching.

4. **Scene Revision Tracking**: Scene revision increments on every successful mutation. Included in all observations.

5. **Full Observation Protocol**: observe_scene, observe_object, observe_camera, observe_health, observe_delta, capture_frame.

6. **Framebuffer Capture**: glReadPixels reads actual rendered pixels, flips vertically for correct orientation, encodes as base64.

7. **Delta/Diff Observations**: Can request only changes since a known revision, reducing bandwidth.

8. **Failure Conditions**: Invalid objects, invalid commands, and queue overflow all fail gracefully with structured error responses.

## Architecture

```
External Controller
    ↓ NDJSON via named pipe
Bridge Thread (reader)
    ↓ SPSC ring buffer
GL/Main Thread (pump)
    ↓ execute on GL thread
Scene mutation + render
    ↓
Observation API
    ↓ NDJSON via named pipe
External Controller
```

## Key Design Decisions

1. **ID-based removal** instead of index-based (indices shift on removal)
2. **Single ack per command** (fixed HSE-004 double-response bug)
3. **Snapshot-based deltas** (store periodic snapshots, diff between them)
4. **Base64 frame capture** (self-contained, no file I/O required)
5. **seq-based correlation** (external systems can match acks to commands)

## What Does NOT Work Yet

1. **No actual WebOS connection** - bridge is HSE-side only
2. **No harmonic concepts** - protocol allows them but none implemented
3. **No real AI cognition** - controller is deterministic scripts
4. **No scene hierarchy** - flat primitive list only
5. **No material system** - color is the only material property
