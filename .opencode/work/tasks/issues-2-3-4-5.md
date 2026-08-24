# Task: issues-2-3-4-5 — Fundação do Projeto (Ambiente, Build, Especificação, Testes)

## Status: READY_TO_COMMIT

## Metadata

- **Type:** chore, docs, test
- **Scope:** backend (compilador CLI em C — Flex/Bison)
- **Priority:** high
- **Source:** GitHub Issues #2, #3, #4, #5
- **Branch:** starting-compiler

## Problem Statement

O repositório está em estado esqueleto: `lexer/lexer.l`, `parser/parser.y`, `src/main.c` e `Makefile` existem mas estão vazios. Precisamos criar a fundação completa do projeto:

1. **Issue #2:** Setup do ambiente canônico WSL + docs de pré-requisitos no README
2. **Issue #3:** `.gitignore` correto + hello Flex/Bison mínimo + Makefile completo (`all`, `clean`, `test`)
3. **Issue #4:** Nome oficial da linguagem + especificação `docs/linguagem-simc.md` (tokens, BNF, exemplos)
4. **Issue #5:** Runner de golden tests (`tests/run_tests.sh`) + estrutura de pastas + 1 smoke test

## Acceptance Criteria

- [ ] `make` compila o hello world Flex/Bison sem erros nem warnings no WSL (`-Wall -Wextra`, C99)
- [ ] `make clean` remove todos os artefatos gerados sem erro
- [ ] `make test` roda pelo menos 1 smoke test verde
- [ ] `.gitignore` exclui artefatos gerados (`.tab.c`, `.tab.h`, `lex.yy.c`, binário, `node_modules`, logs)
- [ ] Decisão documentada: `%option noyywrap` OU linkar `-lfl` (nunca ambos)
- [ ] `git status` limpo após `make`
- [ ] `docs/linguagem-simc.md` criado com Tabela 1 de tokens, BNF v1.0, 2+ exemplos válidos, 1 exemplo com erro
- [ ] Nome oficial da linguagem decidido e registrado no README
- [ ] `tests/run_tests.sh` executável, integrado ao `make test`
- [ ] Estrutura `tests/lexer/`, `tests/parser/aceita/`, `tests/parser/erros/` criada
- [ ] Pelo menos 1 smoke test (`.simc` + `.expected`) passa verde
- [ ] Runner retorna exit != 0 quando teste falha
- [ ] README atualizado com seção de pré-requisitos (comandos WSL)

## Technical Approach

**Decision:** Criar fundação completa do projeto seguindo o plano `task-entrega1-scanner-parser.md`.

