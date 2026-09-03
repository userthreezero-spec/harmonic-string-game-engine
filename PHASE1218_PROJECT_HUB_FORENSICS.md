# PHASE 1218 — Project Hub Forensics

## Executive Summary
Prior to Phase 1218, direct execution of `hse_app.exe` appeared to launch directly into the Two-Story Home runtime without presenting a project selection interface. Forensic analysis revealed that `hse_app.cpp` possessed a console-based project menu with a 1-second timeout that auto-selected the first project (`two_story_home.json`) when executed without immediate interactive stdin, creating the illusion of a hardcoded direct launch.

## Forensic Findings
1. **Entry Point Mechanism**: `hse_app.cpp::main()` instantiated `HSEApp` and called `run()`. `run()` invoked `browseProjects()`, which polled stdin for 1000ms. If no console input was received (as when launching via double-click or non-interactive launcher), `browseProjects()` automatically returned `projects[0]` (`two_story_home.json`).
2. **Missing In-Engine Application Lifecycle**: The engine lacked an explicit state machine representing application phases (`ENGINE_HOME`, `PROJECT_BROWSER`, `PROJECT_SELECTED`, `PROJECT_LOADING`, `PROJECT_OPEN`, `SCENE_LOADING`, `ENGINE_RUNTIME`, `CHECKPOINTING`, `PROJECT_CLOSING`, `RETURNING_TO_HUB`).
3. **Esc Key Behavior**: Pressing `ESC` during runtime previously issued `glfwSetWindowShouldClose`, quitting the entire process rather than returning to a Project Hub.
4. **Thread Termination Bug**: When closing the active project, `Bridge::stop()` called `.join()` on `m_readerThread` while `m_pipe` was blocked in `ConnectNamedPipe`. This caused a deadlock / `0xC000013A` process exit crash. This was resolved by issuing `CancelIoEx` and closing pipe handles prior to joining worker threads.

## Target Hardware Profile Alignment
- **Processor**: Intel Pentium CPU 4425Y @ 1.70GHz (2 Cores, 4 Logical Processors, Ultra-Low-Power).
- **GPU**: Intel UHD Graphics 615 (1GB Shared VRAM, OpenGL 3.3 Core Profile).
- **RAM**: 8.0 GB Physical RAM.
- **Architectural Implications**:
  - The Project Hub UI and rendering loop must maintain ultra-low CPU/GPU overhead (< 50MB RAM footprint).
  - Pipe IPC worker thread must avoid thread contention on 2 physical CPU cores. `CancelIoEx` ensures non-blocking clean shutdown.

## Forensic Remediation
- Implemented explicit 10-stage `ApplicationState` enum and deterministic transition graph in `src/app/hse_app.cpp`.
- Integrated native Project Hub UI into `PROJECT_BROWSER` state inside `hse_app.exe`.
- Made `ESC` in runtime return to `PROJECT_BROWSER` (Project Hub) after saving a checkpoint.
- Fixed named pipe thread shutdown sequence in `PipeServer::stop()` and `Bridge::stop()`.
