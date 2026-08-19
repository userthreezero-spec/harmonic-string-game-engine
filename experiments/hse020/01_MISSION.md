# HSE-020 — Interactive World Inspection & Manipulation

## Mission Description

Determine whether WebOS can autonomously extend HSE so that a human can open a persistent ".hsc" world, navigate its 3D environment, inspect objects, select objects, manipulate objects, observe the resulting state, and save the modified world without rebuilding the entire engine.

## Core Objectives

1.  **Project Loading**: Open the HSE-019 ".hsc" project.
2.  **Navigation**: Implement manual Camera Orbit, Pan, and Zoom via mouse/keyboard.
3.  **Selection**: Implement object selection (Picking) under the mouse cursor.
4.  **Identity**: Report the selected object's ID and name.
5.  **Visualization**: Visually distinguish the selected object (e.g., tint or wireframe).
6.  **Inspection**: Allow inspection of the selected object's transform.
7.  **Manipulation**: Enable modification of position, rotation, and scale.
8.  **Immediate Feedback**: Reflect changes in the renderer instantly.
9.  **Persistence**: Save the modified world back to ".hsc".
10. **Persistence Verification**: Reload the world and confirm changes are preserved.

## Acceptance Criteria Summary

-   Existing HSE-019 room is rendered correctly.
-   Camera responds to mouse drag (Orbit), middle-click/shift+drag (Pan), and scroll (Zoom).
-   Left-click selects an object.
-   Selected object changes color/brightness.
-   Selected object can be moved/rotated/scaled via keyboard or bridge.
-   Saving project works and re-loading shows moved objects in new locations.
