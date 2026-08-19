# HSE-019 — Autonomous 3D World Construction & Visual Verification

## Mission Description

Create a small furnished 3D room in the Harmonic String Engine that is visually coherent, navigable, persistently saved as an HSE project, and suitable for human inspection.

The room should contain:
- a floor
- four walls
- a ceiling or visually convincing enclosure
- a doorway
- a window
- a table
- at least two chairs
- at least one light source
- distinct materials/colors for major surfaces
- a camera positioned to provide a useful view of the room
- at least one animated element

The completed environment must be saved as a persistent ".hsc" project that can be reopened without rebuilding the entire engine.

After construction, inspect the resulting world, identify deficiencies, correct them, and perform a final verification.

The final result must be made visible to a human observer through the HSE renderer.

## Success Criteria

1. **Autonomous Construction**: The room is built without human-provided coordinates or implementation steps.
2. **Persistence**: The scene is saved to a `.hsc` file and can be reloaded.
3. **Visual Coherence**: The room looks like a room (walls, floor, ceiling, furniture).
4. **Lighting**: At least one light source is active and contributes to the visual result.
5. **Animation**: At least one object is visibly moving/animating.
6. **Human Verification**: A human operator confirms the visual result.
