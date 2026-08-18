# HSE-008 FAILURE ANALYSIS

## Failures Identified

### Failure 1: HSE-007 Claimed m_normals Exists

**Classification**: REPOSITORY-GROUNDING GAP

**What happened**: HSE-007's SYMBOL_DOMAIN_MAP.md stated that Primitive has `m_normals` field and that normals "exist but are unused." This is incorrect. The actual Primitive class (primitive.h) has no `m_normals` field.

**Evidence**: primitive.h:42-43 shows only `m_vertices` and `m_indices`. No normals.

**Impact**: If WebOS had relied on this claim, it would have tried to use existing normals for lighting calculations, which would have failed.

**Root cause**: HSE-007 inferred the existence of normals from the presence of a lighting-related field (m_color) and standard 3D engine conventions. It did not verify against the actual source.

**Prevention**: Always verify claims against actual source code before using them in architecture.

### Failure 2: HSE-007 Referenced renderer_state.cpp

**Classification**: SYMBOL-GROUNDING GAP

**What happened**: HSE-007 referenced "renderer_state.cpp" as the location of shader source code. The actual file is `renderer.cpp`. There is no `renderer_state.cpp`.

**Evidence**: renderer.cpp:13-31 contains the hardcoded shader strings.

**Impact**: If WebOS had searched for renderer_state.cpp, it would not have found the shader source.

**Root cause**: HSE-007 inferred the filename from the pimpl pattern (RendererState struct) and assumed a separate source file. The actual implementation puts RendererState in renderer.cpp.

**Prevention**: Always verify file paths against actual repository structure.

### Failure 3: HSE-007 Did Not Identify UV VBO Tracking Gap

**Classification**: ARCHITECTURE GAP

**What happened**: HSE-007's texture mission chain identified that a UV VBO is needed, but did not identify that the current Primitive class has no field to track the UV VBO handle (only m_vbo for position).

**Impact**: The UV VBO would leak if the primitive is re-uploaded (though this is acceptable for first implementation).

**Root cause**: HSE-007 focused on the happy path (create once, render many times) and did not analyze the full lifecycle.

**Prevention**: Trace the full lifecycle of GPU resources (create → use → destroy).

### Failure 4: WebOS Initially Over-Engineered TextureManager

**Classification**: ARCHITECTURAL GAP (self-corrected)

**What happened**: The initial WebOS architecture proposed a TextureManager with reference counting. On verification, this was found to be more complex than needed for the bounded mission.

**Impact**: None — the over-engineering was caught during verification.

**Root cause**: WebOS defaulted to "industry best practice" rather than "minimum viable architecture."

**Prevention**: Explicitly bound architecture to mission scope.

## Failure Classification Summary

| Failure | Classification | Impact | Root Cause |
|---------|---------------|--------|------------|
| m_normals claim | REPOSITORY-GROUNDING | Would cause implementation failure | Inference without verification |
| renderer_state.cpp | SYMBOL-GROUNDING | Would cause file-not-found | Inference from pattern |
| UV VBO tracking | ARCHITECTURE | Resource leak (acceptable) | Lifecycle analysis incomplete |
| TextureManager over-engineering | ARCHITECTURE | Complexity increase (caught) | Industry defaults vs. mission scope |

## What Would Have Happened Without Verification

If WebOS had produced the architecture WITHOUT repository verification:

1. **Would have tried to use m_normals** → compile error
2. **Would have looked for renderer_state.cpp** → file not found
3. **Would have built full TextureManager with ref counting** → unnecessary complexity
4. **Would have missed UV VBO lifecycle issue** → potential resource leak

**Total implementation failures prevented by verification**: 2
**Total complexity reductions from verification**: 1
**Total lifecycle issues caught**: 1

## Conclusion

The failures are all in the **inference** category — HSE-007 made plausible inferences that turned out to be incorrect. The verification phase caught all of them. This demonstrates that:

1. **Inference is dangerous** without verification
2. **Verification is essential** before implementation
3. **The architecture-to-symbol trace** enables verification at every level
4. **WebOS's value is in the verification**, not just the architecture
