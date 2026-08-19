# HSE-021 — Repository Archaeology

## Technical Baseline (PROVEN)

### 1. Scene Graph (`scene.h`)
- `hse::Scene`: Manages `m_primitives` (objects) and `m_lights`.
- Support for `id`, `name`, `position`, `rotation`, `scale`, and `rotation_speed`.
- Hierarchy: `reparent` method exists; `Primitive` has `m_parent`.

### 2. Manipulation (`workspace_main.cpp`)
- Manual controls exist for position, rotation, and scale.
- Bridge (`bridge.h`) allows remote mutation via named pipes.

### 3. Bridge Protocol (`HSE_EXTERNAL_PROTOCOL.md`)
- `set_position`: `{"cmd": "set_position", "seq": N, "object_id": ID, "px": X, "py": Y, "pz": Z}`
- `set_rotation`: Similar to position.
- `set_rotation_speed`: (Note: Protocol docs say `set_color`, but `bridge.cpp` supports `set_rotation_speed`).

### 4. Persistence (`scene_builder.h`)
- `.hsc` format (JSON).
- Supports full transform state and lighting.

## Symbols Verified
- `hse::Scene::findByID`
- `hse::Scene::findByName`
- `hse::Primitive::setPosition`
- `hse::Primitive::setRotation`
- `hse::Bridge::pumpCommands`

## Semantic Limitations
- No native semantic tags in HSE.
- "Role" must be INFERRED from name, material, or geometry.
