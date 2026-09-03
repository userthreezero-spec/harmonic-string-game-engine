# PHASE 1219 — In-Window Graphical Project Hub Design

## Design Directive
The Project Hub menu is rendered **entirely inside the desktop application window canvas** (OpenGL/GLFW window). The user never sees or interacts with a terminal console to select or open projects.

## Graphical In-Window Hub Layout

```
 ┌──────────────────────────────────────────────────────────────────────────────┐
 │                      HARMONIC STRING GAME ENGINE                             │
 │                         GRAPHICAL PROJECT HUB                                │
 └──────────────────────────────────────────────────────────────────────────────┘

 ┌──────────────────────────────────────┐  ┌──────────────────────────────────────┐
 │ [Card 1] Two-Story Home              │  │ [Card 2] Workbench Validation        │
 │  3D Model Preview Panel              │  │  3D Model Preview Panel              │
 │  Category: Residential Architecture  │  │  Category: Engineering Benchmark     │
 │  Primitives: 63 Components           │  │  Primitives: 18 Components           │
 │                                      │  │                                      │
 │  [ Open Project (Click / Enter) ]   │  │  [ Open Project (Click / Key 2) ]   │
 └──────────────────────────────────────┘  └──────────────────────────────────────┘

 ┌──────────────────────────────────────────────────────────────────────────────┐
 │ [Card New]  + Create New Project (Click / Press N)                           │
 └──────────────────────────────────────────────────────────────────────────────┘
```

## Graphical In-Window Mouse & Keyboard Interactions
1. **Mouse Hovering**: Hovering over a 3D project card panel in the window fires an OpenGL picking ray (`Picker::screenToRay`). The card panel visually highlights with an Electric Blue border (`#3B82F6`).
2. **Mouse Clicking**: Left-clicking directly on a card panel in the window immediately opens that project and loads its 3D scene into the window.
3. **Keyboard Shortcuts**: `Enter` / `1` opens Two-Story Home; `2` opens Workbench; `N` / `C` creates a new project; `Q` / `ESC` exits cleanly.
4. **Return to Hub**: Pressing `ESC` inside the 3D runtime scene returns directly to this graphical in-window Project Hub menu!