**Origin:** collaborative (issues #2–#5 do GitHub)

**Rationale:** Essas issues são a fundação — sem Makefile funcional, sem toolchain validada, sem especificação, nada mais avança. São interdependentes e devem ser feitas juntas.

## Architecture Fit

```
programa SimC
   │
   ▼
[Flex] lexer.l ──tokens──► [Bison] parser.y
   │                           │
   └── dump -t (artefato v1)   ▼
                         validação sintática
```

- Pipeline fixo: `bison -d` → `flex` → `gcc -std=c99 -Wall -Wextra`
- C99 estrito
- `%option noyywrap` no lexer (decisão — evita `-lfl`)
- Artefatos gerados na raiz (espelho do professor): `parser.tab.c`, `parser.tab.h`, `lex.yy.c`, binário `parser`

## Implementation Plan

### Tasks

#### Issue #3 — .gitignore + Hello Flex/Bison + Makefile

- [x] **T3.1** Atualizar `.gitignore` — excluir `parser.tab.c`, `parser.tab.h`, `lex.yy.c`, binário `compilador`, `.opencode/node_modules/`, logs, artefatos de build, `*.o`
- [x] **T3.2** Criar `lexer/lexer.l` mínimo — `%option noyywrap noinput nounput`, regra ignora caracteres, `#include "parser.tab.h"`
- [x] **T3.3** Criar `parser/parser.y` mínimo — tokens declarados, regra inicial `programa: /* vazio */`, `void yyerror(const char *s)` básico
- [x] **T3.4** Criar `src/main.c` mínimo — `#include <stdio.h>`, `extern int yyparse()`, `int main() { yyparse(); return 0; }`
- [x] **T3.5** Criar `Makefile` — targets `all`, `clean`, `test`; pipeline `bison -d` → `flex` → `gcc -std=c99 -Wall -Wextra -D_DEFAULT_SOURCE`; `%expect 0` (sem dangling else ainda)
- [x] **T3.6** Testar `make` no WSL — compilar sem erros nem warnings ✅
- [x] **T3.7** Testar `make clean` no WSL — remover artefatos ✅
- [x] **T3.8** Verificar `git status` limpo após `make clean` ✅

#### Issue #4 — Especificação da Linguagem

- [x] **T4.1** Nome oficial: SimC (provisório — registrado em README e docs)
- [x] **T4.2** Criar `docs/linguagem-simc.md` — Tabela 1 de tokens completa (36 tokens)
- [x] **T4.3** Documentar BNF v1.0 com precedência de operadores
- [x] **T4.4** 2+ exemplos válidos (Hello World, Média) + 1 erro de sintaxe
- [x] **T4.5** Convenções documentadas (escape strings, comentários, tipos v1)
- [x] **T4.6** README atualizado — pré-requisitos, versões, build/test/clean

#### Issue #5 — Runner de Golden Tests

- [x] **T5.1** Estrutura criada: `tests/lexer/`, `tests/parser/aceita/`, `tests/parser/erros/`
- [x] **T5.2** `tests/run_tests.sh` — executável, compara .simc vs .expected, pass/fail colorido, exit code correto
- [x] **T5.3** Integrado ao Makefile via target `test`
- [x] **T5.4** Smoke test `tests/lexer/hello.simc` + `.expected`
- [x] **T5.5** `make test` passa verde ✅
- [x] **T5.6** Teste falho retorna exit != 0 ✅

#### Issue #2 — Validação do Ambiente

- [x] **T2.1** flex 2.6.4, bison 3.8.2, gcc 13.3.0, make 4.3 confirmados via WSL
- [x] **T2.2** Versões documentadas no README
- [x] **T2.3** `make` e `make test` rodando sem erros de ambiente
- [x] **T2.4** Professor `sergioaafreitas` registrado no README como pendência do dono `renanpr7`

### Implementation Order

1. **T3.1–T3.5** — Criar arquivos (`.gitignore`, lexer, parser, main, Makefile)
2. **T3.6–T3.8** — Validar build no WSL
3. **T4.1–T4.6** — Criar especificação + atualizar README
4. **T5.1–T5.6** — Criar runner + testes + validar
5. **T2.1–T2.4** — Validar ambiente + documentar

### Files to Create/Modify

| File | Action | Purpose |
|------|--------|---------|
| `.gitignore` | MODIFY | Excluir artefatos gerados (T3.1) |
| `lexer/lexer.l` | MODIFY | Hello Flex mínimo (T3.2) |
| `parser/parser.y` | MODIFY | Hello Bison mínimo (T3.3) |
| `src/main.c` | MODIFY | Main que chama yyparse (T3.4) |
| `Makefile` | MODIFY | Targets all/clean/test (T3.5) |
| `docs/linguagem-simc.md` | CREATE | Especificação da linguagem (T4.2–T4.5) |
| `README.md` | MODIFY | Pré-requisitos + nome linguagem (T4.6) |
| `tests/run_tests.sh` | CREATE | Runner de golden tests (T5.2) |
| `tests/lexer/*.simc` + `.expected` | CREATE | Smoke test do lexer (T5.4) |

### Decisões de Design

| # | Decisão | Regra |
|---|---------|-------|
| D1 | `%option noyywrap` | Usar no lexer (evita `-lfl`) |
| D2 | Artefatos gerados | Na raiz: `parser.tab.c`, `parser.tab.h`, `lex.yy.c`, binário `compilador` (renomeado de `parser` para evitar conflito com dir `parser/`) |
| D3 | Dangling else | `%expect 0` (v1 sem if/else; mudar para `%expect 1` ao adicionar KW_IF/KW_ELSE) |
| D4 | Pipeline | `bison -d` → `flex` → `gcc -std=c99 -Wall -Wextra` |

## Testing Strategy

- **Framework:** golden tests — `tests/**/*.simc` (entrada) + `.expected` (saída exata)
- **Runner:** `tests/run_tests.sh` via `make test`
- **Smoke test:** 1 teste lexer simples que compara dump de tokens
- **Verificação final:** `make` e `make test` verdes no WSL

## Risks and Considerations

| Risco | Mitigação |
|-------|-----------|
| CRLF vs LF (flex sensível a `\r`) | Configurar `core.autocrlf` se necessário |
| Bison 3.x com `%expect 1` | Validar no WSL que warning é aceito |
| `%option noyywrap` vs `-lfl` | Decidir e documentar (escolhido: noyywrap) |

## Evidence (filled by tester/reviewer)

- **Test Log:** `<path — filled after testing>`
- **Coverage:** `<path — filled after testing>`
- **Security Scan:** N/A (acadêmico)
- **Review Verdict:** `<APPROVED|CHANGES_REQUESTED — filled after review>`

---

_Created by @orchestrator-nontdd_
_Last updated: 2026-08-23_
