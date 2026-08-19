# HSE-019 — Architecture

## 1. World Representation
The world is a single `hse::Scene` containing a hierarchy of `hse::Primitive` objects.

## 2. Room Coordinate System
- **Unit**: 1.0 = 1 meter (implied).
- **Origin**: Center of the floor.
- **Up**: +Y.
- **Forward**: -Z.
- **Room Bounds**: 6x6x3 meters.

## 3. Object Hierarchy
- `root_scene`
    - `env_group` (floor, walls, ceiling)
    - `furn_group`
        - `table`
        - `chair_1`
        - `chair_2`
    - `light_source` (visual representation)

## 4. Primitive Composition
- **Floor**: Quad (6x6)
- **Ceiling**: Quad (6x6)
- **Walls**: Quads with apertures for door/window.
- **Table**: Cube (Top) + 4 Cubes (Legs) or simplified block.
- **Chairs**: Cube (Seat) + Cube (Back) + Cubes (Legs).

## 5. Materials
- `mat_floor`: Wood-ish (Brown, high roughness)
- `mat_wall`: Plaster-ish (Off-white, medium roughness)
- `mat_furniture`: Wood-ish (Darker brown)
- `mat_light`: Emissive-looking (Yellow, no metallic)

## 6. Lighting
- **Type**: Single Point Light.
- **Position**: (0.0, 1.2, 0.0) — near the ceiling.
- **Color**: (1.0, 0.95, 0.8) — warm white.
- **Implementation**: `uLightPos`, `uLightColor`, `uLightIntensity` in fragment shader.

## 7. Camera Placement
- **Position**: (4.5, 2.0, 4.5)
- **Target**: (0.0, 0.0, 0.0)
- **FOV**: 60 degrees.
- **Mode**: Perspective.

## 8. Animation
- **Target**: `chair_1` or a dedicated "spinner" object.
- **Mechanism**: `m_rotationSpeed` (0.0, 45.0, 0.0) degrees/sec.

## 9. Persistence
- **Format**: `.hsc` (JSON).
- **Scope**: All primitives, materials, and (newly) transforms + lighting.

## 10. Observation
- Use `observe_scene` via bridge for machine verification.
- Use `capture_frame` for automated visual checking (if possible, otherwise human-only).

## 11. Correction Strategy
- 10-iteration loop: `IMPLEMENT -> BUILD -> TEST -> DIAGNOSE -> CORRECT`.

## 12. Human Visual Verification
- Launcher: `launch_hse.bat --project experiments/hse019/room.hsc`.
- Verification Gate: Wait for human confirmation of "Structure", "Furniture", "Materials", "Lighting", "Camera", "Animation".
