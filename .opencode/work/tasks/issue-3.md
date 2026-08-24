# Task: issue-3 — [CHORE] Criar .gitignore, hello Flex/Bison e Makefile (all/clean/test)

## Status: READY_TO_COMMIT

## Metadata
- **Type:** chore
- **Scope:** infrastructure
- **Priority:** high
- **Source:** GitHub Issue #3

## Problem Statement
O repositório precisa da fundação de build:
1. `.gitignore` excluindo artefatos gerados.
2. "hello" Flex/Bison mínimo para validar a toolchain.
3. Makefile completo com targets `all`, `clean`, `test` e pipeline `bison -d` → `flex` → `gcc -std=c99 -Wall -Wextra`.

## Acceptance Criteria
- [x] `make` compila o hello world Flex/Bison sem erros nem warnings no WSL (`-Wall -Wextra`, C99)
- [x] `make clean` remove todos os artefatos gerados (`.tab.c`, `.tab.h`, `lex.yy.c`, binário)
- [x] `make test` roda pelo menos 1 smoke test verde
- [x] `.gitignore` exclui arquivos compilados e logs
- [x] Decisão documentada: `%option noyywrap` no lexer OU linkar `-lfl`
- [x] `git status` limpo após `make`

## Technical Approach
**Decision:** orchestrator-decided
**Rationale:** Usaremos `%option noyywrap` no lexer para evitar a dependência do `libfl` na linkagem do gcc. Criaremos um lexer.l que reconhece qualquer caracter e imprime, e um parser.y vazio (apenas para testar o fluxo bison). A main() no c chamará yyparse().

## Architecture Fit
Padrão C99, compatível com as regras.

## Implementation Plan
### Tasks
- [x] Task 1: Criar `.gitignore` na raiz com exclusões corretas.
- [x] Task 2: Criar `parser/parser.y` (esqueleto básico).
- [x] Task 3: Criar `lexer/lexer.l` (esqueleto básico com `%option noyywrap`).
- [x] Task 4: Criar `src/main.c` chamando `yyparse()`.
- [x] Task 5: Criar `Makefile` com targets `all`, `clean`, e `test`. (Considerar %expect 1 tolerado).

### Implementation Order
1. .gitignore
2. parser/parser.y
3. lexer/lexer.l
4. src/main.c
5. Makefile

### Files to Create/Modify
| File | Action | Purpose |
| --- | --- | --- |
| .gitignore | MODIFY | Adicionar exclusões |
| parser/parser.y | CREATE | Esqueleto Bison |
| lexer/lexer.l | CREATE | Esqueleto Flex |
| src/main.c | CREATE | Ponto de entrada |
| Makefile | MODIFY | Regras de build |

## Testing Strategy
- **Unit tests:** N/A
- **Integration tests:** `make test` stub execution

## Dependencies
- **Blocked by:** issue-2
- **Blocks:** issue-5

## Evidence
- **Test Log:** N/A
- **Coverage:** N/A
- **Security Scan:** N/A
- **Review Verdict:** APPROVED

---
_Created by @orchestrator-nontdd_
