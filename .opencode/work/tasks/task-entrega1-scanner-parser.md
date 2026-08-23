# Task: task-entrega1-scanner-parser — Entrega 1 (v1.0): Scanner e Parser

## Status: PLANNING

## Metadata

- **Type:** feature
- **Scope:** backend (compilador CLI em C — Flex/Bison)
- **Priority:** high
- **Source:** Prompt (decisões de escopo em `.opencode/work/docs/proposta-escopo-simc.md` §3.2, §5, §6, §7)
- **Entrega:** **v1.0 — "MVP: Scanner e Parser"** (release list do GitHub)
- **Janela:** Sprints S1 + S2 (17/08/2026 → 21/09/2026) — marco P1 em 23/09/2026
- **Equipe:** 3 pessoas, Scrum semanal, dailies às quartas

---

## Problem Statement

O repositório está em estado **esqueleto**: `lexer/lexer.l`, `parser/parser.y`, `src/main.c` e `Makefile` existem mas estão **vazios** (0 linhas). Só há documentação (README, PROJECT_CONTEXT.md, proposta de escopo).

A **Entrega 1 (v1.0)** deve construir a **primeira metade do front-end** do compilador SimC, espelhando as **semanas 01–05** do repositório oficial do professor:

1. **Semana 01** — Ambiente (WSL), estrutura espelho, definição da linguagem (tokens/exemplos), "hello" Flex/Bison, Makefile.
2. **Semana 02** — Scanner: tokens iniciais, `%union`, números.
3. **Semana 03** — Scanner **definitivo**: `if`/`while`, IDENT, NUMBER (float), strings com escape, comentários `//` e `/* */`.
4. **Semana 04** — Parser com precedência (`%left`) e `%union`.
5. **Semana 05** — Recuperação de erros sintáticos (`error`, `yyerrok`, `yyclearin`) + mensagens contextuais com linha/coluna (decisão técnica #6 do escopo).

**Decisões da equipe (confirmadas em 19/08/2026):**

| Decisão | Regra adotada |
|---|---|
| Escopo do parser | **Núcleo fiel**: `int`/`float`, declaração, atribuição, `print` (multargs), `if`/`else`, `while`, blocos, expressões com precedência (`+ - * /`, relacionais, parênteses) |
| AST | **Não** — a Entrega 1 valida sintaxe e imprime artefatos (dump de tokens / aceita-rejeita). AST entra na Entrega 2 (semana 06) |
| Scanner | **Completo** (100% da linguagem prevista): reconhece todos os tokens, inclusive `string`, `for`, `return`, `&&`/`||`, `[ ]` — mesmo que a gramática núcleo ainda não os use |
| Preparação P1 | **Incluída** — bloco final do plano |

---

## Acceptance Criteria

- [ ] `make` compila no WSL Ubuntu com Flex + Bison, sem erros (`-Wall -Wextra`, C99)
- [ ] `make test` verde no WSL — golden tests de **lexer** (modo `-t`) e **parser** (aceita/rejeita + mensagens de erro) passam 100%
- [ ] `./parser < programa.simc` valida sintaxe do núcleo e reporta **OK** (exit 0) ou erros com **linha/coluna e mensagem contextual** (exit 1)
- [ ] `./parser -t < programa.simc` imprime o **dump de tokens** (tipo, lexema, linha:coluna) cobrindo **todos** os tokens da linguagem prevista no escopo
- [ ] O scanner trata: comentários `//` e `/* */`, literais string com escape `\"`, erros léxicos (caractere inválido, string/comentário não fechado)
- [ ] O parser aceita: declarações (com/sem init), atribuição, `print(a, b)`, `if/else` (aninhado), `while`, blocos, expressões com precedência correta
- [ ] O parser **recupera-se de erros**: um arquivo com N erros sintáticos reporta N mensagens (não para no primeiro)
- [ ] Recusa corretamente construções fora da v1 (ex.: `for`, `string`, `array`) com mensagem clara
- [ ] `docs/linguagem-simc.md` (especificação) + relatórios semanais (`docs/semana01.md`…`semana05.md`) criados
- [ ] Release **v1.0** publicada no GitHub com release notes; milestone P1 criado
- [ ] Formulário P1 enviado pelo líder + apresentação de 5 min ensaiada (com demo ao vivo no WSL)

---

## Technical Approach

**Decision:** Scanner completo (Flex) + Parser núcleo com precedência e recuperação de erros (Bison), integrados por `src/main.c` com dois modos de operação (`-t` = dump de tokens; default = validação sintática). Golden tests por fase via `make test`.

**Origin:** user-driven (decisões confirmadas em 19/08/2026) + decisões técnicas do escopo (§6)

**Rationale:**

- **Scanner completo já na Entrega 1** é barato (regras de flex) e dá um artefato forte para a P1: "reconhecemos 100% da linguagem prevista". Evita retrabalho no lexer na Entrega 2.
- **Parser núcleo** fiel às semanas 04–05 e ao calendário ("parser básico" na S2). A gramática ambiciosa (funções, `for`, `string`, `array`, `&&`/`||`) entra na Entrega 2, quando o parser ganha AST — evita escrever regras que seriam reescritas junto com a semântica.
- **Sem AST**: o escopo coloca a semana 06 (AST + tabela de símbolos) na Entrega 2. Construir AST agora inflaria a v1.0 e os testes antes da P1 sem retorno imediato.
- **Mensagens de erro com linha/coluna** desde já: decisão técnica #6 do escopo (diferencial barato de qualidade, já exigido na semana 05).
- **Golden tests + `make test`**: formato obrigatório por fase (§6.3 e dicas finais da disciplina).

**Decisões de design tomadas neste plano (para registro):**

| # | Decisão | Regra |
|---|---|---|
| D1 | Tokens fora da gramática v1 | Lexer reconhece `for`, `string`, `return`, `&&`, `||`, `[ ]`; parser núcleo não os usa (Bison avisa "token unused" — aceitável, não é erro) |
| D2 | Erro léxico | Reporta "caractere inválido 'X' na linha L, coluna C", **continua** a varredura, exit 1 ao final |
| D3 | Float malformado (`1.`, `1e3`) | Regra específica no flex: `[0-9]+\.` → "número malformado '1.' na linha L" (sem ela, `1.` viraria `NUMBER_INT 1` + "caractere inválido '.'"); `1e3` → não suportado (fora da linguagem), erro léxico |
| D4 | Dangling else | `%expect 1` documentado — `else` liga ao `if` mais próximo (comportamento C/C++) |
| D5 | Número máximo de erros reportados | Cap de 10 mensagens por arquivo para não inundar a saída |
| D6 | Saída do parser | stdout: "OK" (aceite) / dump de tokens (`-t`); stderr: mensagens de erro |
| D7 | Artefatos gerados | `parser/parser.tab.c`, `parser/parser.tab.h`, `lex.yy.c`, binário `parser` na raiz (espelho do professor); todos no `.gitignore` |

---

## Architecture Fit

Integra-se ao pipeline definido no PROJECT_CONTEXT.md §3 (front-end monolítico C):

```
programa SimC
   │
   ▼
[Flex] lexer.l ──tokens──► [Bison] parser.y (precedência + recuperação de erros)
   │                           │
   └── dump -t (artefato v1)   ▼
                         validação sintática (aceita/rejeita + mensagens)
```

- **v1.0 (esta entrega):** Flex → Bison → validação sintática. O Bison **não** constrói AST (semana 06 fica para a Entrega 2).
- **v2.0 (próxima entrega):** o mesmo parser evolui para construir a AST → semântica → TAC (trilho didático) → gerador C++ (trilho real). A gramática núcleo de v1 é a base que a Entrega 2 estende (funções, `for`, `string`, `array`, `&&`/`||`).
- **Convenções respeitadas (PROJECT_CONTEXT §5):** `snake_case`, tokens MAIÚSCULAS no Bison, headers com guardas `#ifndef`, commits `[ADD]/[FIX]/[DOCS]/[REF]`, branches `LEX-`/`SIN-`/`DOC-`.

---

## Implementation Plan

### Escopo Fora da Entrega 1 (Não-Fazer — explícito)

| Não entra na v1.0 | Motivo |
|---|---|
| AST (`NoAST`) e tabela de símbolos | Semana 06 → Entrega 2 (S3) |
| Análise semântica (tipos, declaração-uso, coerção) | Semana 07 → Entrega 2 (S4) |
| `for`, funções/`return`, `string`, `array`, `&&`/`||` na **gramática** | Parser núcleo fiel (decisão da equipe) — os **tokens** já existem no lexer |
| TAC, constant folding, geração de C++ | Semanas 08–10 → Entrega 2 |
| `struct`, bounds check de array, `else if`, `do while` | Extra pós-P2 |

> Se a S2 atrasar, a rampa de fuga **nunca** corta o núcleo (MoSCoW Must): cortam-se casos de teste redundantes, o `&&`/`||` do lexer (1 linha, adiado) e o capricho dos relatórios.

---

### Tasks

#### Bloco A — Sprint 1: Ambiente e Fundações (17/08 → 07/09)

- [ ] **T1.1** [SETUP] Validar ambiente canônico WSL Ubuntu (`sudo apt install flex bison build-essential`) e registrar no README os comandos de setup; confirmar `flex --version` / `bison --version`
- [ ] **T1.2** [DOC] Decidir **nome oficial da linguagem** (SimC é provisório) e registrar em `docs/linguagem-simc.md`
- [ ] **T1.3** [DOC] Criar `docs/linguagem-simc.md` — especificação: lista de tokens (Tabela 1 abaixo), gramática núcleo (BNF), exemplos de programa, convenções da linguagem
- [ ] **T1.4** [ADD] Criar `.gitignore` — `parser.tab.c`, `parser.tab.h`, `lex.yy.c`, binário `parser`, `.opencode/node_modules/`, logs, artefatos de build
- [ ] **T1.5** [ADD] "Hello" Flex/Bison mínimo (lexer devolve EOF, parser vazio, `main.c` chama `yyparse`) — **valida a toolchain antes de escrever o scanner real**
- [ ] **T1.6** [ADD] `Makefile` completo: targets `all`, `clean`, `test` (e `tokens` opcional) — pipeline `bison -d` → `flex` → `gcc -std=c99 -Wall -Wextra -lfl`; decidir **um** dos dois: `%option noyywrap` no lexer **ou** linkar `-lfl` (nunca ambos)
- [ ] **T1.7** [ADD] Esqueleto `tests/run_tests.sh` + pastas `tests/lexer/`, `tests/parser/aceita/`, `tests/parser/erros/`; 1 smoke test verde via `make test`
- [ ] **T1.8** [SETUP] Adicionar o professor `sergioaafreitas` como **colaborador** do repositório (obrigação do escopo §3.1 — ação do dono do repo, `renanpr7`)
- [ ] **T1.9** [DOC] `docs/semana01.md` — relatório: ambiente, estrutura, decisão do nome, problemas encontrados

#### Bloco B — Sprint 1: Scanner — tokens iniciais → dump + testes (17/08 → 07/09 · movido da S2 no rebalanceamento de 19/08)

- [ ] **T2.1** [ADD] `lexer/lexer.l`: regras base — IDENT `[a-zA-Z_][a-zA-Z0-9_]*`, NUMBER int `[0-9]+` e float `[0-9]+\.[0-9]+`, operadores, pontuação, whitespace
- [ ] **T2.2** [ADD] `lexer/lexer.l`: palavras reservadas (9) com **precedência sobre IDENT** — `int`, `float`, `string`, `print`, `if`, `else`, `while`, `for`, `return`
- [ ] **T2.3** [ADD] `lexer/lexer.l`: literal string `"..."` com escape `\"`, comentários `//` e `/* */` (descartados); erro se string/comentário não fechado
- [ ] **T2.4** [ADD] Rastreamento de **linha e coluna** (contador de coluna resetado em `\n`) exposto ao parser (`yylineno` + variável de coluna)
- [ ] **T2.5** [ADD] `parser/parser.y`: `%union` + declaração `%token` de **todos** os tokens do scanner (inclusive os que a gramática v1 não usa — D1); esqueleto que compila. **Nota:** em v1 os campos do `%union` (valor numérico, string) ficam **declarados mas não consumidos** — as ações das regras são vazias (sem AST/semântica). Os campos existem já para a Entrega 2
- [ ] **T2.6** [ADD] `src/main.c`: modo `-t/--tokens` — dump de tokens `linha:coluna TOKEN "lexema"` por linha + `EOF` ao final; exit 0/1 conforme erros léxicos; integração com o lexer
- [ ] **T2.7** [TEST] `tests/lexer/` positivos: tokens simples, literais (int/float/string com escape), palavras reservadas, comentários entre tokens
- [ ] **T2.8** [TEST] `tests/lexer/` erros léxicos: caractere inválido (`$`, `@`), string aberta, comentário `/*` aberto, float malformado `1.` (D3)
#### Bloco C — Sprint 2: Scanner definitivo + validação (08/09 → 14/09)

- [ ] **T2.9** [DOC] `docs/semana02.md` — relatório (registrar: scanner cobre 100% da linguagem já na v1) — relatório da semana 02, escrito no início da S2 (backfill do scanner feito na S1)

- [ ] **T3.1** [FIX] Revisão de **gap check**: conferir o lexer contra a Tabela 1 (todos os tokens do escopo) e contra casos de borda — IDENT iniciando com dígito, linha longa, EOF no meio de comentário/string
- [ ] **T3.2** [TEST] Casos de fumaça: arquivo vazio, só comentários, programa com comentários entre comandos, string contendo `//` dentro (não deve virar comentário)
- [ ] **T3.3** [DOC] `docs/semana03.md` — relatório (scanner definitivo congelado; critérios de aceite do scanner)

#### Bloco D — Sprint 2: Parser núcleo com precedência (08/09 → 14/09, paralelo ao C)

- [ ] **T4.1** [ADD] `parser/parser.y`: gramática núcleo completa (programa, comandos, declaração com/sem init, atribuição, `print` multargs, `if/else`, `while`, blocos) — ver BNF abaixo
- [ ] **T4.2** [ADD] `parser/parser.y`: cadeia de precedência `%left` (EQ/NE < LT/GT/LE/GE < PLUS/MINUS < TIMES/DIVIDE), unário `-` (`%prec UMINUS` — `UMINUS` entra na **lista `%left`** como pseudo-token, não em `%token`), parênteses; `%expect 1` documentado para dangling else (D4)
- [ ] **T4.3** [ADD] `src/main.c`: modo default — executa `yyparse`; stdout "OK" + exit 0; stderr mensagens de erro + exit 1; exit 2 para erro de uso; cap de 10 erros (D5)
- [ ] **T4.4** [TEST] `tests/parser/aceita/`: declarações (com/sem init), atribuições, expressões de precedência (`a + b * 2`, `(a + b) * 2`, `-a * b`), `if/else` aninhado, `while`, blocos, `print(a, b, 42)`, `(a < b) == c` (relacional associando à esquerda)
- [ ] **T4.5** [TEST] `tests/parser/erros/`: faltou `;`, faltou `)`, `else` solto, `if` sem parênteses, `while` sem corpo, construções fora da v1 (`for`, `string`, `return`) → mensagem esperada
- [ ] **T4.6** [DOC] `docs/linguagem-simc.md` — seção "Exemplos validados" (programas testados pelo `make test`)
- [ ] **T4.7** [DOC] `docs/semana04.md` — relatório (precedência/associatividade adotadas, conflitos resolvidos)

#### Bloco E — Sprint 2: Recuperação de erros + robustez (15/09 → 18/09)

- [ ] **T5.1** [ADD] `parser/parser.y`: regra `error` de recuperação em nível de comando/bloco com `yyerrok` + `yyclearin` — parser continua após erro
- [ ] **T5.2** [ADD] `yyerror` aprimorado: mensagem contextual com posição — `"esperava ';' na linha 3, coluna 10"` (decisão #6 do escopo); anexar lexema/token inesperado quando disponível
- [ ] **T5.3** [TEST] `tests/parser/erros/multiplos_erros.simc`: arquivo com 3 erros → **3 mensagens** com linhas corretas (prova a recuperação)
- [ ] **T5.4** [DOC] `docs/semana05.md` — relatório (estratégia de recuperação + exemplos de mensagens)

#### Bloco F — Sprint 2: Integração, Release v1.0 e P1 (19/09 → 23/09)

- [ ] **T6.1** [ADD] `make test` unificado (lexer + parser) — rodada de regressão completa verde no **WSL**
- [ ] **T6.2** [TEST] Casos de borda finais: vazio, só comentário, programa demo da P1 (aceito), demo com erro (mensagem contextual)
- [ ] **T6.3** [DOC] Atualizar `README.md` (uso do parser v1.0, comandos, exemplos) e `PROJECT_CONTEXT.md` se necessário (novas convenções/lições)
- [ ] **T6.4** [DOC] Script de demonstração da P1: 1 programa SimC núcleo → dump de tokens → parse OK; 1 programa com erro → mensagem contextual (tudo executável no WSL em <1 min)
- [ ] **T6.5** [REL] GitHub: criar milestone **P1**; publicar release **v1.0** com release notes (Scanner completo + Parser núcleo + testes); tag `v1.0`
- [ ] **T6.6** [DOC] Preparação P1: preencher formulário P1 (líder — forms.office.com/r/MyKh4HiAAu), slides 5 min (decisões §6 do escopo → o que foi implementado → demo → planejamento/MoSCoW → próximos passos), ensaio com cronômetro

---

### Implementation Order

Ordem guiada por **dependência técnica** e **feedback cedo**:

1. **Bloco A (T1.1→T1.9):** fundações primeiro — sem Makefile/toolchain verdes, nada funciona. O "hello Flex/Bison" (T1.5) valida a toolchain antes de qualquer regra real.
2. **Bloco B (T2.1→T2.8, na S1):** scanner antes do parser — o parser consome os tokens; o dump `-t` (T2.6) + testes do lexer (T2.7/T2.8) **congelam** o scanner com feedback imediato. (T2.9 relatório migrou para a S2)
3. **Blocos C e D (paralelos):** gap check do scanner (C) enquanto a gramática núcleo (D) é escrita — independem entre si.
4. **Bloco E:** recuperação de erros por último, sobre gramática já estável (semana 05 do professor).
5. **Bloco F:** integração final, release e P1.

> **Regra de ouro:** cada tarefa termina com `make` + `make test` verdes no WSL antes do commit. Commits pequenos e frequentes (padrão `[ADD]/[FIX]/[DOCS]/[REF]`), branches `LEX-`/`SIN-`/`DOC-`.

---

### Files to Create/Modify

| File | Action | Purpose |
|---|---|---|
| `Makefile` | MODIFY | Targets `all`/`clean`/`test`; pipeline bison→flex→gcc (T1.6) |
| `.gitignore` | CREATE | Excluir artefatos gerados (T1.4) |
| `lexer/lexer.l` | MODIFY | Scanner completo (tokens, strings, comentários, linha/coluna) — T2.1–T2.4, T3.1 |
| `parser/parser.y` | MODIFY | `%union`, `%token`, gramática núcleo, precedência, recuperação de erros — T2.5, T4.1–T4.2, T5.1–T5.2 |
| `src/main.c` | MODIFY | CLI (`-t`, default, `-h`), dump de tokens, `yyerror` contextual, exit codes — T2.6, T4.3, T5.2 |
| `docs/linguagem-simc.md` | CREATE | Especificação da linguagem: tokens, BNF, exemplos (T1.3, T4.6) |
| `docs/semana01.md` … `docs/semana05.md` | CREATE | Relatórios semanais espelho (T1.9, T2.9, T3.3, T4.7, T5.4) |
| `tests/run_tests.sh` | CREATE | Runner de golden tests (T1.7, T6.1) |
| `tests/lexer/*.simc` + `.expected` | CREATE | Golden tests do scanner (T2.7, T2.8, T3.2) |
| `tests/parser/aceita/*.simc` + `.expected` | CREATE | Golden tests de aceitação (T4.4) |
| `tests/parser/erros/*.simc` + `.expected` | CREATE | Golden tests de rejeição/erros (T4.5, T5.3) |
| `README.md` | MODIFY | Uso do parser v1.0, comandos, exemplos (T6.3) |
| `.opencode/work/tasks/task-entrega1-scanner-parser.md` | CREATE | Este plano |

---

### Language Specification (Entrega 1) — Tabela 1: Tokens

| Categoria | Tokens (nomes Bison) | Lexema |
|---|---|---|
| Palavras reservadas (9) | `INT`, `FLOAT`, `STRING`, `PRINT`, `IF`, `ELSE`, `WHILE`, `FOR`, `RETURN` | `int float string print if else while for return` |
| Identificador | `IDENT` | `[a-zA-Z_][a-zA-Z0-9_]*` |
| Número inteiro | `NUMBER_INT` | `[0-9]+` |
| Número ponto flutuante | `NUMBER_FLOAT` | `[0-9]+\.[0-9]+` |
| Literal string | `STRING_LITERAL` | `"..."` com escape `\"` |
| Aritméticos | `PLUS`, `MINUS`, `TIMES`, `DIVIDE` | `+ - * /` |
| Relacionais | `EQ`, `NE`, `LT`, `GT`, `LE`, `GE` | `== != < > <= >=` |
| Lógicos (lexer só) | `AND`, `OR` | `&& \|\|` |
| Atribuição | `ASSIGN` | `=` |
| Pontuação | `LPAREN`, `RPAREN`, `LBRACE`, `RBRACE`, `SEMI`, `COMMA`, `LBRACKET`, `RBRACKET` | `( ) { } ; , [ ]` |
| Comentários | — (descartados) | `// ...` e `/* ... */` |
| Fim de arquivo | `EOF` | — |

> **Nota:** `STRING`, `FOR`, `RETURN`, `AND`, `OR`, `LBRACKET`, `RBRACKET` são reconhecidos pelo lexer mas **não usados pela gramática v1** (D1) — o parser rejeita programas que os exijam, com mensagem clara.

### Gramática Núcleo (BNF — v1.0)

```
programa         : lista_comandos
lista_comandos   : /* vazio */
                 | lista_comandos comando
comando          : declaracao
                 | atribuicao
                 | print_stmt
                 | if_stmt
                 | while_stmt
                 | bloco
declaracao       : tipo IDENT SEMI
                 | tipo IDENT ASSIGN expressao SEMI
tipo             : INT
                 | FLOAT
atribuicao       : IDENT ASSIGN expressao SEMI
print_stmt       : PRINT LPAREN lista_expr RPAREN SEMI
lista_expr       : expressao
                 | lista_expr COMMA expressao
if_stmt          : IF LPAREN expressao RPAREN comando
                 | IF LPAREN expressao RPAREN comando ELSE comando
while_stmt       : WHILE LPAREN expressao RPAREN comando
bloco            : LBRACE lista_comandos RBRACE
expressao        : expr_rel
expr_rel         : expr_arit
                 | expr_rel (EQ | NE | LT | GT | LE | GE) expr_arit
expr_arit        : termo
                 | expr_arit (PLUS | MINUS) termo
termo            : fator
                 | termo (TIMES | DIVIDE) fator
fator            : MINUS fator          /* unário: %prec UMINUS */
                 | primario
primario         : IDENT
                 | NUMBER_INT
                 | NUMBER_FLOAT
                 | LPAREN expressao RPAREN
```

**Exemplo ilustrativo de programa v1 válido** (também é o embrião da demo da P1, T6.4):

```c
int a = 10;
float b = 2.5;
int soma = a + b * 2;        // precedência: a + (b * 2)
if (soma > 20) { print(soma, a); } else { print(0); }
while (a > 0) { a = a - 1; }
```

### CLI Contract (v1.0)

| Invocação | Saída | Exit |
|---|---|---|
| `./parser < prog.simc` | stdout: `OK` | 0 |
| `./parser < prog.simc` (com erros) | stderr: `erro: <mensagem contextual>` (máx. 10, D5) | 1 |
| `./parser -t < prog.simc` | stdout: dump `linha:coluna TOKEN "lexema"` por linha, terminando com `linha:coluna EOF` (sem lexema) | 0 (sem erro léxico) / 1 (com erro léxico) |
| `./parser -h` | stdout: uso | 0 |
| flag inválida | stderr: mensagem de uso | 2 |

Formato da mensagem de erro: `erro: esperava ';' na linha 3, coluna 10` (decisão #6 do escopo).

### Database Changes

N/A — compilador CLI sem persistência.

---

## Testing Strategy

- **Framework:** golden tests — `tests/**/*.simc` (entrada) + `.expected` (saída exata), executados por `tests/run_tests.sh` via `make test` (espelho do `run_tests.sh` do professor, §6.3).
- **Lexer (modo `-t`):** `.expected` = dump de tokens esperado, linha a linha (difícil de falsar — qualquer mudança de lexema/posição quebra o diff).
- **Parser aceita:** `.expected` = `OK` (stdout) + exit 0.
- **Parser erros:** `.expected` = mensagens de erro exatas (stderr) — cobre mensagem contextual, recuperação e exit 1.
- **Casos obrigatórios:**
  - *Lexer:* tokens simples, literais (int/float/string com escape), palavras reservadas, comentários (`//`, `/* */`, `//` dentro de string), caractere inválido, string aberta, comentário aberto, `1.` malformado, arquivo vazio, só comentários.
  - *Parser aceita:* declarações com/sem init, atribuição, expressões de precedência (`a + b * 2` ≠ `(a + b) * 2`; `-a * b` = `(-a) * b`), `if/else` aninhado, `while`, blocos, `print(a, b, 42)`.
  - *Parser erros:* faltou `;`, faltou `)`, `else` solto, `if` sem parênteses, `for`/`string`/`return` (fora da v1), arquivo com 3 erros → 3 mensagens.
- **E2E (leve):** pipeline completo stdin → dump/validação; a geração de C++ é da Entrega 2.
- **Verificação final:** `make` e `make test` verdes no **WSL** (ambiente canônico) antes da release v1.0.

---

## Risks and Considerations

| Risco | Impacto | Mitigação |
|---|---|---|
| Desenvolvimento em Windows, ambiente canônico WSL | `make` verde no Windows ≠ verde no WSL | Rodar `make test` no WSL antes de todo commit grande e da release (regra de ouro) |
| CRLF vs LF no git | Regras do flex sensíveis a `\r` quebram silenciosamente | Configurar `core.autocrlf`/`.gitattributes` na T1.4; validar no WSL |
| Conflitos shift/reduce na gramática | Comportamento inesperado de precedência | `%left` explícito + `%expect 1` (dangling else) documentado + testes de precedência (T4.4) |
| Tokens não usados pelo parser v1 (`for`, `string`, …) | Warnings do Bison | Aceitar/registrar (D1) — não virar erro |
| Atraso na S2 (janela curta até P1) | Risco na v1.0 | Rampa de fuga escrita (§Escopo Fora): cortar casos redundantes, `&&`/`||` do lexer, capricho dos relatórios — **núcleo nunca corta** |
| Recuperação de erros infinita/mensagens em cascata | Saída confusa | Regra `error` em nível de comando + cap de 10 erros (D5) |
| Escopo rasteiro ("só mais um token…") | Estouro da S2 | DoD por bloco + revisão nas dailies de quarta; fora-da-v1 documentado como rejeição, não como trabalho |

---

## Dependencies

- **External:** Flex, Bison, GCC/g++ (WSL Ubuntu — `apt install flex bison build-essential`); GitHub (release v1.0, milestone P1).
- **Internal (docs que o plano consome):** `proposta-escopo-simc.md` (§3.2 semanas 01–05, §5 MoSCoW, §6 decisões, §7 sprints), `PROJECT_CONTEXT.md` (§2 comandos, §3 arquitetura, §5 convenções, §6 testes), `README.md` (convenções de branch/commit).
- **Nenhuma dependência de código** — repositório é esqueleto; tudo é criado nesta entrega.

---

## Sprint Mapping

| Sprint | Período | Blocos | Carga | Marco |
|---|---|---|---|---|
| S1 | 17/08 → 07/09 | A (fundações) + B (scanner base: T2.1–T2.8) | 17 tasks (~77%) | — |
| S2 | 08/09 → 21/09 | T2.9 + C + D + E + F | 21 tasks (~140%) | **P1 23/09** |

> Volume: **38 tasks** em 5 semanas para 3 pessoas — ~2,5 tasks/pessoa/semana, dentro da capacidade estimada no escopo (~61 issues no total do projeto). **Rebalanceamento em 19/08 (Proposta A aprovada):** o escopo original deixava a S1 em ~32% (grande parte já pronta: `.gitignore`, estrutura espelho, README/PROJECT_CONTEXT) e a S2 em ~193%. O scanner base (T2.1–T2.8) foi movido para a S1 → S1 = 17 tasks (~77%) e S2 = 21 tasks (~140%, foco em parser + erros + release/P1).

---

## Evidence (filled by tester/reviewer)

- **Test Log:** `<path — filled after testing>`
- **Coverage:** `<path — filled after testing>`
- **Security Scan:** N/A (acadêmico, sem serviço) — revisão de código manual
- **Review Verdict:** `<APPROVED|CHANGES_REQUESTED — filled after review>`

---

## DoD Final (Definition of Done — v1.0)

- [ ] `make` e `make test` verdes no WSL
- [ ] 100% dos golden tests passando (lexer + parser)
- [ ] Demo P1 executável ao vivo (token dump + OK + erro contextual)
- [ ] Release v1.0 publicada + milestone P1 no GitHub
- [ ] Formulário P1 enviado + apresentação ensaiada
- [ ] Relatórios semanais 01–05 + especificação da linguagem no repo

---

## Open Items

- [ ] **Nome oficial da linguagem** — SimC é provisório (T1.2); impacta docs e mensagens do compilador
- [ ] **Formato exato do dump `-t`** — a forma `linha:coluna TOKEN "lexema"` é proposta; congelar nos `.expected` na T2.6/T2.7
- [ ] **Estrutura final dos gerados** (`parser.tab.c` na raiz vs `build/`) — seguir o espelho do professor (raiz, D7), revisar se o Bison 3.x gerar conflito com o `%expect 1`
- [ ] **Confirmar versões do Bison no WSL** — `%union`/`%left` são portáveis; validar com o Bison instalado (T1.1)
- [ ] **Demonstração P1** — qual programa demo apresentar (embrião no exemplo acima); decidir na S2

---

## Histórico de Revisões

| Data | Mudança |
|---|---|
| 19/08/2026 | **Rebalanceamento S1/S2 (Proposta A aprovada):** scanner base (T2.1–T2.8) movido da S2 para a S1; T2.9 migrado para o Bloco C (S2); milestones do GitHub (Sprint 1 e Sprint 2) atualizadas |
| 19/08/2026 | Revisão do plano: D3 corrigido, `%union`/`noyywrap`/`UMINUS` documentados, T1.8 (professor colaborador) adicionada, exemplo de programa, Open Items |

---

_Created by @plan-maker_
_Last updated: 19/08/2026_