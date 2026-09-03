# HSE-023R2 — Test Evidence

## 1. Interaction Integration Test (`hse_test_interaction.exe`)
Verified the mathematical correctness of the projection pipeline.

```
Testing picking at center (500, 500)...
  PASS: Center hit confirmed.
Testing picking offset (600, 500)...
  PASS: Offset hit confirmed.
Testing picking miss (900, 500)...
  PASS: Miss confirmed.
ALL INTERACTION TESTS PASSED!
```

## 2. Recursive Assembly Test (`test_assembly.exe`)
Verified propagation of explosion factors through nested hierarchies.

```
Displaced Child World X: 4
PASS: recursive explosion
All assembly tests passed!
```

## 3. Full Regression Suite (`ctest`)
```
      Start  1: MathTests ........................   Passed
      Start  2: SceneTests .......................   Passed
      Start  3: MaterialTests ....................   Passed
      Start  4: HierarchyTests ...................   Passed
      Start  5: OrbitTests .......................   Passed
      Start  6: PersistenceTests .................   Passed
      Start  7: InputTests .......................   Passed
      Start  8: PickingTests .....................   Passed
      Start  9: CollisionTests ...................   Passed
      Start 10: AssemblyTests ....................   Passed
      Start 11: AssemblyPersistenceTests .........   Passed
      Start 12: InteractionTests .................   Passed
      Start 13: GoalTests ........................   Passed
      Start 14: MultiConstraintTests .............   Passed

100% tests passed out of 14
```
