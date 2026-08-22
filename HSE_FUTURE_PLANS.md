# Harmonic String Engine — Future Plans

This document records the future direction established after HSE-023 and the material/physical-state work being developed alongside WebOS.

The goal is not to replace the existing semantic, assembly, primitive, or renderer systems. The goal is to place a deeper physical/harmonic representation underneath them so that rendered geometry is a consequence of world state rather than the source of semantic knowledge.

## 1. Persistent Physical Reality

HSE objects must be persistent physical states, not disposable procedural results.

A persisted state should retain, as applicable:

- deterministic identity
- material identity and knowledge version
- material properties and provenance
- transformation/manufacturing history
- functional requirements and constraints
- harmonic state
- derived physical properties
- derived geometry state
- assembly relationships
- parent/child lineage
- validity state
- provenance/evidence
- branch/restore relationships

The final mesh is not the authoritative identity of the object.

### Required behaviors

- retrieve an already-realized object without unnecessary recomputation
- modify an existing state without destroying the original
- branch from any prior state
- compare branches causally
- restore historical states
- revalidate states when authoritative knowledge changes
- preserve stale historical states without silently treating them as current
- represent `KNOWLEDGE_GAP` honestly

The intended workflow is:

`create → persist → retrieve → manipulate → branch → continue → restore`

A cog created once should be immediately retrievable later rather than requiring WebOS to rediscover how to manufacture it from scratch.

## 2. Knowledge-Versioned Physical Lineage

Physical state must remain connected to the knowledge used to derive it.

A knowledge update must be able to produce a new derivation without destroying the historical result. Old states remain available for inspection and comparison but can become `STALE` when their authoritative inputs are superseded.

This is physical-world version control rather than an ordinary geometry cache.

## 3. Harmonic Field / Continuous Geometry Layer

Develop a renderer-independent realization layer in which a physical/harmonic state can produce a continuous mathematical representation before mesh generation.

Conceptual pipeline:

`physical state → harmonic state → continuous field/curve → geometry sampling → mesh → renderer`

The mesh should be treated as a derived, disposable representation.

The representation should support variable sampling/resolution without changing semantic identity or physical state.

This allows the same object state to be rendered at different levels of geometric detail according to camera distance, hardware capability, importance, or simulation requirements.

## 4. Dynamic Harmonic Reality

Extend HSE from static/equilibrium realization into time-dependent physical behavior.

Initial validation targets:

### Strings

Represent:

- length
- density
- tension
- stiffness
- damping
- boundary conditions
- excitation

and derive harmonic response and spatial displacement.

### Ropes and cables

Represent:

- mass
- length
- stiffness
- damping
- gravity
- tension
- anchors
- external forces

and derive constrained deformation as a continuous curve/field.

### Grass / flexible vegetation

Represent:

- material/structural properties
- anchoring
- mass
- elasticity
- damping
- environmental coupling
- wind/force fields

and derive movement rather than storing only pre-authored animation states.

The same underlying harmonic/state machinery should be reusable across these cases.

## 5. Harmonic Deformation

The parametric/deformable-beam technique investigated during development is useful as an implementation pattern, but must not become the semantic representation of HSE.

The reusable concept is:

`underlying function/field → sample points → construct geometry`

not a Unity-specific mesh implementation and not arbitrary sine-wave parameters exposed as object semantics.

Harmonic functions should represent or approximate physical state and response where justified by the model.

Potential future targets include:

- beams
- strings
- ropes
- cables
- grass blades
- vines
- branches
- reeds
- flexible wires
- hanging chains
- soft mechanical elements
- deformable structural members

## 6. Static and Dynamic Unification

Static manufactured objects and dynamically deforming objects should eventually share the same fundamental model.

Static path:

`material → transformation → functional constraints → equilibrium → harmonic state → geometry`

Dynamic path:

`material → physical state → constraints + external forces → harmonic response over time → geometry(t)`

Both should converge on a common physical/harmonic representation rather than becoming separate procedural engines.

## 7. Material Reality Expansion

Expand authoritative material knowledge so HSE can reason about actual matter rather than appearance alone.

Priority domains include:

- steels and alloys
- aluminum and alloys
- copper and conductors
- polymers
- rubber/elastomers
- glass
- ceramics
- wood
- fuels
- lubricants
- engine oil
- composites
- common manufactured stocks

Engine oil is explicitly required as a material-domain proving case, including composition/base oils, additives, viscosity behavior, lubrication, thermal behavior, oxidation/degradation, material interactions, and manufacturing/refining/blending where authoritative sources support the claims.

