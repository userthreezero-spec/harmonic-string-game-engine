# Phase 1224: Timing & Estimation Specification

## Historical Timing Basis
- `MissionTimingEstimator`: Calculates realistic ETAs based on historical execution windows (e.g. `COMPOSITION` ~15–30s, `BUILD` ~20–45s, `VALIDATION` ~10–25s).
- **Honest Uncertainty:** If insufficient historical data exists, `estimated_remaining_seconds` is explicitly set to `null` with basis `"insufficient historical data"`.
