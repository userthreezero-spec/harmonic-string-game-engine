# HSE-006 Project Format Specification

## File Extension: `.hsc` (Harmonic String Canvas)

## Format: JSON (versioned)

```json
{
  "hsc_version": "1.0",
  "engine_version": "0.1.0",
  "world_revision": 42,
  "project": {
    "name": "MyProject",
    "description": "Project description"
  },
  "scene": {
    "name": "SceneName",
    "clear_color_r": 0.05,
    "clear_color_g": 0.05,
    "clear_color_b": 0.08,
    "objects": [
      {
        "id": "object_name",
        "type": "Quad|Triangle|Cube|Sphere",
        "position": [x, y, z],
        "rotation": [rx, ry, rz],
        "scale": [sx, sy, sz],
        "color": [r, g, b]
      }
    ],
    "cameras": [
      {
        "id": "camera_name",
        "type": "Perspective|Orthographic",
        "position": [x, y, z],
        "target": [tx, ty, tz],
        "fov": 60.0
      }
    ]
  }
}
```

## Fields

| Field | Type | Description |
|-------|------|-------------|
| `hsc_version` | string | Format version (for backwards compatibility) |
| `engine_version` | string | HSE version that created the project |
| `world_revision` | uint64 | Monotonically increasing revision counter |
| `project.name` | string | Human-readable project name |
| `project.description` | string | Project description |
| `scene.name` | string | Scene name (used as Scene constructor arg) |
| `scene.clear_color_r/g/b` | float | Background clear color |
| `scene.objects[]` | array | Scene primitives |
| `objects[].id` | string | Stable string identifier (mapped to Primitive.name) |
| `objects[].type` | string | Primitive type |
| `objects[].position` | [x,y,z] | World position |
| `objects[].rotation` | [rx,ry,rz] | Euler rotation (degrees) |
| `objects[].scale` | [sx,sy,sz] | Scale factors |
| `objects[].color` | [r,g,b] | Material color (0-1) |
| `scene.cameras[]` | array | Camera definitions |
| `cameras[].id` | string | Camera identifier |
| `cameras[].type` | string | Projection type |
| `cameras[].position` | [x,y,z] | Camera position |
| `cameras[].target` | [tx,ty,tz] | Look-at target |
| `cameras[].fov` | float | Field of view (degrees) |

## Project Directory Structure

```
ProjectName/
├── ProjectName.hsc      # Project manifest (this format)
├── scenes/              # Additional scene files
├── assets/              # Textures, models, etc.
├── generated/           # Auto-generated content
├── scripts/             # External scripts
├── state/               # Runtime state snapshots
└── build/               # Build artifacts
```

## Version History

| Version | Changes |
|---------|---------|
| 1.0 | Initial format: project metadata, scene objects, cameras |
