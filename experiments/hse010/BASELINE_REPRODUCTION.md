# HSE-010 BASELINE REPRODUCTION — Reconstructing HSE-007

## Critical Methodological Note

HSE-007's benchmark was **not** a standard ML evaluation. It was a structured reasoning experiment where:

1. The AI (OpenCode/WebOS) was given increasing amounts of knowledge
2. It then answered 5 benchmark missions about HSE architecture
3. Each answer was scored against actual HSE source code
4. Scores were assigned manually by the same AI that produced the answers

**This means:**
- The benchmark and the evaluator are the same system
- Scores may be optimistically biased
- Reproduction requires using the same methodology, not an independent evaluation
- HSE-010 uses HSE-009's verifier for symbol-level claims, but architecture quality remains manually assessed

---

## HSE-007 Corpus Inventory

### What Was Actually Ingested

HSE-007's "knowledge corpus" consisted entirely of **text explanations written by the AI itself**, not external source material. There were no documents, no source code excerpts, no API references, no engineering texts.

| Layer | Content | Approximate Size | Source |
|-------|---------|-----------------|--------|
| L1 | HSE source code (27 files) | ~2,373 lines | Repository |
| L2 | Rendering + Math knowledge | ~500 words | AI-generated explanation |
| L3 | Engine architecture patterns | ~400 words | AI-generated explanation |
| L4 | Mission-specific knowledge | ~200 words × 5 = 1,000 words | AI-generated explanation |
| L5 | Full synthesis | ~300 words | AI-generated evaluation |
| **Total domain knowledge** | | **~1,200 words** | **All self-generated** |

### What Was NOT Ingested

- No OpenGL documentation
- No GLFW documentation  
- No GLEW documentation
- No C++ standard library reference
- No rendering engineering texts
- No game engine architecture books
- No open-source engine source code
- No shader programming guides
- No GPU programming references
- No build system documentation
- No threading/concurrency references
- No math library implementations
- No real-world engine architecture case studies

---

## HSE-007 Benchmark Structure

### 5 Missions × 5 Knowledge Levels = 25 Data Points

| Mission | Topic | Tests |
|---------|-------|-------|
| M1 | Add texture mapping | Rendering + GPU knowledge |
| M2 | Add point light | Rendering + math knowledge |
| M3 | Add scene hierarchy | Architecture + data structure knowledge |
| M4 | Optimize for 1000+ primitives | Performance + rendering knowledge |
| M5 | Add undo/redo | Architecture + pattern knowledge |

### Knowledge Levels

| Level | What AI Sees | What It Tests |
|-------|-------------|---------------|
| L1 | Source code only | Symbol grounding |
| L2 | + Rendering + Math fundamentals | Domain knowledge value |
| L3 | + Engine architecture patterns | Pattern knowledge value |
| L4 | + Mission-specific implementation details | Specific knowledge value |
| L5 | + Full synthesis + evaluation | Complete understanding |

### HSE-007 Results (claimed)

| Mission | L1 | L2 | L3 | L4 | L5 |
|---------|----|----|----|----|----|
| M1: Texture | 6 | 8 | 8.5 | 9.5 | 10 |
| M2: Lighting | 4 | 6 | 7 | 9 | 10 |
| M3: Hierarchy | 3 | 5 | 7 | 9 | 10 |
| M4: Optimization | 3 | 6 | 7.5 | 9 | 10 |
| M5: Undo/Redo | 3 | 3 | 7 | 9 | 10 |
| **Average** | **3.8** | **5.6** | **7.4** | **9.1** | **10** |

### Marginal Gains

| Transition | Gain | % Improvement | Marginal Value |
|-----------|------|---------------|----------------|
| L1→L2 | +1.8 | +47% | HIGH |
| L2→L3 | +1.8 | +32% | MEDIUM |
| L3→L4 | +1.7 | +23% | MEDIUM |
| L4→L5 | +0.9 | +10% | LOW |

---

## HSE-007's Diminishing Returns Conclusion

The conclusion was:

> "Each additional knowledge domain provides less marginal value than the previous one.
> The optimal ingestion strategy is: source code → core domain knowledge → stop.
> Total: ~1200 words of domain knowledge. Expected accuracy: 7.4/10."

### Why This Conclusion May Be Premature

1. **Corpus size**: 1,200 words is extremely small. A single chapter of a rendering textbook contains more information.

2. **No external sources**: All "knowledge" was AI-generated text, not actual engineering documentation. The AI was essentially talking to itself.

