# HSE-011: Novelty and Generalization Test

> **Purpose**: Test whether the knowledge corpus enables synthesis of concepts
> NOT explicitly provided as single examples. Each mission requires combining
> multiple knowledge domains to produce a coherent architectural proposal.

---

## Methodology

1. **Combination Missions**: 5 tasks requiring multi-domain synthesis
2. **Knowledge Domains**: List of domains available in the corpus
3. **Synthesis Quality**: Rated against what the corpus could plausibly enable
4. **Ground Truth**: HSE's actual architecture as reference

---

## Available Knowledge Domains in Corpus

| Domain | Sources | Depth |
|--------|---------|-------|
| OpenGL rendering | Multiple engine docs, tutorials | HIGH |
| Scene graph design | Urho3D, Magnum, custom engines | HIGH |
| Resource management | Multiple engine patterns | HIGH |
| Shader systems | GLSL docs, engine examples | HIGH |
| Platform abstraction | GLFW, SDL, windowing docs | HIGH |
| Memory management | C++ RAII, smart pointers, allocators | MEDIUM |
| Physics integration | Bullet, PhysX references | MEDIUM |
| Material/PBR | Filament, PBR guides | MEDIUM |
| Serialization | JSON, binary formats | MEDIUM |
| Input handling | GLFW, SDL, custom | HIGH |

---

## Combination Mission 1: Persistent Texture-Backed Material Attached to Scene Primitives with Safe GPU Lifetime

### Mission Description
Create a Material that owns a texture, is attached to a SceneObject, and is
safely cleaned up when the SceneObject is destroyed — even if the texture was
loaded asynchronously.

### Knowledge Domains to Synthesize

| Domain | Contribution |
|--------|-------------|
| Texture lifecycle | How textures are created, loaded, destroyed |
| Resource ownership | Who owns the texture: Material? TextureLoader? SceneObject? |
| Scene objects | How Primitives are attached to SceneObjects |
| GPU lifetime | RAII, move semantics, reference counting for GL handles |
| Asynchronous loading | Loading textures on background thread, completing on main thread |

### Synthesis Challenge
This requires understanding:
1. TextureLoader creates GL textures (domain: resource creation)
2. Material struct holds texture handle (domain: material system)
3. SceneObject owns Material (domain: scene graph)
4. GPU resources must be freed on GL context thread (domain: GPU lifetime)
5. Async loading means Material may reference not-yet-loaded texture (domain: async)

### Expected Synthesis Quality: **HIGH**

**Reasoning**: Each domain is well-represented in the corpus. The combination is non-trivial but follows logically from individual patterns. A well-trained model should produce:
- Material with TextureHandle (RAII wrapper)
- SceneObject with optional Material
- TextureLoader with async callback that populates Material
- Cleanup chain: SceneObject → Material → TextureHandle → glDeleteTextures

### What Would Go Wrong Without Multi-Domain Knowledge
- Texture might outlive Material (use-after-free)
- Material might reference dangling texture ID after async load failure
- GPU cleanup might happen on wrong thread
- SceneObject destruction might not propagate to texture cleanup

### Verification
- Create SceneObject with Material → load texture async → verify texture loads
- Destroy SceneObject → verify texture is freed (RenderDoc/GL debug)
- Verify no GL errors during lifecycle
- Verify Material with nullptr texture still renders (fallback)

---

## Combination Mission 2: Instanced Rendering with Per-Instance Materials and Frustum Culling

### Mission Description
Render 10,000 cubes with instanced drawing, where each instance can have a
different material (color/texture), and only visible instances are submitted
to the GPU.

### Knowledge Domains to Synthesize

| Domain | Contribution |
|--------|-------------|
| Instanced rendering | glDrawArraysInstanced, instance attributes |
| Material system | Per-instance material binding |
| Frustum culling | Camera frustum, bounding box testing |
| GPU resource management | Instance buffer creation/update |
| Camera system | Frustum planes from view-projection matrix |

### Synthesis Challenge
1. Instance transforms in a buffer (instanced rendering)
2. Instance materials in a buffer (material system extension)
3. Frustum culling removes invisible instances before draw call
4. Visible instance list is compacted into draw buffer
5. Camera provides frustum planes for culling

### Expected Synthesis Quality: **MEDIUM**

**Reasoning**: Instanced rendering and material systems are well-covered. Frustum culling requires camera frustum math (GLM matrices → frustum planes) which is a common pattern. The combination of all three in one system is advanced but derivable.

### What Would Go Wrong Without Multi-Domain Knowledge
- Instance buffer might not handle variable instance counts
- Material binding might not work with instanced draw calls
- Frustum culling might be per-object instead of per-instance
- GPU buffer updates might stall the pipeline

### Verification
- Render 10K cubes with 5 different materials
- Verify only visible cubes are drawn (camera looking away → fewer draw calls)
- Profile: single draw call for all visible instances
- Verify material variation within instances

---

## Combination Mission 3: Scene Serialization with Preserved Material References and Async Texture Loading

### Mission Description
Serialize a scene to JSON that includes material references, and deserialize it
back with textures loaded asynchronously while materials show a placeholder
until loading completes.

### Knowledge Domains to Synthesize

| Domain | Contribution |
|--------|-------------|
| Scene serialization | JSON scene graph format |
| Material system | Material → texture references |
| Async texture loading | Background loading with callback |
| Resource management | Placeholder textures, loading states |
| Scene graph | Reconstructing hierarchy from JSON |

