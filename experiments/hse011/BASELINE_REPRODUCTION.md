# HSE-011 BASELINE REPRODUCTION

## Reproduction Date: 2026-08-17

## Repository State

| Parameter | Value | Evidence |
|-----------|-------|----------|
| Repository | C:\Users\Rico\HarmonicStringEngine | Verified |
| Branch | harmonic-string-game-engine | Verified |
| HEAD commit | 59ab54a | git log |
| Source code changes | ZERO | git diff --stat HEAD: empty |
| Working tree | Clean | git status |

## HSE-009 Verifier Baseline (Re-run)

### Ground Truth Statistics

| Category | Count | HSE-010 Claimed | Reproduced? |
|----------|-------|-----------------|-------------|
| Source files | 27 | 27 | YES |
| Classes | 10 | 10 | YES |
| Structs | 8 | 8 | YES |
| Enums | 3 | 3 | YES |
| Functions | 125 | 125 | YES |
| Include dependencies | 127 | 127 | YES |
| Namespaces | 1 | 1 | YES |
| Primitive fields | 15 | 15 | YES |
| Primitive methods | 25 | 25 | YES |

### HSE-008 Regression (24 claims)

| Metric | HSE-010 Claimed | Reproduced? | Actual |
|--------|-----------------|-------------|--------|
| Total claims | 24 | YES | 24 |
| VERIFIED | 16 | YES | 16 |
| CONTRADICTED | 8 | YES | 8 |
| UNVERIFIED | 0 | YES | 0 |
| Accuracy | 100% | YES | 100% |

### Claim-by-Claim Verification

| Claim | HSE-010 | Reproduced | Match |
|-------|---------|------------|-------|
| C000: include/scene/primitive.h exists | VERIFIED | VERIFIED | YES |
| C001: src/renderer/renderer.cpp exists | VERIFIED | VERIFIED | YES |
| C002: src/scene/scene_builder.cpp exists | VERIFIED | VERIFIED | YES |
| C003: src/bridge/bridge.cpp exists | VERIFIED | VERIFIED | YES |
| C004: include/bridge/command_queue.h exists | VERIFIED | VERIFIED | YES |
| C005: Primitive class exists | VERIFIED | VERIFIED | YES |
| C006: Renderer class exists | VERIFIED | VERIFIED | YES |
| C007: Scene class exists | VERIFIED | VERIFIED | YES |
| C008: Primitive::m_color exists | VERIFIED | VERIFIED | YES |
| C009: Primitive::m_vertices exists | VERIFIED | VERIFIED | YES |
| C010: Primitive::m_vao exists | VERIFIED | VERIFIED | YES |
| C011: Primitive::m_id exists | VERIFIED | VERIFIED | YES |
| C012: PrimitiveType enum exists | VERIFIED | VERIFIED | YES |
| C013: hse namespace exists | VERIFIED | VERIFIED | YES |
| C014: m_normals does NOT exist | CONTRADICTED | CONTRADICTED | YES |
| C015: renderer_state.cpp does NOT exist | CONTRADICTED | CONTRADICTED | YES |
| C016: m_textureID does NOT exist | CONTRADICTED | CONTRADICTED | YES |
| C017: m_texCoords does NOT exist | CONTRADICTED | CONTRADICTED | YES |
| C018: TextureManager does NOT exist | CONTRADICTED | CONTRADICTED | YES |
| C019: texture_manager.cpp does NOT exist | CONTRADICTED | CONTRADICTED | YES |
| C020: m_uvbo does NOT exist | CONTRADICTED | CONTRADICTED | YES |
| C021: Scene owns m_primitives | VERIFIED | VERIFIED | YES |
| C022: Primitive owns m_vao | VERIFIED | VERIFIED | YES |
| C023: Primitive does NOT own m_normals | CONTRADICTED | CONTRADICTED | YES |

**All 24 claims match HSE-010 exactly. Baseline is reproducible.**

## HSE-010 Corpus Assessment (Verified)

### What HSE-010 Documented

