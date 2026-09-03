# PHASE 1218 — Runtime Entry Audit

## Executable Entry Point Analysis

| Target Executable | Classification | Role & Purpose | Lifecycle Compliance |
|-------------------|----------------|----------------|----------------------|
| `hse_app.exe` | **CANONICAL_ENGINE** | Single authoritative engine executable containing Project Hub, State Machine, Scene Loaders, Interactive 3D Runtime, Checkpoint/Save, and IPC Bridge. | **FULL COMPLIANCE** (10 Application States) |
| `hse_workspace_app.exe` | **CANONICAL_PROJECT_HUB_CLIENT** | Secondary developer/test workspace client. Absorbed into `hse_app.exe`. | **SUPPORTED DEV CLIENT** |
| `hse_home_demo.exe` | **TEST_ONLY** | Dedicated automated test executable for Two-Story Home physical regression testing. | **TEST BENCH ONLY** |
| `hse_main.exe` | **LEGACY_TEST** | Minimal legacy material system test executable. | **DEPRECATED** |

## Startup Routing Behavior
When `hse_app.exe` starts:
1. `main(argc, argv)` instantiates `HSEApp`.
2. `parseArgs()` checks for `--project <path>`, `--headless`, or `--frames <N>`.
3. If `--project <path>` is supplied, routing bypasses Project Hub interactive menu and enters `PROJECT_SELECTED` directly. Pressing `ESC` during runtime returns to `PROJECT_BROWSER` (Project Hub).
4. If no arguments are supplied, engine enters `PROJECT_BROWSER` and renders the interactive Project Hub UI.

## Verification
Tested via `hse_test_phase1218.exe` and `hse_app.exe --headless --frames 5`. Both verified clean startup, project loading, runtime execution, and clean process exit 0.
