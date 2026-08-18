# HSE-009: Automated Ground-Truth Architecture Verification

**Status:** COMPLETE
**Accuracy:** 100% (15/15 benchmark, 24/24 HSE-008 regression)

## Overview

HSE-009 builds an automated verifier that checks architectural claims against actual HSE source code. It catches inference errors (fabricated symbols, wrong file paths, missing members) before they become implementation bugs.

## Quick Start

```bash
# Verify HSE-008 claims
python tools/architecture_verifier/verify.py --repo . --hse008

# Run accuracy benchmark
python tools/architecture_verifier/verify.py --repo . --benchmark

# Scan repository (produce ground truth)
python tools/architecture_verifier/verify.py --repo . --scan
```

## Files

| File | Description |
|------|-------------|
| `tools/architecture_verifier/verify.py` | Verifier tool (v2) |
| `ground_truth.json` | Repository scan output |
| `experiments/hse009/verification_results.json` | HSE-008 verification results |
| `experiments/hse009/claims_hse008.json` | HSE-008 claim set (24 claims) |
| `experiments/hse009/claims_benchmark.json` | Benchmark claim set (15 claims) |
| `experiments/hse009/accuracy_report.md` | Full accuracy analysis |
| `experiments/hse009/implementation_readiness_gate.json` | IS_RTF gate decision |
| `experiments/hse009/correction_loop_demo.md` | Error correction walkthrough |
| `experiments/hse009/end_to_end_demo.md` | Full pipeline demonstration |
| `experiments/hse009/MISSION_REPORT.md` | Mission summary |

## Claim Types

- `file_exists` / `file_not_exists`
- `class_exists` / `struct_exists`
- `function_exists` / `member_exists`
- `enum_exists` / `namespace_exists`
- `relationship` (ownership)

## Accuracy

| Metric | Value |
|--------|-------|
| Precision | 100% |
| Recall | 100% |
| F1 Score | 1.00 |
| False Positives | 0 |
| False Negatives | 0 |
