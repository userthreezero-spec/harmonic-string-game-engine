# HSE-010 ARCHITECTURE CHANGE MATRIX

## Purpose

Track whether proposed architecture changes across knowledge layers. This is the primary saturation signal: if architecture stops changing, knowledge may be sufficient.

---

## Matrix

For each knowledge layer addition, record whether the proposed architecture for each mission changes.

### Layer B: +C++ Engineering

| Mission | Boundary Changed? | Symbol Changed? | Dependency Changed? | Strategy Changed? | Notes |
|---------|-------------------|-----------------|---------------------|-------------------|-------|
| A1: Pyramid | | | | | |
| A2: Color | | | | | |
| A3: GET_PRIMITIVE_COUNT | | | | | |
| A4: Reset camera | | | | | |
| A5: Get primitive name | | | | | |
| B1: Texture mapping | | | | | |
| B2: Point light | | | | | |
| B3: Scene hierarchy | | | | | |
| B4: GPU RAII | | | | | |
| B5: Scene serialization v2 | | | | | |
| C1: Material system | | | | | |
| C2: Render batching | | | | | |
| C3: Async asset loading | | | | | |
| C4: Render pass abstraction | | | | | |
| C5: Undo/redo | | | | | |
| D1: Instanced + frustum | | | | | |
| D2: Shader hot-reload | | | | | |
| D3: Scene streaming | | | | | |
| D4: GPU profiling | | | | | |
| D5: Editor viewport | | | | | |
| E1: GPU resource lifetime | | | | | |
| E2: Texture atlas | | | | | |
| E3: Render commands | | | | | |
| E4: Material instances | | | | | |
| E5: Multi-threaded update | | | | | |

### Layer C: +Software Architecture

| Mission | Boundary Changed? | Symbol Changed? | Dependency Changed? | Strategy Changed? | Notes |
|---------|-------------------|-----------------|---------------------|-------------------|-------|
| (same 25 missions) | | | | | |

### Layer D: +C++ Implementation Engineering

| Mission | Boundary Changed? | Symbol Changed? | Dependency Changed? | Strategy Changed? | Notes |
|---------|-------------------|-----------------|---------------------|-------------------|-------|
| (same 25 missions) | | | | | |

### Layer E: +3D Mathematics

| Mission | Boundary Changed? | Symbol Changed? | Dependency Changed? | Strategy Changed? | Notes |
|---------|-------------------|-----------------|---------------------|-------------------|-------|
| (same 25 missions) | | | | | |

### Layer F: +Rendering Engineering

| Mission | Boundary Changed? | Symbol Changed? | Dependency Changed? | Strategy Changed? | Notes |
|---------|-------------------|-----------------|---------------------|-------------------|-------|
| (same 25 missions) | | | | | |

### Layer G: +Game Engine Architecture

| Mission | Boundary Changed? | Symbol Changed? | Dependency Changed? | Strategy Changed? | Notes |
|---------|-------------------|-----------------|---------------------|-------------------|-------|
| (same 25 missions) | | | | | |

### Layer H: +Tooling and Editor Architecture

| Mission | Boundary Changed? | Symbol Changed? | Dependency Changed? | Strategy Changed? | Notes |
|---------|-------------------|-----------------|---------------------|-------------------|-------|
| (same 25 missions) | | | | | |

### Layer I: +Existing Engine Architecture Studies

| Mission | Boundary Changed? | Symbol Changed? | Dependency Changed? | Strategy Changed? | Notes |
|---------|-------------------|-----------------|---------------------|-------------------|-------|
| (same 25 missions) | | | | | |

### Layer J: +Production Software Construction

| Mission | Boundary Changed? | Symbol Changed? | Dependency Changed? | Strategy Changed? | Notes |
|---------|-------------------|-----------------|---------------------|-------------------|-------|
| (same 25 missions) | | | | | |

### Layer K: +Architecture-to-Implementation Practice

| Mission | Boundary Changed? | Symbol Changed? | Dependency Changed? | Strategy Changed? | Notes |
|---------|-------------------|-----------------|---------------------|-------------------|-------|
| (same 25 missions) | | | | | |

---

## Summary: Architecture Stability Over Layers

| Layer | % Missions Changed | % Boundary Changes | % Symbol Changes | % Dependency Changes | % Strategy Changes |
|-------|-------------------|-------------------|-----------------|---------------------|-------------------|
| B: +C++ | | | | | |
| C: +Arch | | | | | |
| D: +Impl | | | | | |
| E: +Math | | | | | |
| F: +Render | | | | | |
| G: +Engine | | | | | |
| H: +Editor | | | | | |
| I: +Engines | | | | | |
| J: +Prod | | | | | |
| K: +Practice | | | | | |

**Saturation signal**: When % Missions Changed drops below 10% and stays there for 2+ consecutive layers, saturation is approaching.

---

## Analysis

### Architecture Changes That Matter

An architecture change "matters" if it:
1. Changes a subsystem boundary (what belongs to what)
2. Changes a symbol (class, method, field)
3. Changes a dependency (what includes what)
4. Changes a strategy (forward vs deferred rendering, ECS vs inheritance)

### Architecture Changes That Don't Matter

An architecture change "doesn't matter" if it:
1. Only changes variable names
2. Only changes code style
3. Only changes comments
4. Only adds redundancy

### Key Question

> Does knowledge beyond L5 (rendering engineering) materially change architectural decisions?

If YES → false saturation (HSE-007 stopped too early)
If NO → true saturation (HSE-007 was correct)
