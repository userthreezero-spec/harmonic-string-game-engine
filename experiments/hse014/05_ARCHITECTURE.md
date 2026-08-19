# HSE-014 Architecture: Transform Hierarchy

## System Overview
Transform Hierarchy adds parent-child spatial relationships to HSE primitives, creating a scene graph. A primitive can be a child of another primitive, inheriting its transform. The world matrix of a child = parent's world matrix * child's local matrix.

## Existing Subsystem Reuse
- **Primitive**: extends existing class with parent/children/worldMatrix fields
- **Scene**: extends with hierarchy management (reparent, traversal)
- **Renderer**: modifies renderScene() to use world matrix
- **Bridge**: extends with reparent command
- **SceneBuilder**: extends .hsc format with parent field

## Data Model
```
Primitive
├── Local transform: m_position, m_rotation, m_scale (existing)
├── Parent: weak_ptr<Primitive> m_parent (NEW)
├── Children: vector<shared_ptr<Primitive>> m_children (NEW)
├── World matrix: Mat4 m_worldMatrix (NEW)
├── computeWorldMatrix(): m_worldMatrix = parent ? parent->m_worldMatrix * localMatrix : localMatrix
└── setParent(shared_ptr<Primitive> parent): adds/removes from parent's children

Scene
├── m_primitives: flat list (existing, all primitives including children)
├── getRoots(): returns primitives with no parent (NEW)
├── reparent(childID, parentID): changes parent relationship (NEW)
└── computeAllWorldMatrices(): traverses roots, computes world matrices (NEW)
```

## Ownership Model
- Primitive owns its children via `vector<shared_ptr<Primitive>>`
- Parent references children via `shared_ptr` (strong ownership)
- Child references parent via `weak_ptr` (non-owning, prevents cycles)
- Scene owns all primitives via `vector<shared_ptr<Primitive>>` (existing)

## Lifetime Model
- When parent is deleted, children become roots (parent weak_ptr expires)
- When child is reparented, it's removed from old parent's children, added to new parent's
- Scene owns all primitives regardless of hierarchy

## Threading Model
- Same as existing: main thread modifies, bridge reader thread reads commands
- World matrix computation happens on main thread before render

## GPU/Resource Implications
- Renderer uses m_worldMatrix instead of computing local matrix per primitive
- No new GPU resources (matrices are CPU-side)

## Serialization Implications
- .hsc objects gain optional "parent" field (name of parent primitive)
- On load: parse parent names, establish hierarchy after all primitives created

## Bridge Implications
- New command: `reparent` (child_id, parent_id)
- Observation includes "parent" field per object
- World transform query: `get_world_transform` returns world matrix

## Testing Strategy
- Unit: parent-child creation, world matrix computation, reparent
- Integration: nested primitives render correctly, .hsc round-trip
- Regression: existing 29 tests unchanged

## Exact New Symbols
| Symbol | File | Kind |
|--------|------|------|
| Primitive::m_parent | primitive.h | weak_ptr<Primitive> field |
| Primitive::m_children | primitive.h | vector<shared_ptr<Primitive>> field |
| Primitive::m_worldMatrix | primitive.h | Mat4 field |
| Primitive::setParent() | primitive.h/cpp | method |
| Primitive::getParent() | primitive.h | getter |
| Primitive::getChildren() | primitive.h | getter |
| Primitive::getWorldMatrix() | primitive.h | getter |
| Primitive::computeWorldMatrix() | primitive.h/cpp | method |
| Primitive::addChild() | primitive.h/cpp | method |
| Primitive::removeChild() | primitive.h/cpp | method |
| Scene::reparent() | scene.h/cpp | method |
| Scene::getRoots() | scene.h/cpp | method |
| Scene::computeAllWorldMatrices() | scene.h/cpp | method |
| Command::CMD_REPARENT | command_queue.h | enum value |