Missing properties must produce `KNOWLEDGE_GAP` rather than fabricated values.

## 8. Manufacturing and Transformation Reality

Continue expanding the material → stock → manufacturing → part pathway.

Important transformations include:

- extraction
- refining
- melting
- casting
- cooling
- forging
- rolling
- extrusion
- machining
- cutting
- drilling
- welding
- bending
- forming
- polishing
- heat treatment
- coating
- joining
- fracture
- separation

Transformation history must remain part of physical lineage because two objects made from the same nominal material can have different resulting properties and geometry due to their processing history.

## 9. Engine as First Complex Proof

The engine remains the first major complex real-world proving ground.

The engine must not become a hardcoded asset.

It should emerge from:

`materials → manufactured stock → transformations → parts → functional constraints → mechanical relationships → assemblies → complete engine`

Initial component families can include:

- pistons
- connecting rods
- crankshaft
- cylinder/head structures
- bearings
- seals
- shafts/gears
- lubrication system
- fasteners

The objective is to prove the generalized machinery, not merely create a convincing engine mesh.

## 10. Non-Engine Generalization

The same machinery must prove itself outside mechanical engines.

Candidate validation objects include:

- chair
- tree
- rock
- tool
- cable
- furniture
- vehicle components
- building components

A successful architecture should be able to reuse the same material, transformation, physical-state, harmonic, geometry, and assembly machinery across these domains.

## 11. Visual Validation

Maintain a lightweight visual proving track alongside core HSE development.

A simple semantic workbench/mechanical scene should be used to verify that current HSE outputs can be rendered coherently.

The scene should contain, where supported:

- wooden structure
- metal supports
- a persisted/derived forged-steel cog
- a shaft
- explicit assembly relationships

The visual test is successful only when visible geometry can be traced back through:

`semantic object → material → transformation → functional constraint → physical state → HSE derivation → geometry → assembly → renderer`

A renderer must never become the source of semantic truth.

## 12. Renderer Independence

HSE must remain engine/renderer agnostic.

Existing primitives remain valid as final renderer representations. They are not forbidden; they simply cannot be treated as the semantic definition of an object.

A cylinder may be the rendered consequence of a shaft state. A generated tooth mesh may be the rendered consequence of a cog state. A sampled tube may be the rendered consequence of a rope field.

The semantic/physical representation must survive removal or replacement of the renderer.

## 13. Hardware-Efficient World Projection

Treat modest-hardware performance as an explicit design objective.

The long-term objective is not merely to reproduce an AAA asset pipeline with fewer resources. It is to reduce the amount of explicit information that must be stored and simulated.

Investigate:

- GPU harmonic field evaluation
- adaptive geometry sampling
- hierarchical physical/semantic LOD
- temporal coherence
- instancing/shared physical models
- selective simulation
- spatial streaming of physical state
- on-demand geometry realization
- procedural material realization
- shared environmental fields
- resolution-independent physical representations

The hypothesis to test is:

> A world represented primarily as causal physical state may contain substantially more meaningful variation than an asset-heavy representation of comparable memory and compute budget.

This is a hypothesis to benchmark, not an assumption to declare proven.

## 14. Population-Level Procedural Reality

Explore whether large populations can share a common physical/harmonic model while retaining local state.

Potential examples:

- forests
- grass fields
- gravel
- rocks
- cables/wires
- crowds of manufactured components

The objective is to represent common rules and fields once while storing only meaningful local deviations.

## 15. AAA-Quality Benchmarking

Do not define success solely as matching a specific AAA game's appearance.

Future benchmarks should separately measure:

- visual fidelity
- physical plausibility
- semantic richness
- persistent state size
- geometry generation cost
- simulation cost
- GPU cost
- CPU cost
- memory footprint
- streaming cost
- scene complexity
- hardware tier

The desired outcome is to determine where HSE's representation provides a measurable advantage over conventional asset-heavy approaches.

## 16. Core Architectural Principle

The central HSE rule remains:

> **WebOS does not know an object because it knows its primitives. WebOS knows an object because it knows the material, transformations, physical state, constraints, relationships, and processes that cause that object to exist.**

The renderer is the final projection of that knowledge.

The persistent physical state is the reusable asset.

The harmonic representation is the mathematical bridge between physical reality and geometry.

The resulting architecture should generalize from individual manufactured parts to complete worlds.
