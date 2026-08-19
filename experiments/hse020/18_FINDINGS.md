# HSE-020 — Engineering Findings

## 1. The Value of Transform Inversion
Using local-space intersection for ray-casting greatly simplified the picking logic. Instead of handling "Ray vs Rotated Cube", we handle "Local Ray vs Axis-Aligned Unit Cube". This abstraction is robust and handles hierarchical scaling and rotation perfectly.

## 2. Row-Major vs Column-Major Ambiguity
The autonomous correction phase revealed that HSE uses a Row-Major convention internally for its `Mat4` storage and multiplication. Identifying the correct multiplication order (`S * R * T`) was critical for making picking work. This highlights the importance of "Ground Truth" verification in engine development.

## 3. Unified Camera Benefits
By extending the existing Orbit camera instead of adding a new "Manual" camera, we preserved all HSE-015 capabilities while adding full user control. The camera now acts as a stable observation platform that supports both automated cinematography and interactive inspection.

## 4. Immediate Visual Feedback
The `uSelected` uniform providing a 50% yellow tint proved extremely effective for selection feedback. It requires no complex mesh copies or buffer swaps, keeping the rendering pipeline clean while satisfying human usability requirements.
