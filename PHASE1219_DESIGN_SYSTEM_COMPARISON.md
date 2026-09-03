# PHASE 1219 — Design System Comparison Matrix

| Design System | Primary Strength | Key Pattern Extracted | Application to HSE Project Hub |
|---------------|------------------|----------------------|--------------------------------|
| **Apple HIG** | Visual clarity, feedback | Progressive disclosure & clean grouping | Hide raw system state behind clear human labels. |
| **Microsoft Fluent 2** | Desktop density, command bars | Structured cards, keyboard accessibility | Information-dense card layout for engineering projects. |
| **Material Design 3** | Component hierarchy & state | Elevation, focus & active states | Clear button hierarchy (`Open`, `New Project`, `Assistant`). |
| **IBM Carbon** | Data presentation | Grid alignment & component tokens | Consistent spacing, border framing, and structured tables. |
| **shadcn/ui / Radix** | Keyboard focus & accessibility | Accessible dialogs & command surface | Full keyboard navigation (`Enter`, `1-N`, `N`, `A`, `Q`). |
| **Unity Hub / Unreal** | Project discovery & templates | Project cards with primitive count & path | Project Hub layout showcasing recent projects & templates. |

## HSE Design Language Determination
WebOS synthesized these patterns into a unified **Harmonic String Engine Design Language**:
- Dark-mode technical theme (Background `#14171F`, Surface `#1E222D`, Accent `#3B82F6`, Text `#F3F4F6`).
- Information-dense human-centered project cards.
- Natural language WebOS Assistant surface (`Press A` or voice/text input via Sovereign Gateway).
