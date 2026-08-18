# HSE-008 HANDOFF

## For Implementation Phase (HSE-009 or later)

### What Is Ready
- Complete implementation specification (7 specs)
- Exact files, symbols, line numbers
- Acceptance criteria per spec
- Test strategy per spec
- Build system changes identified
- External dependency identified (stb_image)

### What Is NOT Ready
- No C++ code written (by design)
- No tests written (by design)
- No visual verification (requires human)
- No performance testing (not in scope)

### Implementation Order
1. SPEC 1: Vec2 math type (new files)
2. SPEC 2: Primitive UV extension (modify existing)
3. SPEC 3: TextureManager (new files)
4. SPEC 4: Shader extension (modify existing)
5. SPEC 5: Bridge command (modify existing)
6. SPEC 6: Serialization (modify existing)
7. SPEC 7: CMake update (modify existing)

### Key Risks
1. UV VBO not tracked in Primitive (resource leak on re-upload) — acceptable for v1
2. stb_image must be downloaded manually (not in CMake)
3. Shader recompilation needed for all primitives (minor perf cost)
4. No mipmap generation (texture quality at distance)

### Verification Checklist
- [ ] All 7 specs implemented
- [ ] All acceptance criteria met
- [ ] All tests pass
- [ ] Visual verification (textured quad)
- [ ] External controller can set texture via bridge
- [ ] Save/load round-trip preserves textures
- [ ] No GPU memory leaks (manual check)
