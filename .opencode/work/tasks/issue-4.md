# Task: issue-4 — [DOCS] Definir nome oficial da linguagem e criar especificação docs/linguagem-simc.md

## Status: READY_TO_COMMIT

## Metadata
- **Type:** docs
- **Scope:** documentation
- **Priority:** high
- **Source:** GitHub Issue #4

## Problem Statement
Definir o nome oficial da linguagem e registrá-lo em `docs/linguagem-simc.md`, contendo lista de tokens, gramática BNF, e exemplos.

## Acceptance Criteria
- [x] Nome oficial da linguagem decidido e registrado em `docs/linguagem-simc.md` e no README
- [x] Tabela 1 de tokens completa (9 palavras reservadas, IDENT, NUMBER_INT/FLOAT, STRING_LITERAL, operadores, pontuação, comentários)
- [x] Gramática núcleo BNF v1.0 documentada
- [x] Mínimo 2 exemplos de programa válido + 1 exemplo com erro documentados
- [x] Convenções da linguagem registradas

## Technical Approach
**Decision:** orchestrator-decided
**Rationale:** Vamos chamar a linguagem de "SimC" (ou manter SimC provisório como oficial se preferido, assumiremos SimC). O doc conterá a especificação Markdown.

## Implementation Plan
### Tasks
- [x] Task 1: Criar `docs/linguagem-simc.md` e escrever Especificação.
- [x] Task 2: Atualizar `README.md` com menção ao nome da linguagem.

### Implementation Order
1. docs/linguagem-simc.md
2. README.md

### Files to Create/Modify
| File | Action | Purpose |
| --- | --- | --- |
| docs/linguagem-simc.md | CREATE | Spec oficial |
| README.md | MODIFY | Referência |

## Evidence
- **Review Verdict:** APPROVED

---
_Created by @orchestrator-nontdd_
