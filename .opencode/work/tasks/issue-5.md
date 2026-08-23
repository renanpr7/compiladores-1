# Task: issue-5 — [TEST] Criar runner de golden tests (tests/run_tests.sh) com estrutura de pastas

## Status: READY_TO_COMMIT

## Metadata
- **Type:** test
- **Scope:** infrastructure
- **Priority:** medium
- **Source:** GitHub Issue #5

## Problem Statement
Criar o esqueleto do runner de testes que espelha o `run_tests.sh` do professor: `tests/run_tests.sh` + pastas `tests/lexer/`, `tests/parser/aceita/`, `tests/parser/erros/`.

## Acceptance Criteria
[x] `tests/run_tests.sh` executa via `make test` no WSL
[x] Estrutura de pastas criada
[x] Pelo menos 1 smoke test (entrada `.simc` + `.expected`) passa verde
[x] Um teste que falha faz o runner retornar exit != 0
[x] Saída do runner lista testes passados/falhados individualmente

## Technical Approach
**Decision:** orchestrator-decided
**Rationale:** Escrever um script bash (`run_tests.sh`) que itera sobre os diretórios, roda o compilador (`./parser`) sobre os `.simc` e compara a saída com o `.expected` usando `diff`.

## Implementation Plan
### Tasks
[x] Task 1: Criar pastas `tests/lexer/`, `tests/parser/aceita/`, `tests/parser/erros/`.
[x] Task 2: Escrever `tests/run_tests.sh` (com suporte a verificação de exit code e `diff`).
[x] Task 3: Criar um smoke test (`tests/lexer/smoke.simc` e `.expected`).
[x] Task 4: Atualizar `Makefile` para invocar o `run_tests.sh` no target `test`.

### Implementation Order
1. Pastas
2. Smoke test
3. run_tests.sh
4. Makefile

### Files to Create/Modify
| File | Action | Purpose |
| --- | --- | --- |
| tests/run_tests.sh | CREATE | Runner de teste |
| tests/lexer/smoke.simc | CREATE | Teste básico |
| tests/lexer/smoke.expected | CREATE | Saída esperada |
| Makefile | MODIFY | Link com make test |

## Evidence
- **Review Verdict:** APPROVED

---
_Created by @orchestrator-nontdd_
