# HSE-011 AUTHORITATIVE_SOURCE_POLICY.md — Knowledge Quality Classification

**Date**: 2026-08-18
**Phase**: 2 — Source Archaeology
**Status**: COMPLETE

---

## Purpose

Define rules for classifying knowledge quality during HSE-011 ingestion. This policy determines:
- Which sources count toward saturation measurement
- How to handle conflicting information
- How to verify claims against ground truth

---

## Classification System

### A — Primary Specification
**Definition**: Official standard or specification from the governing body.

**Examples**:
- OpenGL 3.3 Core Profile Specification (Khronos Group)
- C++ ISO Standard (ISO/IEC 14882)
- GLSL Language Specification (Khronos Group)

**Rules**:
- Always takes precedence over other sources
- If specification is ambiguous, note the ambiguity
- Must be accessed from official source (khronos.org, iso.org)

### B — Official Documentation
**Definition**: Documentation published by the source owner.

**Examples**:
- GLFW 3.4 Documentation (glfw.org)
- GLM Manual (github.com/g-truc/glm)
- CMake Documentation (cmake.org)
- Microsoft C++ Documentation (learn.microsoft.com)
- Dear ImGui Wiki (github.com/ocornut/imgui)

**Rules**:
- Authoritative for API behavior
- May lag behind specification if specification is newer
- Prefer over third-party tutorials

### C — Official Source Repository
**Definition**: Source code repository maintained by the source owner.

**Examples**:
- GLFW source (github.com/glfw/glfw)
- Godot source (github.com/godotengine/godot)
- bgfx source (github.com/bkaradzic/bgfx)

**Rules**:
- Authoritative for implementation details
- Code comments are authoritative
- Function names and signatures are authoritative
- Implementation algorithms may differ from documentation (document the difference)

### D — Mature Open-Source Implementation
**Definition**: Open-source project with 1000+ stars, 5+ years of active development, used in production.

**Examples**:
- Dear ImGui (50K+ stars, 10+ years)
- Box2D (9K+ stars, 15+ years)
- Flecs (7K+ stars, 5+ years)
- Tracy Profiler (10K+ stars, 5+ years)

**Rules**:
- Patterns are authoritative (how experienced developers solve problems)
- Specific implementations may not be optimal for HSE
- Extract patterns, not code
- Note if implementation differs from documentation

### E — Secondary Reference
**Definition**: Textbooks, academic papers, or reputable reference works.

**Examples**:
- Game Engine Architecture (Jason Gregory)
- Real-Time Rendering (Akenine-Möller et al.)
- OpenGL SuperBible

**Rules**:
- Authoritative for concepts and algorithms
- May be outdated for specific API versions
- Cross-reference with primary sources (A/B) for current implementations

### F — Tutorial/Community Resource
**Definition**: High-quality tutorials from recognized contributors.

**Examples**:
- LearnOpenGL (Joey de Vries, 12K+ stars)
- TheCherno YouTube series / Hazel engine

**Rules**:
- Useful for understanding concepts
- Must be verified against authoritative sources (A-D)
- Do not count toward saturation measurement
- Can be used to identify gaps in knowledge

### G — Generated/Untrusted
**Definition**: AI-generated content, random blog posts, Stack Overflow answers without verification.

**Examples**:
- ChatGPT/Claude explanations
- Random Medium articles
- Unverified Stack Overflow answers

**Rules**:
- NEVER use as knowledge source
- NEVER count toward saturation measurement
- May be used to identify questions to research further
- Always verify against authoritative sources

---

## Conflict Resolution Rules

When sources conflict:

1. **A vs B**: Specification (A) wins
2. **A vs C**: Specification (A) wins, but note implementation may differ
3. **B vs C**: Documentation (B) wins for API behavior, Source (C) wins for implementation details
4. **B vs D**: Official documentation (B) wins
5. **C vs D**: Official source (C) wins
6. **D vs E**: Textbook (E) wins for concepts, Implementation (D) wins for patterns
7. **Any vs G**: Always reject G

**Special case**: If HSE's actual behavior (verified by ground truth scan) conflicts with any source, HSE's actual behavior is authoritative for HSE itself.

---

## Verification Protocol

### Step 1: Source Extraction
- Extract knowledge from authoritative source
- Record: source ID, section, page/line, quote
- Classify the knowledge type (API behavior, pattern, algorithm, concept)

### Step 2: Cross-Reference
- Check against at least one other authoritative source
- If sources agree: knowledge is confirmed
- If sources disagree: apply conflict resolution rules

### Step 3: Ground Truth Check
- For HSE-specific knowledge: verify against ground truth scan (ground_truth.json)
- For external tech behavior: verify against source owner's documentation

### Step 4: Saturation Measurement
- Only count knowledge from classifications A-D toward saturation
- Knowledge from E-F is useful but not counted
- Knowledge from G is never counted

---

## Tracking Requirements

For each knowledge item ingested:

```json
{
  "id": "KNOW-XXX",
  "source_id": "SRC-XXX",
  "source_classification": "A|B|C|D",
  "section": "specific section/page/line",
  "domain": ["rendering", "shaders"],
  "knowledge_type": "api_behavior|pattern|algorithm|concept",
  "content": "extracted knowledge",
  "word_count": 150,
  "verified_by": ["SRC-YYY"],
  "hse_relevant": true,
  "timestamp": "2026-08-18T12:00:00Z"
}
```

---

## Saturation Measurement Rules

### What Counts Toward Saturation
- Knowledge from classifications A-D
- Verified by at least one other authoritative source
- HSE-relevant (applies to HSE's architecture)

### What Does NOT Count
- Knowledge from classifications E-G
- Unverified knowledge (single source, no cross-reference)
- Knowledge about HSE's own code (that's ground truth, not external knowledge)
- Knowledge about tools (CMake, Git) unless directly relevant to engine architecture

### Saturation Threshold
- HSE-010 identified 11 knowledge layers (L0-L10) across 36 domains
- Target: 80%+ coverage across all critical domains
- Critical domains: rendering, math, resource management, scene management, engine architecture
- Non-critical domains: audio, networking, animation (can be lower coverage)

---

## Provenance Requirements

Every ingested source MUST record:
1. **URL/Repository**: Exact location of source
2. **Organization/Author**: Who maintains the source
3. **Source Type**: specification, documentation, source_code, textbook, tutorial
4. **Date Accessed**: When source was accessed for ingestion
5. **Version/Release**: What version was accessed
6. **Domain**: What knowledge domain it covers
7. **Specific Sections Used**: Which parts were actually read
8. **Authority Level**: A-G classification

---

## Enforcement

- All ingestion must follow this policy
- Violations must be documented and corrected
- The policy may be updated as ingestion proceeds
- All policy changes must be committed with clear rationale
