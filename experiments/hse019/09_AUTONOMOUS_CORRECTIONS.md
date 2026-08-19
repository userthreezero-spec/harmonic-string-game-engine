# HSE-019 — Autonomous Corrections

The following corrections were made during implementation:

## Correction 1: Missing Header
- **Failure**: `renderer.cpp` failed to compile.
- **Evidence**: Compiler errors related to `Light` type being incomplete or undefined.
- **Diagnosis**: `renderer.cpp` used `scene.getLights()` but did not include `scene/light.h`.
- **Correction**: Added `#include "scene/light.h"` to `renderer.cpp`.
- **Verification**: `renderer.cpp` compiled successfully.

## Correction 2: Regression in Scene Tests
- **Failure**: `SceneTests` failed during `ctest`.
- **Evidence**: `Assertion failed: prim.getVertices().size() == 15`.
- **Diagnosis**: The update to 8 floats per vertex (Pos+UV+Normal) changed the vertex buffer size.
- **Correction**: Updated `tests/test_scene.cpp` to expect 24 floats for a triangle (3 vertices * 8).
- **Verification**: `SceneTests` passed.

## Correction 3: JSON Parser Greedy Matching
- **Failure**: Room project failed to load with correct primitive count.
- **Evidence**: `Primitive count: 20` observed (expected 18).
- **Diagnosis**: `jsonFindArrayObjects` matched `{` objects outside the target array because it didn't respect the closing `]`.
- **Correction**: Updated `jsonFindArrayObjects` to find the array bounds `[...]` and only parse objects within those bounds.
- **Verification**: `hse_test_hsc` passed with 18 primitives.

## Correction 4: CMake Configuration Sync
- **Failure**: New test target not found by `mingw32-make`.
- **Evidence**: `No rule to make target 'hse_test_hsc'`.
- **Diagnosis**: `cmake --build` was run without a preceding re-configuration step after modifying `CMakeLists.txt`.
- **Correction**: Explicitly ran `cmake -S . -B build` followed by build.
- **Verification**: Target was built and executable generated.
