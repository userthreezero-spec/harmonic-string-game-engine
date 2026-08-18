# HSE-012 — Knowledge Contribution Trace

This document traces architectural decisions to the accumulated authoritative knowledge from HSE-011.

## Decision Traceability

| Architectural Decision | Contributor | Domain | Knowledge Item |
| :--- | :--- | :--- | :--- |
| `class Texture` RAII wrapper | C++ Core Guidelines | C++ / Architecture | R.1 (Source #69) |
| Interleaved Vertex Layout | docs.gl | GPU / Rendering | Attribute Layout (Source #3) |
| `std::shared_ptr` for Assets | HSE-011 Synthesis | C++ / Memory | Synthesis (L1 + L2) |
| `uHasTexture` uniform check | LearnOpenGL | Rendering / GPU | Fragment Shaders (Source #15) |
| `stbi_load` for PNG/JPG | Engineering Best Practice | Systems / IO | STB Library Integration |
| VBO Stride (5 floats) | OpenGL Specification | GPU / Rendering | Buffer Data (Source #2) |

## Analysis
Authoritative knowledge (Tier A/B) directly informed the structure of the `Texture` class and the `Renderer` shader update. Without this knowledge, the implementation might have relied on raw pointers or non-standard vertex layouts, leading to memory leaks or rendering artifacts.
