# HSE-005 Handoff

## Status: STATE B (External control + observation works, WebOS not connected)

## What Was Built

### Core Data Model
- **Stable Object IDs**: `Primitive` has `uint64_t m_id` (auto-assigned) + `std::string m_name`
- **Scene ID Lookup**: `Scene::findByID()`, `findByName()`, `removePrimitiveByID()`
- **ID Index**: `unordered_map<uint64_t, size_t>` for O(1) lookup

### Bridge Protocol
- **Command Sequence Numbers**: Every command has `seq`, every ack returns it
- **Single Ack**: Fixed HSE-004 double-response bug
- **Object ID Resolution**: Commands use `object_id` (stable) or `index` (unstable fallback)
- **6 Observation Types**: scene, object, camera, health, delta, frame_capture

### Observation System
- **Scene Snapshot**: Records full state at each observation
- **Delta Computation**: Diffs snapshots to report only changes
- **Framebuffer Capture**: glReadPixels → base64 RGBA
- **Health Telemetry**: renderer status, bridge status, frame count, revision, etc.

### External Controller
- **PowerShell script**: `phase005_probe/controller.ps1`
- **Continuous loop**: OBSERVE → ACT → OBSERVE → VERIFY
- **8 demo actions**: move walls, rotate, color, camera, scale
- **Failure tests**: invalid object, invalid command

## Files Modified
- `include/scene/primitive.h` - Added m_id, m_name, static s_nextID
- `src/scene/primitive.cpp` - Added ID assignment in constructor
- `include/scene/scene.h` - Added ID lookup, ID-based removal, idIndex map
- `src/scene/scene.cpp` - Implemented ID lookup, removal, index rebuild
- `include/scene/camera.h` - Added getTarget(), getFOV(), getNearPlane(), etc.
- `include/renderer/renderer.h` - Added getClearColor(), readPixels(), frame count
- `src/renderer/renderer.cpp` - Implemented getClearColor(), readPixels()
- `include/bridge/command_queue.h` - Added seq, objectID, sinceRevision to Command
- `include/bridge/bridge.h` - Full observation API, snapshot system
- `src/bridge/bridge.cpp` - Complete rewrite with observation, delta, capture
- `src/bridge_main.cpp` - Added frame count increment

## Files Created
- `phase005_probe/controller.ps1` - External cognitive controller
- `HSE_EXTERNAL_PROTOCOL.md` - WebOS integration protocol
- `experiments/hse005/` - 5 experiment documents

## How to Use

1. Start HSE bridge: `launch_bridge.bat` or desktop shortcut
2. Run controller: `.\phase005_probe\controller.ps1`
3. Watch objects appear and move in real-time

## For Future WebOS Integration

1. WebOS should implement the protocol in `HSE_EXTERNAL_PROTOCOL.md`
2. Connect via named pipe `\\.\pipe\hse_bridge`
3. Send commands as NDJSON
4. Parse observations from NDJSON responses
5. Use `object_id` for stable references
6. Use `seq` for command correlation
7. Use `observe_delta` for efficient updates
8. Use `capture_frame` for visual verification

## What WebOS Must NOT Do
- Must not modify HSE source code
- Must not assume HSE is part of WebOS
- Must not treat deterministic scripts as AI cognition
- Must not fabricate capabilities not demonstrated
