# PHASE 1211 — Home Architecture — HarmonicStringGameEngine

## Semantic Model (derived from engine capabilities, not invented physics)

Engine's only genuine relationship mechanism is `Primitive` hierarchy (`setParent` `primitive.h:39` → `computeWorldMatrix` `primitive.cpp:60` → `Scene::computeAllWorldMatrices` `scene.cpp:123` → `Renderer` `renderer.cpp:242`). No string `Relationship` table — hierarchy IS the relationship. We map house semantics onto this canonical mechanism with minimal extension (Group nodes as semantic parents).

## Hierarchy

```
HOUSE (Group "house_root" at 0,0,0)
├── FOUNDATION (Group "foundation" at 0,-0.4,0)
│   └── foundation_slab (Cube "foundation_slab" scale 10.4,0.3,8.4)
├── FLOOR_1 (Group "floor_1" at 0,0,0)
│   ├── floor_1_slab (Cube "floor_1_slab" 10,0.2,8 at y=-0.1)
│   ├── EXTERIOR WALLS (Group "floor_1_exterior")
│   │   ├── wall_f1_north (Cube 10,3,0.2 at 0,1.5,4)
│   │   ├── wall_f1_south (Cube 10,3,0.2 at 0,1.5,-4)
│   │   ├── wall_f1_east  (Cube 0.2,3,8 at 5,1.5,0)
│   │   └── wall_f1_west  (Cube 0.2,3,8 at -5,1.5,0)
│   ├── INTERIOR WALLS (Group "floor_1_interior")
│   │   ├── interior_f1_a (Cube 0.15,3,5 at -1,1.5,0) // divides rooms
│   │   └── interior_f1_b (Cube 5,3,0.15 at 0,1.5,1.0)
│   ├── ROOMS (Group "floor_1_rooms") — logical tiles, visually distinct floor quads
│   │   ├── room_f1_living  (Quad "room_f1_living" 4,0,4 at -2.5,0.01,0)  material living
│   │   ├── room_f1_kitchen (Quad "room_f1_kitchen" 4,0,4 at 2.5,0.01,-1.5)
│   │   └── room_f1_bed     (Quad "room_f1_bed" 3,0,3 at 2.5,0.01,1.5)
│   ├── DOORS (Group "floor_1_doors")
│   │   ├── door_f1_front (Cube 1,2,0.1 at 0,1,4)   // opening marker
│   │   ├── door_f1_interior_a (Cube 0.9,2,0.1 at -1,1,0.5)
│   │   └── door_f1_interior_b (Cube 0.9,2,0.1 at 1,1,1.0)
│   └── WINDOWS (Group "floor_1_windows")
│       ├── window_f1_north (Quad 1.2,1,0 at -3,1.5,4.01)
│       ├── window_f1_south (Quad 1.2,1,0 at 3,1.5,-4.01)
│       ├── window_f1_east  (Quad 0,1,1.2 at 5.01,1.5,0)
│       └── window_f1_west  (Quad 0,1,1.2 at -5.01,1.5,2)
├── FLOOR_2 (Group "floor_2" at 0,3.0,0)
│   ├── floor_2_slab (Cube "floor_2_slab" 10,0.2,8 at y=-0.1) // also floor_1 ceiling
│   ├── EXTERIOR WALLS (Group "floor_2_exterior")
│   │   ├── wall_f2_north (Cube 10,3,0.2 at 0,1.5,4)
│   │   ├── wall_f2_south (Cube 10,3,0.2 at 0,1.5,-4)
│   │   ├── wall_f2_east  (Cube 0.2,3,8 at 5,1.5,0)
│   │   └── wall_f2_west  (Cube 0.2,3,8 at -5,1.5,0)
│   ├── INTERIOR WALLS (Group "floor_2_interior")
│   │   ├── interior_f2_a (Cube 0.15,3,6 at 1,1.5,0)
│   │   └── interior_f2_b (Cube 4,3,0.15 at 0,1.5,-1.5)
│   ├── ROOMS (Group "floor_2_rooms")
│   │   ├── room_f2_master (Quad 5,0,4 at -1,0.01,0)
│   │   ├── room_f2_bed2   (Quad 4,0,3 at 2.5,0.01,1)
│   │   └── room_f2_bath   (Quad 3,0,2.5 at 2,0.01,-2)
│   ├── DOORS (Group "floor_2_doors")
│   │   ├── door_f2_hall (Cube 0.9,2,0.1 at 1,1,0.5)
│   │   └── door_f2_master (Cube 0.9,2,0.1 at -1,1,1.5)
│   └── WINDOWS (Group "floor_2_windows")
│       ├── window_f2_north (Quad 1.2,1,0 at 0,1.5,4.01)
│       ├── window_f2_east  (Quad 0,1,1.2 at 5.01,1.5,1)
│       └── window_f2_south (Quad 1.2,1,0 at -2,1.5,-4.01)
├── STAIRS (Group "stairs" at -4,0,0, parent house_root)
│   ├── stair_01 .. stair_10 (Cubes 2,0.3,0.8 stepping y+=0.3, z+=0.7)
│   └── stair_rail (Cube thin along stairs)
├── ROOF (Group "roof" at 0,6.15,0, parent house_root)
│   ├── roof_slab (Cube "roof_slab" 10.6,0.3,8.6 at y=0)
│   ├── roof_peak_a (Cube 10.6,0.2,4.3 at 0,0.5,2.15 rot x=-20deg)
│   └── roof_peak_b (Cube 10.6,0.2,4.3 at 0,0.5,-2.15 rot x=20deg)
└── LIGHTS: main_light (house interior), sun_light (exterior)
```

