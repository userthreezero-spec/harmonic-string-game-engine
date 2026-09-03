# Phase 1224: Execution Telemetry HTTP API Specification

## Endpoints Implemented on HTTP Port 8725 (`machine_interface.py`):
1. `GET /goals` — Returns list of all persistent goals.
2. `GET /goals/{goal_id}` — Returns goal details and handoff plan.
3. `GET /telemetry/active` — Returns active telemetry snapshots for all missions.
4. `GET /telemetry/{mission_id}` — Returns full event history and timing for a mission.
5. `GET /progress/{state}` — Returns step-by-step human progress list (`DONE`, `ACTIVE`, `PENDING`).
