# HSE-006 Findings

## What Works

1. **`.hsc` Project Format**: Versioned JSON format with project metadata, scene objects, cameras, clear color, and world revision.

2. **Round-trip Serialization**: `SceneBuilder::importState()` loads .hsc → Scene. `SceneBuilder::exportHSC()` saves Scene → .hsc. Object names, transforms, colors, cameras all persist.

3. **Workspace Launcher**: `hse_workspace_app.exe` takes .hsc path as argument, loads project, starts bridge, runs render loop. Single executable, no manual steps.

4. **Internal Bridge**: Bridge starts automatically with the workspace. No separate launch needed. Protocol unchanged.

5. **Incremental Modification**: Commands modify existing scene objects without restart. Add/remove/move/rotate/scale/color all work through existing bridge protocol.

6. **Persistent State**: Save via `save_project` bridge command writes current state to .hsc. Reopen restores exact state.

7. **Project Isolation**: Each .hsc is self-contained. Opening one does not affect another.

8. **No Rebuild on Reopen**: .hsc is pure data. No compilation or build step needed.

## What Does NOT Work Yet

1. **Auto-save on exit** — Manual save required
2. **Scene hot-reload** — load_project reports success but caller must handle scene swap
3. **Multi-scene projects** — Single scene per .hsc
4. **File association** — .hsc double-click not configured
5. **WebOS connection** — Protocol ready but not exercised
6. **Harmonic concepts** — Placeholder only

## Verification

- All 19 existing tests pass
- 6 executables built successfully
- .hsc parsing produces correct scene state
- exportHSC writes valid .hsc with all object data
- Round-trip: import → export → import produces identical scene
