# Phase 1223: Goal Graph Specification

## 1. Goal Entity Schema
`Goal` sits above `MissionState` to represent long-term destinations rather than transient execution passes:
- `goal_id`: Deterministic identifier (`GOAL-HSE-001`, `GOAL-TAILCAT-001`).
- `name`: Human-readable goal name.
- `human_description`: High-level goal destination expressed by human operator.
- `requirements`: List of functional and non-functional requirements.
- `constraints`: Hardware, OS, security, and governance constraints.
- `quality_attributes`: Extensibility, maintainability, reliability, performance.
- `desired_capabilities`: List of required capability IDs.
- `architectural_intent`: Primary architectural pattern selected.
- `status`: `PROPOSED` | `ACCEPTED` | `ANALYZING` | `ACTIVE` | `BLOCKED` | `COMPLETED` | `PAUSED`.

## 2. Goal Switching
Goals coexist in `data/goals/goal_graph.json` and can be switched active/inactive without state loss.