### Synthesis Challenge
1. Scene graph serializes to JSON (serialization domain)
2. Materials serialize texture file paths (material domain)
3. On deserialize, textures load asynchronously (async domain)
4. Materials show placeholder until textures arrive (resource state domain)
5. Placeholder transitions to real texture atomically (GPU lifetime domain)

### Expected Synthesis Quality: **MEDIUM**

**Reasoning**: Serialization and async loading are both well-covered individually. The combination requires understanding loading states and placeholder transitions, which is a common game engine pattern but may not be explicitly in the corpus.

### What Would Go Wrong Without Multi-Domain Knowledge
- Textures might load synchronously (blocking main thread)
- Material might reference invalid texture ID during loading
- Placeholder might persist indefinitely if loading fails
- Serialization might not preserve loading order dependencies

### Verification
- Serialize scene with 10 objects, each with different texture
- Deserialize → verify placeholder colors appear
- Wait for async loads → verify real textures replace placeholders
- Verify no visual glitches during transition

---

## Combination Mission 4: Shader Hot-Reload with Material System and Render State Preservation

### Mission Description
Enable hot-reloading of shader source files at runtime (watch file changes),
recompile the shader, and re-apply all material uniform bindings without
losing render state.

### Knowledge Domains to Synthesize

| Domain | Contribution |
|--------|-------------|
| Shader compilation | Compile, link, error handling |
| Material system | Uniform bindings that reference shader |
| File watching | Detecting shader file changes |
| GPU resource management | Deleting old program, creating new |
| Render state | Preserving material bindings across shader swap |

### Synthesis Challenge
1. File watcher detects shader change (file system domain)
2. Shader recompile + link (shader domain)
3. Old program deleted, new program created (GPU lifetime domain)
4. All materials rebind uniforms to new program (material domain)
5. Render state preserved (no visual glitch during reload)

### Expected Synthesis Quality: **MEDIUM**

**Reasoning**: Shader compilation and material binding are well-covered. File watching is less common but derivable (GLFW doesn't provide it, would need platform API or polling). The state preservation across shader swap is the tricky part — requires understanding uniform locations are per-program.

### What Would Go Wrong Without Multi-Domain Knowledge
- Old shader might be deleted before new one is compiled (black frame)
- Materials might reference stale uniform locations
- File watching might not handle multiple shader files
- Shader compilation errors might crash the engine

### Verification
- Modify shader file → engine detects change
- Shader recompiles → rendering continues without interruption
- Errors in shader → engine logs error, keeps old shader
- Uniform bindings preserved across reload

---

## Combination Mission 5: Object Picking with Material-Aware Highlighting and Undo/Redo Selection

### Mission Description
Click on objects to select them, highlight them with a material override, and
support undo/redo of selection history — all integrated with the existing
renderer and input system.

### Knowledge Domains to Synthesize

| Domain | Contribution |
|--------|-------------|
| Object picking | Color-based picking (Mission 9) |
| Material system | Override material for highlight |
| Input handling | Mouse click processing |
| Command pattern | Undo/redo for selection history |
| Renderer | Applying highlight material without breaking pipeline |

### Synthesis Challenge
1. Picking pass identifies clicked object (picking domain)
2. Selected object gets material override (material domain)
3. Previous selection reverts to original material (state management)
4. Undo restores previous selection (command pattern domain)
5. Redo re-applies selection (command pattern domain)

### Expected Synthesis Quality: **MEDIUM-LOW**

**Reasoning**: Picking and material override are derivable from individual domains. Undo/redo via command pattern is a well-known design pattern but may not be explicitly in the corpus. The integration of all three requires careful state management.

### What Would Go Wrong Without Multi-Domain Knowledge
- Material override might permanently replace original material
- Undo might not restore original material state
- Redo might create duplicate highlight materials
- Picking might conflict with highlight rendering

### Verification
- Click object A → A highlighted
- Click object B → A reverts, B highlighted
- Undo → A highlighted, B reverts
- Redo → B highlighted, A reverts
- Multiple undos cycle through history correctly

---

## Synthesis Quality Summary

| Mission | Domains Combined | Expected Quality | Difficulty |
|---------|-----------------|-----------------|------------|
| 1. Texture-backed Material | 5 | HIGH | MEDIUM |
| 2. Instanced + Materials + Culling | 5 | MEDIUM | HIGH |
| 3. Serialization + Async Loading | 5 | MEDIUM | HIGH |
| 4. Shader Hot-Reload | 5 | MEDIUM | HIGH |
| 5. Picking + Highlight + Undo | 5 | MEDIUM-LOW | HIGH |

### Overall Assessment

**Can the corpus enable multi-domain synthesis?**

- **Mission 1 (HIGH)**: Yes — each domain is well-represented and the combination is natural
- **Missions 2-4 (MEDIUM)**: Partially — individual domains are covered but the specific combinations require inference beyond explicit examples
- **Mission 5 (MEDIUM-LOW)**: Unlikely without explicit command pattern examples — the undo/redo domain is the weakest link

**Key Finding**: The corpus enables synthesis for domains that are closely related (texture + material + scene) but struggles for domains with less overlap (command pattern + rendering). The generalization ceiling is approximately **3-domain synthesis** before quality degrades significantly.

### What Would Improve Generalization

1. **More cross-domain examples**: Show texture → material → scene → GPU lifecycle as one connected flow
2. **Explicit synthesis patterns**: Document how domains interact, not just what each domain does
3. **Command pattern examples**: Add undo/redo patterns from editor-like systems
4. **State management patterns**: How to preserve/restore render state across operations

---

*Generated as part of HSE-011 Knowledge Saturation Experiment*
