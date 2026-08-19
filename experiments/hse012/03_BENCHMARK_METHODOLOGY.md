# HSE-012: Benchmark Methodology

## Purpose

HSE-012 measures whether HSE-011's 64-item knowledge corpus improves architecture prediction quality. The benchmark defines 55 missions representing plausible engine features, scores predictions against the actual HSE ground truth, and runs an ablation across 6 knowledge levels to quantify each layer's contribution.

---

## Mission Categories (55 total)

| Category | Count | IDs | Rationale |
|----------|-------|-----|-----------|
| Rendering | 10 | M01–M10 | Core rendering pipeline features (HSE-011's strongest domain) |
| Input | 5 | M11–M15 | Keyboard/mouse handling |
| Math | 5 | M16–M20 | Math utilities and transforms |
| Scene | 8 | M21–M28 | Scene graph, primitives, cameras |
| Resources | 5 | M29–M33 | Texture/material/resource management |
| Bridge | 4 | M34–M37 | IPC bridge system |
| New Subsystems | 8 | M38–M45 | Features requiring new files/classes |
| Cross-Domain | 5 | M46–M50 | Features spanning 2+ subsystems |
| Novel Architecture | 5 | M51–M55 | Features requiring design reasoning |

---

## Scoring Per Mission

Each mission predicts architectural changes, then scores against ground truth.

### File Prediction (2 pts each)

| Criterion | Points |
|-----------|--------|
| Correct file path in correct directory | 2 |
| File exists but wrong path | 1 |
| File does not exist | 0 |

### Class Prediction (2 pts each)

| Criterion | Points |
|-----------|--------|
| Correct class name AND correct file | 2 |
| Correct class name, wrong file | 1 |
| Wrong or missing class name | 0 |

### Method Prediction (1 pt each)

| Criterion | Points |
|-----------|--------|
| Correct method name + signature match | 1 |
| Correct name, wrong signature | 0.5 |
| Wrong or missing name | 0 |

### Member Prediction (1 pt each)

| Criterion | Points |
|-----------|--------|
| Correct member name + correct type | 1 |
| Correct name, wrong type | 0.5 |
| Wrong or missing name | 0 |

### Dependency Prediction (1 pt each)

| Criterion | Points |
|-----------|--------|
| Correct `#include` path | 1 |
| Wrong include path | 0 |

### Architecture Completeness (0–5 scale)

| Score | Meaning |
|-------|---------|
| 0 | No architectural reasoning |
| 1 | Mentioned relevant subsystem but no design |
| 2 | Partial design, missing key components |
| 3 | Complete subsystem design, minor gaps |
| 4 | Full design with data flow |
| 5 | Production-ready architecture with edge cases |

### Acceptance Criteria Quality (0–3 scale)

| Score | Meaning |
|-------|---------|
| 0 | No testable criteria |
| 1 | Vague criteria ("should work") |
| 2 | Testable but incomplete |
| 3 | Complete, specific, and verifiable |

---

## Aggregate Metrics

### Per-Mission Score

```
Mission Score = File_Pts + Class_Pts + Method_Pts + Member_Pts + Dep_Pts
              + Architecture_Completeness + Acceptance_Criteria_Quality
```

Maximum per mission ≈ 20–25 pts depending on feature complexity.

### Aggregate Metrics

| Metric | Formula |
|--------|---------|
| **Precision** | correctly predicted symbols / all predicted symbols |
| **Recall** | correctly predicted symbols / all ground truth symbols |
| **F1** | 2 × (precision × recall) / (precision + recall) |
| **Dependency Accuracy** | correct includes / all predicted includes |
| **Architecture Completeness** | mean of 0–5 scores across missions |
| **Acceptance Criteria Quality** | mean of 0–3 scores across missions |
| **Unsupported Assumptions** | count of predictions with no ground truth basis |

### Architecture Score (0–10 composite)

Derived from: symbol F1 (40%), dependency accuracy (20%), architecture completeness (20%), acceptance criteria quality (10%), unsupported assumptions penalty (10%).

---

## Knowledge Levels for Ablation

| Level | Label | Knowledge Added |
|-------|-------|-----------------|
| L0 | Repo only | Repository structure analysis (files, classes, patterns visible in code) |
| L1 | + C++ | C++ fundamentals: RAII, smart pointers, templates, STL containers |
| L2 | + Rendering | OpenGL 3.3 core, shaders, buffers, textures, state machine |
| L3 | + Math/Graphics | Vectors, matrices, transforms, camera math, coordinate systems |
| L4 | + Engine Arch | Scene graphs, resource management, ECS patterns, layer systems |
| L5 | + Full corpus | All 15 authoritative sources, 167 concepts, 64 knowledge items |

### Ablation Procedure

For each level L0–L5:
1. Simulate an LLM with only the specified knowledge available
2. Run all 55 missions
3. Score each mission against ground truth
4. Record: architecture score, precision, recall, F1, dependency accuracy, acceptance criteria quality, unsupported assumptions, architecture completeness

### Delta Calculation

```
Δ(L1–L0) = Score(L1) − Score(L0)
Δ(L2–L1) = Score(L2) − Score(L1)
Δ(L3–L2) = Score(L3) − Score(L2)
Δ(L4–L3) = Score(L4) − Score(L3)
Δ(L5–L4) = Score(L5) − Score(L4)
```

Diminishing returns = each successive Δ is smaller than the previous.

---

## Methodology Notes

### Why 55 Missions?

55 missions provides:
- 10 per the strongest domain (rendering) for statistical signal
- Coverage of all 16 HSE-011 domains
- Cross-domain missions that test knowledge integration
- Novel architecture missions that test extrapolation
- Enough granularity to measure per-domain knowledge value

### What Counts as "Correct"

- **Exact match**: Symbol exists in ground truth with that name, type, and location
- **Partial match**: Name matches but type/signature differs (0.5 pts)
- **No match**: Symbol predicted but doesn't exist anywhere in repo
- **New symbol**: Legitimate addition not in ground truth (scored on architectural soundness, not existence)

### Bias Mitigation

- Missions are defined BEFORE scoring (pre-registered)
- Ground truth is from `ground_truth_scan.json` (automated scan, not hand-curated)
- Scoring is mechanical (symbol lookup), not subjective
- Ablation levels are defined independently of expected results
