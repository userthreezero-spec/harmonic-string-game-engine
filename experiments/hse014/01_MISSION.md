# HSE-014 Mission: Transform Hierarchy

## Mission Statement
Implement a parent-child Transform Hierarchy for HSE primitives, converting the engine from a flat list of independently-positioned primitives into a proper scene graph where transforms propagate through parent-child relationships.

## Why Transform Hierarchy
The current engine has:
- Primitives with local position/rotation/scale (HSE-001)
- Material system with albedo/roughness/metallic (HSE-013)
- Scene storing flat list of primitives (HSE-001)
- Bridge with live manipulation (HSE-003)
- SceneBuilder with .hsc serialization (HSE-003)

What's missing: parent-child relationships. A door on a wall, a light on a ceiling, a robot arm with joints — all require nested transforms. This is the foundational scene graph layer.

## Subsystems Affected
1. **Primitive** — add parent pointer, children vector, world matrix computation
2. **Scene** — manage hierarchy (reparent, orphan detection, tree traversal)
3. **Renderer** — use world matrix instead of local matrix for rendering
4. **Bridge** — new commands: reparent, set_parent, get_world_transform, get_children
5. **SceneBuilder** — serialize/deserialize parent references in .hsc
6. **Tests** — hierarchy-specific unit tests

## Success Criteria
- Primitive can have a parent (shared_ptr to parent Primitive)
- Primitive can have children (vector of shared_ptr<Primitive>)
- World matrix = parent world matrix * local matrix
- Root primitives (no parent) use local matrix as world matrix
- Renderer uses world matrix for rendering
- Bridge supports reparent command
- .hsc saves/loads parent-child relationships
- Existing 29 tests still pass
- New hierarchy tests pass
- Build succeeds with zero warnings
