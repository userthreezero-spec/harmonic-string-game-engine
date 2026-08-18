# HSE-007 Ingestion Plan — What to Feed WebOS and In What Order

## Layered Knowledge Model

Knowledge is organized in 5 layers, each building on the previous:

```
Layer 5: MISSION KNOWLEDGE (how to build specific features)
  └── "Add texture mapping" → UV coords → shader uniforms → texture loading → GPU pipeline
Layer 4: PATTERN KNOWLEDGE (common solutions to recurring problems)
  └── Entity/Component, Observer, Command, Factory, Resource Handle
Layer 3: DOMAIN KNOWLEDGE (rendering theory, math, engine architecture)
  └── Forward/deferred rendering, quaternions, scene graphs
Layer 2: SYMBOL KNOWLEDGE (what exists in HSE right now)
  └── Class hierarchy, method signatures, data flow, ownership
Layer 1: GROUND TRUTH (raw source code, build system, tests)
  └── Every file, every line, every symbol
```

## Ingestion Strategy

### Round 1: Minimal (Baseline — already done)
- **Input**: Raw source code only
- **What WebOS gets**: Symbol names, class structure, method signatures
- **Expected**: Can answer "what exists" questions, cannot answer "what should we build" questions
- **Baseline score**: 100% symbol grounding (measured in BASELINE.md)

### Round 2: Core Domain Knowledge
- **Input**: Rendering fundamentals + 3D math essentials
- **What WebOS gets**: 
  - What a vertex shader does, what a fragment shader does
  - What model/view/projection matrices mean
  - What VAO/VBO/EBO are
  - What a uniform is
  - What quaternions are and why they beat Euler angles
- **Expected**: Can explain WHY HSE's code does what it does
- **Measurement**: Can HSE-007 explain why `Mat4::perspective` uses `1/tan(fov/2)`?

### Round 3: Engine Architecture Patterns
- **Input**: Game engine architecture patterns + open-source engine references
- **What WebOS gets**:
  - Entity/Component vs inheritance hierarchies
  - Scene graph patterns (flat vs tree)
  - Resource management (handle-based, reference counting)
  - Event systems (observer, message queue)
  - Rendering pipeline patterns (forward, deferred, clustered)
- **Expected**: Can evaluate HSE's architecture against industry patterns
- **Measurement**: Can HSE-007 identify HSE's architectural weaknesses?

### Round 4: Mission-Specific Knowledge
- **Input**: Specific feature implementation guides
- **What WebOS gets**:
  - How to add texture mapping (UV coords → shader → GPU)
  - How to add lighting (normals → light calculation → uniforms)
  - How to add a scene hierarchy (parent/child transforms)
  - How to add animation (keyframes → interpolation → update loop)
- **Expected**: Can produce step-by-step implementation plans with correct file/symbol references
- **Measurement**: Can HSE-007 produce a plan that modifies the correct symbols in the correct files?

### Round 5: Full Synthesis
- **Input**: All of the above + architectural evaluation of HSE
- **What WebOS gets**: Complete understanding of what HSE is, what it should become, and how to get there
- **Expected**: Can identify missing features, prioritize them, and implement them correctly
- **Measurement**: Can HSE-007 produce a 3-month development roadmap with accurate effort estimates?

## Ingestion Method

Each round is delivered as a single context injection before the benchmark task. The injection format:

```
=== KNOWLEDGE INJECTION: ROUND N ===
[Domain knowledge content]
=== END KNOWLEDGE INJECTION ===

Now answer the following about HSE:
[Benchmark question]
```

## Measurement Protocol

### Before Each Round
1. Ask 5 questions that require the knowledge being injected
2. Record answers
3. Inject knowledge
4. Ask same 5 questions again
5. Measure improvement

### Questions (same for all rounds)
1. **Symbol grounding**: "What fields does the Primitive class have?" (14 fields)
2. **Data flow**: "How does a command from the pipe reach the GPU?" (pipe → queue → execute → GL call)
3. **Architecture**: "Why does HSE use a SPSC queue instead of a mutex?" (thread safety + performance)
4. **Design**: "What would you need to add to support textured primitives?" (UV coords, texture loading, shader uniforms)
5. **Planning**: "Write a plan to add a PointLight to the scene." (correct files, correct symbols, correct order)

### Scoring
Each answer scored 0-2:
- 0: Incorrect or hallucinated
- 1: Partially correct (right idea, wrong details)
- 2: Fully correct (right symbols, right files, right reasoning)

Max score per round: 10 points (5 questions × 2 points)

## Expected Results

| Round | Knowledge Added | Expected Score | Cumulative |
|-------|----------------|----------------|------------|
| 1 | Source code only | 6/10 | 6/10 |
| 2 | Rendering + Math | 7.5/10 | 7.5/10 |
| 3 | Engine architecture | 8.5/10 | 8.5/10 |
| 4 | Mission-specific | 9.5/10 | 9.5/10 |
| 5 | Full synthesis | 10/10 | 10/10 |

**Key hypothesis**: Diminishing returns kick in after Round 3. The jump from Round 1→2 is larger than 2→3, which is larger than 3→4.
