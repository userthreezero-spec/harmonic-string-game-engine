# HSE-019 — Repository Archaeology

## Summary

The Harmonic String Engine (HSE) is a minimal C++20 rendering engine based on OpenGL 3.3.
It currently supports a basic scene graph with primitives, materials, and cameras.

## Key Subsystems

### 1. Scene & Primitives
- `hse::Scene`: Manages a list of primitives and cameras. Supports transform hierarchy roots.
- `hse::Primitive`: Geometry container. Types: Triangle, Quad, Cube, Sphere (stub).
- `hse::Primitive` supports `m_rotationSpeed` for simple procedural animation.

### 2. Renderer
- `hse::Renderer`: OpenGL 3.3 Core profile.
- Shaders are embedded as strings in `renderer.cpp`.
- Fragment shader supports a simple PBR-lite material structure (albedo, roughness, metallic, texture).
- **CRITICAL DEFICIENCY**: Normals are not generated or passed to the shader.

### 3. Materials & Textures
- `hse::Material`: Stores PBR-ish properties and an optional `hse::Texture`.
- `hse::Texture`: Wraps `stb_image` for loading PNG/JPG.

### 4. Persistence
- `hse::SceneBuilder`: Contains `exportHSC` and `importState`.
- `exportHSC` uses a custom JSON format but currently omits `rotation` and `scale`.
- `.hsc` extension is the intended project format.

### 5. Bridge & Protocol
- `hse::Bridge`: Named pipe server using `HSE_EXTERNAL_PROTOCOL.md`.
- Supports remote scene construction and observation.

## Missing Capabilities (for HSE-019)

1. **Lighting System**: No `Light` class. No shader logic for light-matter interaction.
2. **Normal Mapping/Generation**: Primitives only provide Pos+UV. Normals are required for lighting.
3. **Complete Persistence**: `SceneBuilder` needs to be updated to save/load all transform properties and materials correctly.
4. **Project Creation**: `SceneBuilder::createProject` is a stub.

## Build System
- CMake 3.20+
- Targets: `hse_core` (lib), `hse_main` (exe), various tests.
- Status: Builds cleanly on `harmonic-string-game-engine` branch.

## Conclusion
HSE is ready for scene construction but requires an immediate "Lighting Injection" and "Persistence Fix" to meet the HSE-019 goals.
