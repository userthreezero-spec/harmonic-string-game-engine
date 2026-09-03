# Phase 1227: Execution Stagnation Watchdog Specification

## 1. Stagnation Detection Logic
In `sovereign_operations.py` `_process_queue`:
```python
last_up = mission.get("last_update", mission.get("queued_at", time.time()))
if mission.get("status") in ["RUNNING", "STARTING"] and (time.time() - last_up) > 300:
    logger.warning(f"Execution stagnation detected for mission {mid}: no activity for >300s")
    mission["status"] = "STALLED"
    mission["last_update"] = time.time()
    mission["results"] = {"status": "STALLED", "reason": "Execution stagnation detected: zero worker activity for >300s", "provenance": "UCS_PRIME_P1227"}
    self.registry.record_mission(mission)
    if mid in self.running_missions:
        del self.running_missions[mid]
    continue
```

## 2. Telemetry & Progress Exposure
HTTP port 8725 endpoints exposed:
- `GET /goals/tree` — Visual Goal Tree
- `GET /telemetry/{id}` — Mission execution event stream & timing
- `GET /progress/{state}` — Human-readable progress steps
