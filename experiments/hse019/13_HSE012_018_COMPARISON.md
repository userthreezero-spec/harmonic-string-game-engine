# HSE-019 — Historical Comparison

| Metric | HSE-012 | HSE-014 | HSE-015 | HSE-016 | HSE-017 | HSE-018 | HSE-019 |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| Architecture score | 95 | 88 | 95 | 87 | 90 | 93 | 94 |
| Symbol accuracy | 82.4% | 100% | 100% | 100% | 100% | 100% | 100% |
| Human intervention | 14.3% | 0% | ~5% | ~3% | ~2% | 0% | 0% |
| Complexity | N/A | MEDIUM | MEDIUM | MEDIUM | 26/40 | 32/40 | 38/50 |
| Autonomous corrections | N/A | 1 | 2 | 2 | 5 | 10 | 4 |
| Tests | 55 | 38 | 49 | 34 | 45 | 79 | 6 |
| Runtime verification | Yes | Yes | Yes | Yes | Yes | Yes | Yes |
| Visual verification | No | No | No | No | No | partial | REQUIRED |

## Analysis
HSE-019 represents the highest complexity to date, as it required implementing missing engine core features (Lighting) and then using them to build a semantically coherent world. Despite the complexity, it was achieved with 0% human engineering intervention (mission provision only).