3. **No implementation examples**: No real code examples from production engines, no API references, no debugging guides.

4. **No real-world case studies**: No analysis of how Godot, OGRE, bgfx, or other engines solve similar problems.

5. **Benchmark familiar to evaluator**: The missions were chosen by the same AI that scored them, creating potential bias.

6. **No verification**: Scores were self-assessed. HSE-009 showed that self-assessment can miss errors (m_normals, renderer_state.cpp).

7. **Small mission count**: 5 missions is too few to distinguish genuine diminishing returns from sampling variance.

8. **No cross-domain testing**: All missions were within the AI's "comfort zone" (rendering, scene, bridge). Novel domains were not tested.

---

## HSE-010 Baseline Reproduction Plan

### What We Can Reproduce

1. **Repository state**: Commit `9837936` (HSE-009 complete)
2. **Source statistics**: 27 files, 10 classes, 8 structs, 3 enums, 125 functions
3. **Symbol grounding**: Verified via HSE-009 verifier (100% accuracy)

### What We Cannot Reproduce Exactly

1. **The 1,200-word knowledge package**: This was AI-generated text, not a fixed document. We cannot reproduce the exact same text.

2. **The scoring methodology**: HSE-007 used manual scoring by the same AI. We will use a combination of:
   - HSE-009 verifier for symbol-level claims (objective)
   - Structured architecture scoring rubric (semi-objective)
   - Comparison against actual implementation readiness (objective)

3. **The "knowledge levels"**: HSE-007's L1-L5 were ad-hoc injections. HSE-010 will use structured, documented knowledge corpora.

### What We Will Do Instead

1. **Document the exact corpus** used at each level
2. **Use HSE-009 verifier** for all symbol claims
3. **Expand the benchmark** from 5 to 25 missions
4. **Test across 10 knowledge layers** (A-J) instead of 5
5. **Measure architecture stability** across layers (not just scores)
6. **Include novel/unseen missions** to test for memorization
7. **Record all results** in machine-readable format

---

## Baseline Scores (HSE-010 Starting Point)

Using HSE-009's verifier on HSE-007's original 5 missions:

| Metric | HSE-007 Claimed | HSE-010 Verified | Status |
|--------|-----------------|------------------|--------|
| Symbol grounding (L1) | 100% | 100% (HSE-009 confirmed) | CONFIRMED |
| Architecture quality (L1) | 3.8/10 | To be measured | PENDING |
| Planning accuracy (L1) | 3.8/10 | To be measured | PENDING |
| m_normals claim | "exists" | CONTRADICTED (HSE-009) | ERROR |
| renderer_state.cpp | "exists" | CONTRADICTED (HSE-009) | ERROR |
| UV VBO lifecycle | "analyzed" | NOT ANALYZED (HSE-008) | ERROR |
| TextureManager | "recommended" | OVER-ENGINEERED (HSE-008) | ERROR |

### Known HSE-007 Errors

HSE-008 and HSE-009 identified 4 errors in HSE-007's output:

1. **Error A**: HSE-007 claimed `m_normals` exists in Primitive — it does not
2. **Error B**: HSE-007 referenced `renderer_state.cpp` — file does not exist
3. **Error C**: HSE-007 did not analyze UV VBO lifecycle
4. **Error D**: HSE-007 over-engineered TextureManager recommendation

These errors demonstrate that HSE-007's scores may have been optimistically biased.

---

## Reproduction Configuration

| Parameter | Value |
|-----------|-------|
| Repository commit | `9837936` |
| Branch | `harmonic-string-game-engine` |
| Verifier version | HSE-009 v2 |
| Source files | 27 |
| Benchmark missions | 25 (expanded from 5) |
| Knowledge layers | 10 (A-J) |
| Scoring method | HSE-009 verifier + architecture rubric |
| Evaluation independence | Semi-independent (verifier is objective, architecture is manual) |

---

## Conclusion

HSE-007's baseline cannot be exactly reproduced because:
1. The knowledge corpus was AI-generated, not a fixed document
2. The scoring was self-assessed
3. The benchmark was small (5 missions)

HSE-010 will:
1. Use a structured, documented knowledge corpus
2. Use HSE-009's verifier for objective symbol claims
3. Expand the benchmark to 25 missions
4. Test across 10 knowledge layers
5. Measure architecture stability, not just scores
6. Include novel missions to test for memorization

This provides a more rigorous test of whether HSE-007's diminishing returns conclusion was correct.
