# HSE-009 Correction Loop Demo

**Purpose:** Demonstrate that the verifier catches errors and enables correction.

## Scenario: HSE-008 Architecture Claims

HSE-008 produced 24 architectural claims about HSE's codebase. Some were correct, some were fabricated by the inference model. The verifier catches all of them.

### Step 1: Load Claims

```bash
python tools/architecture_verifier/verify.py --repo . --hse008
```

Claims loaded: 24 (14 valid + 10 invalid)

### Step 2: Scan Repository

The parser scans 27 source files, extracting:
- 10 classes, 8 structs, 3 enums
- 125 functions, 127 include dependencies
- Complete field inventories for all types

### Step 3: Verify Claims

Each claim is checked against the ground truth:

```
[OK]  C000 VERIFIED: File found: include/scene/primitive.h
[OK]  C001 VERIFIED: File found: src/renderer/renderer.cpp
...
[FAIL] C014 CONTRADICTED: Member 'Primitive::m_normals' NOT found
[FAIL] C015 CONTRADICTED: File NOT found: src/renderer/renderer_state.cpp
...
```

### Step 4: Catch HSE-008 Error A (m_normals)

**Claim:** "Primitive has a field `m_normals` (type: std::vector<Vec3>)"

**Verifier result:** CONTRADICTED — Primitive has 15 members, none named m_normals.

**Evidence:** `primitive.h:55-71` shows all fields. m_normals is not among them.

**Correction:** Remove m_normals from the HSE-008 implementation spec. Primitive does not store normals — they would need to be computed at render time if needed.

### Step 5: Catch HSE-008 Error B (renderer_state.cpp)

**Claim:** "Shader source code lives in src/renderer/renderer_state.cpp"

**Verifier result:** CONTRADICTED — File does not exist.

**Evidence:** Shader source is embedded in `src/renderer/renderer.cpp` at lines 13-31.

**Correction:** Update implementation spec to reference renderer.cpp, not renderer_state.cpp.

### Step 6: Catch Fabricated Symbols

Claims C016-C020 assert that m_textureID, m_texCoords, TextureManager, texture_manager.cpp, and m_uvbo exist. All are CONTRADICTED — they are planned additions, not existing code.

**Correction:** These are correctly identified as NEW symbols to be created during implementation, not existing ones to modify.

### Step 7: Re-verify After Corrections

After applying corrections, re-run the verifier:

```bash
python tools/architecture_verifier/verify.py --repo . --hse008
```

Result: 14/14 valid claims VERIFIED, 10/10 invalid claims CONTRADICTED. **100% accuracy.**

---

## Key Insight

The correction loop follows a simple pattern:

```
Inference → Claims → Verifier → Corrections → Re-verify → Gate
```

Without the verifier, HSE-008's implementation spec would have:
1. Tried to access m_normals (crash at compile time)
2. Tried to load renderer_state.cpp (build failure)
3. Assumed TextureManager exists (undefined reference)

The verifier catches all three categories of errors **before any implementation code is written**.
