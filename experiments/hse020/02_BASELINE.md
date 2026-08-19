# HSE-020 — Baseline Assessment

## Post-HSE-019 State

| Capability | Status | Notes |
| :--- | :--- | :--- |
| Render Loop | PROVEN | OpenGL 3.3, Windowing (GLFW), Renderer class. |
| Scene Graph | PROVEN | Flat list of primitives, Transform hierarchy (Parent-Child). |
| Primitives | PROVEN | Triangle, Quad, Cube (with Normals, UVs). |
| Materials | PROVEN | Albedo, Roughness, Metallic. |
| Lighting | PROVEN | Single Point Light, Blinn-Phong shaders. |
| Persistence | PROVEN | `.hsc` (JSON) format with Pos, Rot, Scale, Materials, Lights. |
| Camera | PROVEN | Perspective, Automated Orbit. |
| Bridge | PROVEN | Command/Observation protocol over Named Pipes. |
| Tests | PROVEN | Math, Scene, Material, Hierarchy, Orbit, Persistence tests. |

## Deficiencies (Starting HSE-020)

1.  **Manual Navigation**: Camera only supports automated orbit; no user input handling for Orbit/Pan/Zoom.
2.  **Input Handling**: `Window` class only handles `pollEvents`; mouse coordinates and clicks are not exposed.
3.  **Picking**: No mechanism to determine which object is under a pixel.
4.  **Selection State**: No concept of a "Selected Object" in the Scene or Workspace.
5.  **Visual Highlight**: Shaders only draw objects with their standard materials.
6.  **Interactive Editor**: `hse_workspace_app` is a passive viewer/bridge-server.

## Architecture Metrics (HSE-019 Baseline)

-   Architecture Score: 94
-   Symbol Accuracy: 100%
-   Complexity: 38/50
-   Tests: 6 passing (HSE-019 set)