## Proportions (coherent)
- Footprint 10 x 8, wall height 3 per floor, total height ~6.5, foundation 0.3, roof 0.3+0.5 peak. No floating: every Group at exact floor heights, walls centered at y=1.5 within floor, slabs at y=-0.1 local to floor Group → world y = floorY + wallY. Vertical alignment per-floor is structurally coherent via hierarchy.

## Relationship Vocabulary (mapped to hierarchy)

| Semantic | Engine realization |
|----------|--------------------|
| `wall part_of floor` | `wall.setParent(floorGroup)` `primitive.cpp:45` |
| `room part_of floor` | `roomQuad.setParent(floorGroup)` |
| `floor part_of house` | `floorGroup.setParent(houseRoot)` |
| `door connects room` | Door cube child of `floor_doors` Group child of `floor`, logically adjacent to room quad; runtime collision `checkCollision` can verify door near rooms (honest adjacency, not fake portal) |
| `window belongs_to wall` | `windowQuad.setParent(wallPrimitive)` or sibling under same floor Group with name convention `window_f1_* → wall_f1_*` documented; parent method provides propagation parity |
| `stair connects floor_1/floor_2` | `stairs Group` is child of `house`, steps span y 0→3.0 bridging floors; moving `stairs` Group preserves relative steps |
| `roof supported_by walls` | `roof Group` is sibling of floors under `house`; moving `floor_2` does NOT auto-move roof (correct: roof is supported-by walls conceptually, but in engine we keep roof as `house` child so house-level move propagates to all; wall-level move does not auto-move roof — documented honestly) |

No invented semantic system: we reuse `Primitive` parent hierarchy directly. Where spec demands `supported_by`, we document structural intent and note that destruction of dependency would require parent children propagation (moving house) rather than per-wall.

## Harmonic Structure Mapping (honest)

Engine's only live harmonic-like machinery is `Scene::update rotationSpeed` and `Camera orbit` (geometric). We map:
- **Parametric harmonic:** repeated structures (windows, stairs) share same material/scale, derived parametrically in builder loop (10 stairs with `y=i*0.3, z=i*0.35`).
- **Propagation demonstration:** changing structural parameter on a Group parent (e.g., `floor_1.setPosition({dx,0,0})`) → `computeWorldMatrix` propagates shift to all descendants (walls, rooms, doors, windows) → `Renderer` reflects via `worldMatrix` → visually coherent house slides as unit. This is **relationship-driven transform propagation** (PROVEN). We do NOT fake per-object manual updates; we mutate parent only and rely on engine's `markDirtyRecursive` + `computeAllWorldMatrices`.

## Colors / Materials (distinguishable rooms)
- `mat_foundation` #5a4a3a, `mat_floor` #8a7a65, `mat_wall_ext` #c8bcb0, `mat_wall_int` #a9a29a, `mat_roof` #7a2a2a, `mat_stairs` #6b5a4a, plus per-room `mat_living` #8ec, `mat_kitchen` #eca, `mat_bed` #aef, etc. Walls distinguish via `Material.albedo` consumed by `renderer.cpp:267` `material->apply`.

No decorative geometry before structural coherence is added; roof/foundation sized to fully cover walls (10.4/10.6 > 10).