| Metric | HSE-010 Claimed | Verified |
|--------|-----------------|----------|
| Corpus type | Self-generated text | CONFIRMED |
| Corpus size | ~1,200 words | CONFIRMED (approximate) |
| External sources used | 0 | CONFIRMED |
| Source code references | 0 external | CONFIRMED |
| Documentation references | 0 external | CONFIRMED |
| Engine studies | 0 | CONFIRMED |

### HSE-010 Deliverables Present

| Deliverable | Present | Verified |
|-------------|---------|----------|
| BASELINE_REPRODUCTION.md | YES | YES |
| CURRENT_KNOWLEDGE_AUDIT.md | YES | YES |
| KNOWLEDGE_DEPTH_LADDER.md | YES | YES |
| KNOWLEDGE_GAP_MAP.md | YES | YES |
| EXPANDED_BENCHMARK.md | YES | YES |
| ABLATION_RESULTS.md | YES (template) | YES |
| FALSE_DIMINISHING_RETURNS.md | YES | YES |
| NOVELTY_RESULTS.md | YES (template) | YES |
| SYMBOL_VERIFICATION_RESULTS.md | YES (template) | YES |
| ARCHITECTURE_CHANGE_MATRIX.md | YES (template) | YES |
| SATURATION_DECISION.md | YES (template) | YES |
| HSE010_FINDINGS.md | YES | YES |
| HSE010_HANDOFF.md | YES | YES |
| RESULTS.json | YES | YES |
| HSE-010_KNOWLEDGE_WIRING_MAP.json | YES | YES |

## Key Baseline Facts (Re-established)

1. **HSE-007's benchmark was 5 missions × 5 knowledge levels = 25 data points**
2. **HSE-007's "knowledge corpus" was ~1,200 words of AI-generated text**
3. **Zero external documentation or source code was used**
4. **HSE-007 scores were self-assessed by the same AI**
5. **HSE-009 identified 4 errors in HSE-007's output**
6. **HSE-010 expanded to 25 missions × 11 layers but did NOT execute the ablation**
7. **HSE-010's framework is complete but all results are templates**

## What HSE-011 Must Do Differently

1. **Use authoritative external sources** — not self-generated text
2. **Record source provenance** — URL, author, date, version, authority level
3. **Deduplicate** — don't count redundant information as independent knowledge
4. **Execute the ablation** — actually run benchmarks, not just create templates
5. **Use 50+ missions** — not 5 or 25
6. **Test novelty** — missions not in previous benchmarks
7. **Verify with HSE-009** — objective symbol verification
8. **Test for false saturation** — ablation per domain
9. **Classify the actual frontier** — what prevents improvement

## Phase 2 Progress — Source Archaeology

### Sources Cataloged

| Metric | Value |
|--------|-------|
| Total sources | 32 |
| Classification A (Primary Spec) | 1 |
| Classification B (Official Docs) | 10 |
| Classification C (Official Source) | 1 |
| Classification D (Mature Open-Source) | 16 |
| Classification E (Textbook) | 2 |
| Classification F (Tutorial) | 2 |
| Classification G (Untrusted) | 0 |

### Deliverables Created

| Deliverable | Status |
|-------------|--------|
| SOURCE_REGISTRY.json | COMPLETE |
| SOURCE_SELECTION.md | COMPLETE |
| AUTHORITATIVE_SOURCE_POLICY.md | COMPLETE |
| SOURCE_CATALOG.md | COMPLETE |
| SOURCE_CROSS_REFERENCE.md | COMPLETE |

### Key Findings

1. **HSE-007's gaps are well-covered**: All critical gaps identified in HSE-010 have authoritative sources
2. **Rendering has highest coverage**: 8 sources on OpenGL rendering pipeline
3. **Audio is the weakest domain**: Only 1 source (miniaudio)
4. **28 of 32 sources count toward saturation**: A-D classifications only
5. **Estimated 200K+ words of HSE-relevant content**: Far exceeds HSE-007's ~1,200 words

### Next Steps

- Phase 3: Begin deep knowledge ingestion using these sources
- Start with highest-coverage domains (rendering, math, engine architecture)
- Track word counts and knowledge quality per source

## Reproduction Confidence

**HIGH**. All baseline numbers from HSE-010 are reproducible. Phase 2 source archaeology is complete. The experiment is valid to proceed to Phase 3.
