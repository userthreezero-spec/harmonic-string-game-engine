# HSE-003 KNOWLEDGE GAP MAP

**Date:** 2026-08-17

---

## Knowledge Categories

| Category | Status | Actionable | Notes |
|----------|--------|------------|-------|
| Rendering (OpenGL) | PRESENT | YES | HSE has working OpenGL 3.3 renderer |
| C++ | PRESENT | YES | HSE compiles with GCC 16.1.0 |
| Game-engine architecture | PRESENT | YES | HSE has window/renderer/scene/camera |
| Geometry | PARTIAL | YES | Triangle/Quad/Cube implemented, Sphere stub |
| Linear algebra | PRESENT | YES | Vec3/Mat4 with transforms |
| Spatial reasoning | ABSENT | NO | No bounding boxes, no spatial queries |
| Scene graphs | ABSENT | NO | Flat container only, no hierarchy |
| Materials | ABSENT | NO | Flat color only, no textures/lighting |
| Shaders | PARTIAL | YES | One hardcoded shader, no file loading |
| Lighting | ABSENT | NO | No light sources, no shading |
| Cameras | PARTIAL | YES | Static perspective/ortho, no movement |
| Animation | PARTIAL | YES | Uniform rotation only, no keyframes |
| Physics | ABSENT | NO | No collision, no rigid body |
| Procedural generation | ABSENT | NO | No generation from parameters |
| Roblox knowledge | PRESENT | YES | Assembly graphs, harmonic strings, M762-786 |
| Unreal knowledge | PRESENT | PARTIAL | UObject, reflection, GC (reference only) |
| Open-source engines | PRESENT | PARTIAL | Godot scene tree, resource system |
| Harmonic/String math | PRESENT | YES | Harmonic strings, P-token system, grammar |
| HSE-specific knowledge | PRESENT | YES | Project knowledge model, architecture docs |

## Smallest Missing Knowledge for Next Experiment

**CRITICAL GAP:** HSE has no way to create objects from a structured description. The world contract (OBJ 4) will define a structured format, and the scene API (OBJ 5) will define how to create objects from it.

**Minimum viable knowledge needed:**
1. How to create a Quad (already implemented — reuse)
2. How to position Quads to form walls (translate + rotate — already implemented)
3. How to create a floor (flat Quad at y=0 — trivial)
4. How to create a camera looking at a room (already implemented)
5. How to create openings (doorway/window) — this is the HARD part

**The doorway/window problem:** HSE has no CSG (constructive solid geometry), no boolean operations, no mesh cutting. A "wall with a window hole" requires either:
- Multiple quads arranged around the opening (manual construction)
- Or a new primitive type (e.g., QuadWithHole)

**Decision:** Use manual quad arrangement. This is within HSE's current capabilities.

## Knowledge NOT Needed for Initial Experiment

- Harmonic string parsing (future)
- Assembly graph reconstruction (future)
- Neural network integration (future)
- Physics simulation (not needed for static room)
- Skeletal animation (not needed)
- Audio (not needed)
