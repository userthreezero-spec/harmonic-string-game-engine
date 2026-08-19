# HSE-014 Knowledge Grounding

## Transform Hierarchy Design Decisions

| Decision | Source | Authority | Concept | Why HSE |
|----------|--------|-----------|---------|---------|
| Parent as weak_ptr, children as vector<shared_ptr> | Godot Node tree pattern | D: Mature impl | Prevents cycles, allows parent deletion without child dangling | Standard scene graph ownership |
| World matrix = parent.worldMatrix * localMatrix | OpenGL/Game engine convention | A: GL spec, math | Standard TRS composition | Required for correct rendering |
| Reparent operation: detach from old parent, attach to new | Godot Node::add_child pattern | D: Mature impl | Standard scene graph mutation | Bridge needs dynamic reparenting |
| Root primitives: worldMatrix = localMatrix | Standard game engine | A: Math | Identity parent multiplication | Simplest correct behavior |
| Weak_ptr for parent reference | C++ Core Guidelines, EnTT patterns | B: Official docs | Prevents reference cycles, allows parent deletion | Correct ownership model |
| Children vector on Primitive (not separate node) | Hazel/EnTT single-entity pattern | D: Mature impl | Keeps API simple, one class owns hierarchy | HSE is small, don't over-engineer |
| Scene manages roots + traversal | Godot SceneTree, raylib scenegraph | D: Mature impl | Scene is the natural owner of traversal | Scene already owns primitives |
| Renderer traverses hierarchy bottom-up for world matrices | Standard game engine render loop | A: GL spec | Children must have parent matrices computed first | Correct rendering order |
| .hsc serializes parent name (not ID) | Name-based consistency | N/A: Design choice | IDs change on reload, names are stable | Matches existing .hsc convention |
