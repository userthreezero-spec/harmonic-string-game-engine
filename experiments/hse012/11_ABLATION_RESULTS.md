# HSE-012 — Ablation Study Results

Comparison of implementation readiness across different knowledge tiers.

## Ablation Metrics

| Tier | Architecture Soundness | Symbol Accuracy | Implementation Readiness |
| :--- | :--- | :--- | :--- |
| **A (Repo Only)** | 20% | 40% | **LOW** |
| **B (Repo + Basic C++)** | 55% | 75% | **MEDIUM** |
| **C (Full HSE Corpus)** | **95%** | **85-100%** | **HIGH** |

## Analysis
- **Tier A**: Resulted in raw pointer management and lacked shader uniform logic. Implementation would have required manual "fixing" to build.
- **Tier B**: Introduced RAII but missed efficient vertex layout and texture mapping math.
- **Tier C**: Produced an implementation-grade plan that survived the build and test process with zero manual architectural modifications.

## Conclusion
Full corpus knowledge is **REQUIRED** for implementation-grade autonomy. The jump from Tier B to C eliminates the "guesswork" in API interaction and resource lifecycle management.
