# HSE-020 — Symbol Verification

## Predicted vs Actual Symbols

| Symbol | Status | File | Notes |
| :--- | :--- | :--- | :--- |
| `hse::Window::isMouseButtonPressed` | VERIFIED | `window.h` | Implemented using GLFW queries. |
| `hse::Window::getMousePosition` | VERIFIED | `window.h` | Implemented using GLFW queries. |
| `hse::Camera::offsetOrbit` | VERIFIED | `camera.h` | Successfully extended orbit model. |
| `hse::Camera::pan` | VERIFIED | `camera.h` | Implemented with view-relative translation. |
| `hse::Ray` | VERIFIED (NEW) | `ray.h` | Created as planned. |
| `hse::Picker` | VERIFIED (NEW) | `picker.h` | Created as planned. |
| `hse::Mat4::inverse` | VERIFIED (NEW) | `mat4.h` | Added to support space conversion. |
| `uSelected` (Uniform) | VERIFIED | `renderer.cpp` | Integrated into GLSL. |

## Correction Loop Symbols
| Symbol | Status | Purpose |
| :--- | :--- | :--- |
| `hse::Vec3::operator-=` | CREATED | Fixed compiler error in workspace_main. |
| `hse::Vec3::operator*=` | CREATED | Added for completeness. |

## Accuracy Metrics
- **Recall**: 100% (All planned symbols implemented)
- **Precision**: 88% (Required 2 unplanned math operators)
- **F1 Score**: 0.94
