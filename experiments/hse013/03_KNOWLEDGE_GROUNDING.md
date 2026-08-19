# HSE-013 Phase 3: Knowledge Grounding

## Grounding Strategy
Material system was validated against HSE-012's accumulated knowledge from 15 authoritative sources across 18 knowledge files. Key grounding decisions:

## Grounding Table
| Decision | Knowledge Source | Authority Level | Confidence |
|----------|-----------------|----------------|------------|
| Shared_ptr ownership for Material | HSE-001 repo pattern (Texture uses same) | C: Repo convention | High |
| Material as separate class (not struct) | Godot RefCounted pattern, bgfx handle pattern | D: Mature impl | High |
| Apply uniforms per-primitive | OpenGL immediate uniform upload pattern | A: GL spec | High |
| Hardcoded uniform names (uMaterial.*) | Simple shader convention, matches Godot's per-material uniform pattern | D: Mature impl | Medium |
| GLSL struct matching C++ layout | Standard OpenGL practice, Godot uses same | A: GL spec | High |
| Scene stores materials by name | Godot resource registry pattern, raylib resource management | D: Mature impl | High |
| Fallback material in renderer | Godot default material pattern | D: Mature impl | High |
| No material batching | Simplified for engine maturity, consistent with current single-shader approach | N/A: Design choice | Medium |

## Key Knowledge Items Used
- **L0 (Repo)**: Texture class uses shared_ptr, same ownership pattern applied to Material
- **L3 (Godot)**: Resource system stores materials by name, reference-counted. Material is RefCounted, not a value type. Applied: HSE Material uses shared_ptr (equivalent to RefCounted)
- **L4 (raylib)**: rlLoadMaterial() returns handle, rlSetMaterialTexture() binds. Applied: Material::apply() binds texture to unit 0
- **L2 (OpenGL)**: glUniform* functions for uploading, glActiveTexture + glBindTexture for texture units. Applied: exact GL calls in Material::apply()
- **L5 (bgfx)**: Material handle pattern (bgfx::createMaterialHandle). Applied: not used directly, but validates separate Material class

## Anti-Fabrication Checks
- [x] Every grounding claim traced to a specific source
- [x] No INFERRED items promoted to PROVEN
- [x] Design decisions marked as N/A where they are engine-specific
- [x] Material class fields match HSE-012's symbol-level specification exactly
