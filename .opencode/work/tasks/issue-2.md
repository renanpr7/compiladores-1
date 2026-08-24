# Task: issue-2 — [CHORE] Setup do ambiente canônico (WSL) e acesso do professor ao repositório

## Status: COMPLETED

## Metadata
- **Type:** chore
- **Scope:** infrastructure
- **Priority:** high
- **Source:** GitHub Issue #2

## Problem Statement
O projeto será desenvolvido majoritariamente em Windows, mas o ambiente canônico é o WSL Ubuntu. Precisamos registrar os comandos de setup no README e o dono do repo (renanpr7) precisa adicionar o professor como colaborador.

## Acceptance Criteria
- [x] Comandos de setup do ambiente estão registrados no README (seção de pré-requisitos: `sudo apt install flex bison build-essential`)
- [ ] Professor `sergioaafreitas` está listado como colaborador (tarefa manual do dono do repositório)

## Technical Approach
**Decision:** orchestrator-decided
**Rationale:** Apenas documentar os passos de setup no README e indicar a tarefa manual. Como o ambiente é WSL e nós rodamos no Windows, garantimos apenas a documentação adequada.

## Architecture Fit
Apenas documentação básica no repositório.

## Implementation Plan
### Tasks
- [x] Task 1: Adicionar seção "Pré-requisitos e Setup (WSL/Ubuntu)" no arquivo `README.md` com os comandos `sudo apt update` e `sudo apt install flex bison build-essential`.
- [ ] Task 2: Lembrar o usuário de adicionar o professor `sergioaafreitas` como colaborador no GitHub.

### Implementation Order
1. Modificar `README.md`

### Files to Create/Modify
| File | Action | Purpose |
| --- | --- | --- |
| README.md | MODIFY | Adicionar comandos de setup |

## Testing Strategy
- **Unit tests:** N/A
- **Integration tests:** N/A
- **E2E tests:** N/A

## Risks and Considerations
- N/A

## Dependencies
- **External:** N/A
- **Internal:** N/A

## Evidence
- **Test Log:** N/A
- **Coverage:** N/A
- **Security Scan:** N/A
- **Review Verdict:** PASS

---
_Created by @orchestrator-nontdd_
