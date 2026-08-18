# HSE-009 Accuracy Report

**Date:** 2026-08-17
**Verifier Version:** v2 (robust regex parser)
**Repository:** harmonic-string-game-engine @ a91f530

## Executive Summary

The HSE-009 ground-truth architecture verifier achieves **100% accuracy** on both the HSE-008 regression test set and the independent symbol accuracy benchmark.

---

## Test Set 1: HSE-008 Regression (24 claims)

**Purpose:** Verify that the verifier correctly catches all 4 known HSE-008 errors and validates all valid claims.

| Metric | Result |
|--------|--------|
| Total Claims | 24 |
| Valid Claims (should be VERIFIED) | 14 |
| Invalid Claims (should be CONTRADICTED) | 10 |
| Verified | 14 |
| Contradicted | 10 |
| Unverified | 0 |
| **Accuracy** | **100%** |

### HSE-008 Error Detection

| Error | Expected | Actual | Status |
|-------|----------|--------|--------|
| (A) m_normals does not exist | CONTRADICTED | CONTRADICTED | PASS |
| (B) renderer_state.cpp does not exist | CONTRADICTED | CONTRADICTED | PASS |
| (C) m_textureID does not exist yet | CONTRADICTED | CONTRADICTED | PASS |
| (D) m_texCoords does not exist yet | CONTRADICTED | CONTRADICTED | PASS |

### Valid Claim Verification

| Claim | Expected | Actual | Status |
|-------|----------|--------|--------|
| include/scene/primitive.h exists | VERIFIED | VERIFIED | PASS |
| src/renderer/renderer.cpp exists | VERIFIED | VERIFIED | PASS |
| src/scene/scene_builder.cpp exists | VERIFIED | VERIFIED | PASS |
| src/bridge/bridge.cpp exists | VERIFIED | VERIFIED | PASS |
| include/bridge/command_queue.h exists | VERIFIED | VERIFIED | PASS |
| Primitive class exists | VERIFIED | VERIFIED | PASS |
| Renderer class exists | VERIFIED | VERIFIED | PASS |
| Scene class exists | VERIFIED | VERIFIED | PASS |
| Primitive::m_color exists | VERIFIED | VERIFIED | PASS |
| Primitive::m_vertices exists | VERIFIED | VERIFIED | PASS |
| Primitive::m_vao exists | VERIFIED | VERIFIED | PASS |
| Primitive::m_id exists | VERIFIED | VERIFIED | PASS |
| PrimitiveType enum exists | VERIFIED | VERIFIED | PASS |
| hse namespace exists | VERIFIED | VERIFIED | PASS |

---

## Test Set 2: Symbol Accuracy Benchmark (15 claims)

**Purpose:** Independent test set measuring precision and recall on a mix of valid and invalid claims.

| Metric | Result |
|--------|--------|
| Total Claims | 15 |
| Valid Claims (should be VERIFIED) | 10 |
| Invalid Claims (should be CONTRADICTED) | 5 |
| Verified | 10 |
| Contradicted | 5 |
| Unverified | 0 |
| **Precision** | **100%** (10/10 valid claims verified) |
| **Recall** | **100%** (5/5 invalid claims contradicted) |
| **F1 Score** | **1.00** |

### Confusion Matrix

|  | Verified | Contradicted |
|--|----------|-------------|
| **Actually Valid** | 10 (TP) | 0 (FN) |
| **Actually Invalid** | 0 (FP) | 5 (TN) |

---

## Ground Truth Statistics

| Category | Count |
|----------|-------|
| Source files | 27 |
| Classes | 10 |
| Structs | 8 |
| Enums | 3 |
| Functions | 125 |
| Include dependencies | 127 |
| Namespaces | 1 |
| **Primitive fields** | **15** |
| **Primitive methods** | **25** |

### Primitive Field Inventory (Verified)

```
s_nextID: uint64_t
m_id: uint64_t
m_name: std::string
m_type: PrimitiveType
m_position: Vec3
m_rotation: Vec3
m_scale: Vec3
m_color: Vec3
m_rotationSpeed: Vec3
m_vertices: std::vector<float>
m_indices: std::vector<uint32_t>
m_vao: unsigned int
m_vbo: unsigned int
m_ebo: unsigned int
m_uploaded: bool
```

---

## Parser Bug Fixes Applied

1. **File path normalization:** All stored file paths normalized to forward slashes (Windows compatibility)
2. **Multi-word type matching:** Regex updated to handle `unsigned int`, `const std::string`, etc.
3. **Class body extraction:** Robust brace-matching parser for nested class definitions

---

## Conclusion

The verifier is production-ready for HSE architecture verification. It correctly:
- Detects all valid symbols in the HSE codebase
- Catches all fabricated/missing symbols from inference errors
- Handles multi-word types, nested classes, and Windows file paths
- Produces machine-readable JSON output for automated gatekeeping
