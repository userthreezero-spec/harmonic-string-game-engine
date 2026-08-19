# HSE-019 — Engineering Findings

## 1. Subsystem Interdependency
The transition from position-only vertices to position+uv+normal vertices required touching `Primitive`, `Renderer`, `SceneBuilder`, and `Tests`. This cross-cutting change is a high-risk operation that WebOS handled by identifying all affected symbols during the archaeology phase.

## 2. JSON Parser Fragility
The minimal JSON parser in `SceneBuilder` proved to be a bottleneck. Its greedy nature led to incorrect scene counts. Implementing a more robust depth-aware matching strategy was necessary for complex projects like the HSE-019 Room.

## 3. Lighting Model
A basic Blinn-Phong model was sufficient for visual coherence. However, without shadow mapping, the interior of the room may look "flat" if the light source is too ambient. The current implementation uses a 0.1 ambient factor to ensure some detail in unlit areas.

## 4. Animation Mechanism
Using `rotationSpeed` in the base `Scene::update` loop is an effective way to fulfill "at least one animated element" without requiring a full keyframe system (which remains in the sandbox but not in the engine).
