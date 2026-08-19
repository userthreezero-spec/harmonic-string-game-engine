# HSE-013 — Material Mission Definition

## Objective
Design and implement a dedicated `Material` system in HSE to decouple surface properties from geometry and enable complex rendering permutations.

## Scope

### 1. The `Material` Abstraction
- Create a `hse::Material` class.
- **Properties**:
    - `albedoColor`: RGB tint.
    - `albedoMap`: Optional `std::shared_ptr<Texture>`.
    - `roughness`: Float (0.0 - 1.0).
    - `metallic`: Float (0.0 - 1.0).
- **Behavior**:
    - Materials should be shared assets (many primitives, one material).
    - Materials should manage their own uniform state.

### 2. Architectural Decoupling
- Remove `m_color` and `m_texture` from `hse::Primitive`.
- `Primitive` will now store a `std::shared_ptr<Material> m_material`.
- The `Renderer` must be updated to bind the material before drawing the primitive.

### 3. Rendering Permutations
- Update the shader to use a simple PBR-lite model (or at least acknowledge the parameters).
- **Conditionals**: The shader must handle:
    - Base Color only.
    - Base Color + Texture.
    - Application of Roughness/Metallic as multipliers or factors.

### 4. Serialization
- Update `.hsc` format to support material definitions.
- Primitives should refer to materials by ID/Name.

### 5. Interaction & Observation
- Add bridge commands to modify material properties at runtime.
- Update observation to report material data.

## Justification
This mission is justified by **HSE-011 L4 (Engine Architecture)** and **HSE-012 (Texture Mapping)**. It moves HSE from a "hardcoded renderer" to a "data-driven engine" where surface appearance is a first-class citizen.

## Constraints
- Must use **RAII** for any new GPU resources.
- Must be compatible with **OpenGL 3.3 Core Profile**.
- Must maintain backward compatibility with existing scenes (fallback to default material).
