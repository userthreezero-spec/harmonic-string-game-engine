# HSE-021 — Goal-Directed Semantic World Transformation

## Mission Description
Implement a bounded experiment to test whether WebOS can reason about the semantic purpose and spatial relationships of objects in an existing world, formulate a transformation that satisfies a high-level objective, and execute it using existing engine capabilities.

## High-Level Goal
"Arrange this room so that two people can comfortably sit at the table and have a conversation."

## Core Objectives
1. **Repository Archaeology**: Establish the technical baseline.
2. **World State Archaeology**: Inspect HSE-019 `room.hsc` to identify objects and relationships.
3. **Semantic Interpretation**: Derive spatial constraints from the human goal.
4. **Transformation Plan**: Formulate a set of moves (translation, rotation, state changes).
5. **Execution**: Perform the transformation using the HSE-020 Bridge/Manipulation layer.
6. **Validation**: Mathematically and visually verify the goal is met.
7. **Persistence**: Save the modified world and ensure it survives a reload.
