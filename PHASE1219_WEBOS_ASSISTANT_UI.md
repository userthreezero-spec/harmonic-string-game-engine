# PHASE 1219 — In-Window WebOS Assistant Surface Integration

## Architecture
```
  [In-Window Application Canvas] ──► [Press 'A' / 'F1' / Click Assistant]
            │
            ▼
  [WebOS Assistant Surface Overlay]
            │
            ├─► [Immediate Local Scene Commands ("save", "impulse", "shift", "focus", "open")]
            │
            └─► [Sovereign Gateway (HTTP 8725 / PrimeGate Pipeline)]
```

## Integrated Commands & Interactions
Pressing `A` or `F1` at any time (in `PROJECT_BROWSER` or `ENGINE_RUNTIME`) opens the WebOS Assistant surface inside the application window:

1. **Local Scene Intent Execution**:
   - `"save"` / `"checkpoint"`: Saves project state to checkpoint file.
   - `"impulse"`: Spawns dynamic harmonic impulse light at `{0, 2, 0}`.
   - `"shift"` / `"move floor"`: Demonstrates real-time relationship propagation by shifting `floor_1`.
   - `"focus"` / `"reset"`: Resets active camera overview.
   - `"open two_story_home"`: Opens Two-Story Home project directly.

2. **Natural Language WebOS Queries**:
   - User types natural language questions (e.g., *"Why is the second floor oscillating?"*, *"Show structural hierarchy"*).
   - Query is dispatched to Sovereign Gateway (`http://127.0.0.1:8725/request`) via non-blocking background thread.
   - WebOS response is printed to the assistant surface without freezing the 60 FPS render loop.
