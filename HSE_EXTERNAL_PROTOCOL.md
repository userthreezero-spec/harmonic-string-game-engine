# HSE External Protocol v1.0

**Purpose**: Define the structured interface between external cognitive systems (like WebOS) and the Harmonic String Engine.

**Transport**: Windows Named Pipe `\\.\pipe\hse_bridge`
**Format**: Newline-delimited JSON (NDJSON)

---

## Message Types

### Commands (External → HSE)

Every command includes:
- `cmd` (string): The command type
- `seq` (uint64): Sequence number for correlation

#### add_primitive
```json
{
  "cmd": "add_primitive",
  "seq": 1,
  "type": "Quad|Triangle|Cube|Sphere",
  "name": "wall_left",
  "px": 0, "py": 0, "pz": 0,
  "rx": 0, "ry": 0, "rz": 0,
  "sx": 1, "sy": 1, "sz": 1,
  "cr": 1, "cg": 1, "cb": 1
}
```

#### remove_primitive
```json
{
  "cmd": "remove_primitive",
  "seq": 2,
  "object_id": 42
}
```

#### set_position / set_rotation / set_scale / set_color
```json
{
  "cmd": "set_position",
  "seq": 3,
  "object_id": 42,
  "px": 1, "py": 2, "pz": 3
}
```

#### set_camera
```json
{
  "cmd": "set_camera",
  "seq": 4,
  "px": 5, "py": 5, "pz": 5,
  "rx": 0, "ry": 0, "rz": 0,
  "fov": 60
}
```

#### set_clear_color
```json
{
  "cmd": "set_clear_color",
  "seq": 5,
  "r": 0.1, "g": 0.1, "b": 0.15
}
```

### Observations (External → HSE → External)

#### observe_scene
```json
{
  "cmd": "observe_scene",
  "seq": 6
}
```

Returns `scene_observation` (see below).

#### observe_object
```json
{
  "cmd": "observe_object",
  "seq": 7,
  "object_id": 42
}
```

#### observe_camera
```json
{
  "cmd": "observe_camera",
  "seq": 8
}
```

#### observe_health
```json
{
  "cmd": "observe_health",
  "seq": 9
}
```

#### observe_delta
```json
{
  "cmd": "observe_delta",
  "seq": 10,
  "since_revision": 41
}
```

#### capture_frame
```json
{
  "cmd": "capture_frame",
  "seq": 11
}
```

---

## Response Types

### command_ack
```json
{
  "type": "command_ack",
  "seq": 1,
  "status": "accepted|failed",
  "object_id": 42,
  "revision": 42,
  "error": "object_not_found|queue_full|..."
}
```

### scene_observation
```json
{
  "type": "scene_observation",
  "observation_id": "obs_42",
  "scene_revision": 42,
  "timestamp": 123.456,
  "frame_count": 7890,
  "commands_processed": 42,
  "pending_commands": 0,
  "camera": {
    "position": [5, 5, 5],
    "target": [0, 0, 0],
    "fov": 60,
    "near": 0.1,
    "far": 1000,
    "aspect": 1.777
  },
  "objects": [
    {
      "id": 42,
      "name": "wall_left",
      "type": "quad",
      "position": [0, 0, -4],
      "rotation": [0, 0, 0],
      "scale": [8, 3, 1],
      "color": [0.6, 0.55, 0.5]
    }
  ]
}
```

### object_observation
```json
{
  "type": "object_observation",
  "id": 42,
  "name": "wall_left",
  "type": "quad",
  "position": [0, 0, -4],
  "rotation": [0, 0, 0],
  "scale": [8, 3, 1],
  "color": [0.6, 0.55, 0.5]
}
```

### camera_observation
```json
{
  "type": "camera_observation",
  "position": [5, 5, 5],
  "target": [0, 0, 0],
  "fov": 60,
  "near": 0.1,
  "far": 1000,
  "aspect": 1.777,
  "projection": "perspective"
}
```

### health_observation
```json
{
  "type": "health_observation",
  "renderer_running": true,
  "bridge_connected": true,
  "scene_objects": 5,
  "camera_count": 1,
  "frame_count": 7890,
  "scene_revision": 42,
  "commands_processed": 42,
  "last_command": "cmd_42",
  "last_command_status": "accepted",
  "clear_color": [0.1, 0.1, 0.15]
}
```

### scene_delta
```json
{
  "type": "scene_delta",
  "from_revision": 41,
  "to_revision": 42,
  "change_count": 1,
  "changes": [
    {
      "object_id": "42",
      "property": "position",
      "old": "[0,0,-4]",
      "new": "[1,0,-4]"
    }
  ]
}
```

### frame_capture
```json
{
  "type": "frame_capture",
  "width": 1280,
  "height": 720,
  "format": "rgba_base64",
  "data": "<base64-encoded RGBA pixel data>"
}
```

---

## Semantic Concepts

### Mission
A structured description of the desired scene state.

### Command
A single atomic operation on the scene (add, remove, modify object, camera, or renderer state).

### Acknowledgement
HSE's confirmation that a command was received and processed.

### Observation
HSE's report of current scene state, camera state, or engine health.

### Scene Revision
Monotonically increasing counter. Incremented on every successful scene mutation.

### Object ID
Stable uint64 identifier assigned on creation. Survives scene updates. Used for all subsequent operations on that object.

### Command Sequence
Unique uint64 sequence number. Returned in acknowledgements for correlation.

---

## Error Handling

| Error | Meaning |
|-------|---------|
| `object_not_found` | No object with given ID exists |
| `queue_full` | Command queue overflow (256 max) |
| `unsupported_operation` | Unknown command type |
| `malformed_message` | JSON parse error |

HSE continues running after any error. Invalid commands are rejected with `status: "failed"`.

---

## Integration Boundary

This protocol defines the clean interface between WebOS and HSE.

- **WebOS side**: Generates commands, interprets observations, reasons about scene state
- **HSE side**: Executes commands, renders scene, provides observations

Future WebOS integration should use this protocol without modifying HSE internals.
