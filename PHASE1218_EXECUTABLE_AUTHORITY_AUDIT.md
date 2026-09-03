# PHASE 1218 — Executable Authority Audit

## Executable Inventory & Classifications

```
                          ┌───────────────────────────┐
                          │   Harmonic String Game    │
                          │   Engine Repository Root  │
                          └─────────────┬─────────────┘
                                        │
             ┌──────────────────────────┼──────────────────────────┐
             ▼                          ▼                          ▼
     [hse_app.exe]             [hse_workspace_app.exe]    [hse_home_demo.exe]
  Classification:           Classification:            Classification:
  CANONICAL_ENGINE          CANONICAL_PROJECT_         TEST_ONLY
                            HUB_CLIENT
  Status: AUTHORITATIVE     Status: ABSORBED           Status: ACTIVE TEST
```

### Detailed Target Classifications

1. **`hse_app.exe`**: `CANONICAL_ENGINE`
   - **Authority Level**: SOVEREIGN CANONICAL ENGINE.
   - **Responsibilities**: Governs full application lifecycle (`ENGINE_HOME` through `APPLICATION_EXIT`), Project Hub UI, JSON/HSC scene loading, 63-primitive Two-Story Home construction, IPC Named Pipe Bridge (`\\.\pipe\hse_bridge`), checkpointing, project switching, and clean process termination.

2. **`hse_workspace_app.exe`**: `CANONICAL_PROJECT_HUB_CLIENT`
   - **Authority Level**: DEPRECATED / ABSORBED CLIENT.
   - **Responsibilities**: Formerly provided IPC pipe bridge and workspace navigation. All capabilities (IPC pipe, mouse picking, assembly selection, exploded view, state export) have been fully absorbed into `hse_app.exe`.

3. **`hse_home_demo.exe`**: `TEST_ONLY`
   - **Authority Level**: AUTOMATED TEST TARGET.
   - **Responsibilities**: Headless/timed C++ physical regression test target for Two-Story Home geometry and lighting.

4. **`hse_main.exe`**: `LEGACY`
   - **Authority Level**: DEPRECATED LEGACY TARGET.
   - **Responsibilities**: Minimal early material system test binary.

## Authority Governance Rule
No alternate binary may create a shadow scene or bypass `hse_app.exe` as the sole canonical game engine runtime authority.
