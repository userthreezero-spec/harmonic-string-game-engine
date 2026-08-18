# HSE-009 End-to-End Demonstration

**Purpose:** Show the complete verification pipeline from scan to gate.

## Pipeline

```
1. Parse Repository    →  Ground Truth Model (27 files, 10 classes, 15 Primitive fields)
2. Load Claims         →  24 claims (14 valid + 10 invalid)
3. Verify Claims       →  14 VERIFIED + 10 CONTRADICTED = 100% accuracy
4. Apply Corrections   →  Fix HSE-008 inference errors
5. Re-verify           →  All claims resolve correctly
6. Gate Decision       →  IS_RTF GRANTED (accuracy >= 90%)
```

## Running the Demo

### Full Pipeline

```bash
# Step 1: Scan repository
python tools/architecture_verifier/verify.py --repo . --scan

# Step 2: Verify HSE-008 claims
python tools/architecture_verifier/verify.py --repo . --hse008

# Step 3: Run accuracy benchmark
python tools/architecture_verifier/verify.py --repo . --benchmark

# Step 4: Verify custom claims
python tools/architecture_verifier/verify.py --repo . --claims experiments/hse009/claims_hse008.json
```

### Expected Output

```
VERIFICATION RESULTS
============================================================
Total:     24
VERIFIED:      14  (58%)
CONTRADICTED:  10  (41%)
UNVERIFIED:    0
INFERRED:      0
============================================================
```

The 58% verified / 41% contradicted split is correct because:
- 14 claims are valid symbols → all VERIFIED
- 10 claims are invalid symbols → all CONTRADICTED
- **Actual accuracy: 100%** (all claims resolve correctly)

## Ground Truth Model

The parser extracts a complete architecture model:

```json
{
  "files": 27,
  "classes": {
    "Primitive": { "fields": 15, "methods": 25 },
    "Renderer": { "fields": 3, "methods": 11 },
    "Scene": { "fields": 5, "methods": 15 },
    "Camera": { "fields": 10, "methods": 16 },
    "Bridge": { "fields": 14, "methods": 21 }
  },
  "structs": 8,
  "enums": 3,
  "functions": 125,
  "includes": 127
}
```

## Claim Types Supported

| Claim Type | Description | Example |
|------------|-------------|---------|
| `file_exists` | File is in repository | `include/scene/primitive.h` |
| `file_not_exists` | File is NOT in repository | `src/renderer/renderer_state.cpp` |
| `class_exists` | Class is defined | `Primitive` |
| `struct_exists` | Struct is defined | `Vec3` |
| `function_exists` | Function/method exists | `uploadGPU` |
| `member_exists` | Field/member exists | `Primitive::m_color` |
| `enum_exists` | Enum is defined | `PrimitiveType` |
| `namespace_exists` | Namespace is used | `hse` |
| `relationship` | Ownership relationship | `Scene owns m_primitives` |

## Accuracy Metrics

| Metric | HSE-008 Set | Benchmark | Combined |
|--------|-------------|-----------|----------|
| Precision | 100% | 100% | 100% |
| Recall | 100% | 100% | 100% |
| F1 Score | 1.00 | 1.00 | 1.00 |
| False Positives | 0 | 0 | 0 |
| False Negatives | 0 | 0 | 0 |

## Use in Implementation Workflow

1. **Before coding:** Run verifier on proposed architecture claims
2. **Gate check:** IS_RTF must be GRANTED before writing implementation code
3. **After coding:** Re-run verifier to confirm new symbols exist
4. **Regression:** Run verifier to catch accidental deletions

This ensures that every implementation step is grounded in verified architecture.
