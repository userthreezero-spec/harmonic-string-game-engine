# HSE-020 — Acceptance Criteria

## Automated Criteria (Machine)

| ID | Criterion | Verification Method |
| :--- | :--- | :--- |
| AC-01 | Build Success | `cmake --build build` returns 0. |
| AC-02 | Regression Tests | `ctest` returns 100% pass. |
| AC-03 | Input API | `hse_test_input` (New) confirms mouse/key state reporting. |
| AC-04 | Picking Math | `hse_test_picking` (New) confirms Ray-Cube intersection accuracy. |
| AC-05 | Persistence | Save-Modify-Reload loop verified via automated project script. |

## Interactive Criteria (Human)

| ID | Category | Expectation |
| :--- | :--- | :--- |
| VC-01 | Viewport | HSE-019 Room loads and renders correctly. |
| VC-02 | Orbit | Left-Drag rotates the view around the room center. |
| VC-03 | Pan | Middle-Drag (or Shift+Drag) moves the view up/down/left/right. |
| VC-04 | Zoom | Scroll wheel moves the view closer or farther away. |
| VC-05 | Selection | Clicking an object (e.g., a chair) selects it. |
| VC-06 | Identity | Console prints the name of the clicked object. |
| VC-07 | Highlight | Selected object turns bright yellow/white. |
| VC-08 | Move | Arrow keys move the selected object on the X/Z plane. |
| VC-09 | Persistence | Pressing 'S' saves the world; reopening shows the object in its new position. |

## Final Gate
Human operator confirms that the room is fully manipulable and changes survive a restart.
