# PHASE 1219 — In-Window Graphical Project Hub Implementation

## Core Implementation
- **File**: `src/app/hse_app.cpp`
- **In-Window 3D Menu Scene**:
  - `setupHubScene()`: Builds an interactive 3D Hub Scene (`m_hubScene`) containing floating project card panels (`card_two_story_home`, `card_workbench_validation`, `card_new_project`) and 3D thumbnail preview meshes (`preview_two_story_home`, `preview_workbench`).
  - `renderProjectHubUI(float dt)`: Renders `m_hubScene` using `m_hubCamera` in the desktop window canvas.
  - Animate 3D thumbnail preview meshes subtly (rotation around Y-axis) in the window canvas while in `PROJECT_BROWSER` state.
- **In-Window Mouse Picking & Clicking**:
  - `handleCursor()` & `renderProjectHubUI()`: Continuously updates mouse `(mx, my)` picking ray using `hse::Picker::screenToRay` on `m_hubCamera`. Highlights hovered card panel.
  - `handleMouseButton()`: Triggers `GLFW_MOUSE_BUTTON_LEFT` click handling on `m_hubScene` card primitives, transitioning `PROJECT_BROWSER -> PROJECT_SELECTED -> PROJECT_LOADING -> PROJECT_OPEN -> ENGINE_RUNTIME` directly in the window!
- **Zero Console Dependency**:
  - Terminal console input is 100% bypassed.
  - Headless CI execution (`--headless` or `HSE_HEADLESS=1`) preserves non-interactive auto-selection for automated build runners.
