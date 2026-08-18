# HSE-006 Handoff

## Status: STATE B (Project/workspace works, WebOS not connected)

## How to Use

### Create a Project
```bash
# The workspace launcher creates a default project if none specified
.\build\hse_workspace_app.exe
```

### Open a Project
```bash
.\build\hse_workspace_app.exe projects\DemoRoom\DemoRoom.hsc
```

### Or use the launcher
```bash
.\launch_workspace.bat projects\DemoRoom\DemoRoom.hsc
```

### Modify via Bridge
```powershell
# Connect and modify
.\phase006_probe\demo.ps1
```

### Save State
The `save_project` bridge command writes current state to .hsc:
```json
{"cmd":"save_project", "seq":1, "path":"path/to/project.hsc"}
```

## Files

| File | Purpose |
|------|---------|
| `src/workspace_main.cpp` | Workspace launcher entry point |
| `launch_workspace.bat` | Desktop launcher |
| `HSE_EXTERNAL_PROTOCOL.md` | Bridge protocol (unchanged) |
| `experiments/hse006/PROJECT_FORMAT.md` | .hsc format specification |
| `phase006_probe/demo.ps1` | Demo controller |

## For Future WebOS Integration

1. WebOS opens .hsc project file
2. HSE loads scene, starts bridge
3. WebOS sends commands via bridge protocol
4. WebOS observes scene state
5. WebOS saves via `save_project` command
6. HSE persists state for next session

## What Remains

- [ ] Auto-save on exit
- [ ] Scene hot-reload (load_project scene swap)
- [ ] Multi-scene projects
- [ ] .hsc file association (double-click to open)
- [ ] WebOS actual connection test
