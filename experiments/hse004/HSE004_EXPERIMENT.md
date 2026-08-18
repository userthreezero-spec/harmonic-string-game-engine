# HSE-004: Live Native HSE Mission Bridge

## Classification: STATE B (Partial)

## Summary

Built a live bridge architecture enabling external processes to send structured commands to the running HSE renderer in real-time via Windows named pipes. The bridge demonstrates command-driven scene manipulation with immediate visual feedback.

## What Was Built

### Bridge Architecture
- **Named pipe transport**: `\\.\pipe\hse_bridge` - bidirectional, same-machine IPC
- **SPSC command queue**: Lock-free single-producer single-consumer ring buffer (256 commands)
- **Bridge thread**: Reads pipe on background thread, pushes to queue
- **Main thread pump**: Executes commands on GL thread between `pollEvents()` and `scene->update(dt)`
- **Command set**: add_primitive, remove_primitive, set_position/rotation/scale/color, set_camera, set_clear_color, scene_state, health

### Files Created
- `include/bridge/command_queue.h` - SPSC ring buffer with atomic head/tail
- `include/bridge/pipe_transport.h` - Named pipe server header
- `src/bridge/pipe_transport.cpp` - Win32 named pipe implementation
- `include/bridge/bridge.h` - Bridge controller header
- `src/bridge/bridge.cpp` - Thread launch, command parsing, execution
- `src/bridge_main.cpp` - Bridge entry point with scene + bridge
- `launch_bridge.bat` - Desktop launcher
- `phase004_probe/bridge_client.py` - Python client (requires pywin32)
- `phase004_probe/bridge_client.ps1` - PowerShell client (no dependencies)

### Files Modified
- `CMakeLists.txt` - Added hse_bridge library and hse_bridge_app target

## How It Works

```
External Process (PowerShell/Python)
    ↓ line-delimited JSON via named pipe
Bridge Thread (blocking ReadFile on pipe)
    ↓ SPSC ring buffer (lock-free)
Main Thread (command pump)
    ↓ execute GL calls (uploadGPU, addPrimitive, etc.)
OpenGL renders updated scene
    ↓
Visible result in window
```

## Verification

- Build: 5 executables (hse_main, hse_room_demo, hse_bridge_app, hse_test_math, hse_test_scene)
- Tests: 19/19 pass (Math + Scene)
- Bridge executable: 356KB, built successfully
- Desktop shortcut: "HSE Live Bridge.lnk" created

## Usage

1. Start bridge: `launch_bridge.bat` or double-click "HSE Live Bridge" desktop shortcut
2. In another terminal: `.\phase004_probe\bridge_client.ps1 -Demo`
3. Watch objects appear in the HSE window in real-time

## Limitations (STATE B)

- No visual verification from OpenCode (requires human observation)
- No framebuffer capture (observation via scene_state JSON export only)
- No WebOS integration (bridge is HSE-side only, WebOS controller is external)
- No error recovery if bridge thread crashes
- No authentication on named pipe
