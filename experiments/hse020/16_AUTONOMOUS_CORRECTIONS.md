# HSE-020 — Autonomous Corrections

## Correction 1: Missing Math Operators
- **Failure**: `workspace_main.cpp` failed to compile.
- **Evidence**: `error: no match for 'operator-='`.
- **Diagnosis**: `Vec3` lacked `operator-=` and `operator*=`, which were needed for transform manipulation.
- **Correction**: Added these operators to `vec3.h` and `vec3.cpp`.
- **Verification**: Compilation succeeded.

## Correction 2: Orbit Test Regression
- **Failure**: `OrbitTests` failed in `ctest`.
- **Evidence**: `Assertion failed: approx(cam.getPosition().x, 1.0f)`.
- **Diagnosis**: The new unified camera model calculated position from yaw/pitch/radius even when automated orbit was disabled, overwriting manual `setPosition` calls.
- **Correction**: Updated `Camera::updateOrbit` to only recalculate position if `m_orbitEnabled` is true OR if `deltaTime` is 0.0f (explicit request).
- **Verification**: `OrbitTests` passed.

## Correction 3: Matrix Multiplication Order
- **Failure**: Transform-aware picking failed.
- **Evidence**: `hit.hit == true` assertion failed for rotated cube.
- **Diagnosis**: `Primitive::computeWorldMatrix` used `T * R * S` in a Row-Major engine, which applied translation first. This resulted in incorrect world-space rays after inversion.
- **Correction**: Changed the order to `S * R * T` (Scale then Rotate then Translate).
- **Verification**: `PickingTests` passed for all cases.
