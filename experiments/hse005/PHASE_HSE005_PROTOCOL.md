# HSE-005 Protocol Reference

See `C:\Users\Rico\HarmonicStringEngine\HSE_EXTERNAL_PROTOCOL.md` for the full protocol specification.

## Quick Reference

### Commands
| Command | Parameters | Response |
|---------|-----------|----------|
| `add_primitive` | type, name, position, rotation, scale, color | object_id, revision |
| `remove_primitive` | object_id | revision |
| `set_position` | object_id, px, py, pz | revision |
| `set_rotation` | object_id, rx, ry, rz | revision |
| `set_scale` | object_id, sx, sy, sz | revision |
| `set_color` | object_id, cr, cg, cb | revision |
| `set_camera` | px, py, pz, rx, ry, rz, fov | - |
| `set_clear_color` | r, g, b | - |
| `observe_scene` | - | scene_observation |
| `observe_object` | object_id | object_observation |
| `observe_camera` | - | camera_observation |
| `observe_health` | - | health_observation |
| `observe_delta` | since_revision | scene_delta |
| `capture_frame` | - | frame_capture |

### All commands accept `seq` for correlation.

### Object resolution
Commands accept either `object_id` (stable uint64) or `index` (deprecated, unstable).
