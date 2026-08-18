# WebOS Integration

## Overview

Harmonic String Engine can request cognitive engineering assistance from WebOS. This document describes the integration boundary.

## Architecture

```
HarmonicStringEngine
        │
        │ engineering request
        ▼
WebOS Cognitive Interface (webos/integration/)
        │
        ├── knowledge retrieval
        ├── repository analysis
        ├── architectural reasoning
        ├── planning
        ├── authoring
        ├── validation
        └── provenance
        │
        ▼
HarmonicStringEngine
```

## Key Principle

**HSE is the independent project. WebOS is an external cognitive service.**

- HSE does not depend on WebOS for compilation or runtime
- HSE can request engineering assistance from WebOS
- WebOS does not own or control HSE
- All WebOS modifications are recorded with provenance

## Integration Files

| File | Purpose |
|------|---------|
| `webos/integration/REQUEST_TEMPLATE.md` | Template for engineering requests |
| `webos/integration/PROVENANCE_LOG.json` | Record of all WebOS modifications |
| `webos/integration/KNOWLEDGE_MODEL.json` | Structured project knowledge |

## How to Request WebOS Assistance

1. Open `webos/integration/REQUEST_TEMPLATE.md`
2. Fill in the engineering objective
3. WebOS will analyze the request, inspect the repository, and produce a plan
4. WebOS will execute approved changes with provenance
5. All changes are committed to HSE's own Git repository

## Boundaries

- WebOS does NOT commit to WebOS repository
- WebOS does NOT modify WebOS itself
- WebOS does NOT make HSE a WebOS subsystem
- HSE remains independently buildable and runnable
