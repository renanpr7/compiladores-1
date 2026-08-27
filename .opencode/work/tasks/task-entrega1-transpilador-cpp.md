# Task: task-entrega1-transpilador-cpp — Entrega 1: Scanner e Parser do Transpilador C++ → SimC

## Status: PLANNING

## Metadata

- **Type:** feature + refactor
- **Scope:** full-stack (docs, gestão, lexer, parser, testes)
- **Priority:** high
- **Source:** Pivot para Transpilador C++ → SimC
- **Entrega:** **v1.0 — "MVP: Scanner e Parser C++"**
- **Equipe:** 3 pessoas

---

## Problem Statement

O projeto sofreu um **pivô estratégico**: abandonamos a compilação SimC → C++ (complexidade insustentável de mapear para OO) e adotamos a direção inversa — um **Transpilador de C++ (subconjunto imperativo) para SimC**.

**Estado atual do repositório:**
- Arquivos base existem mas estão **praticamente vazios** (lexer.l ignora tudo, parser.y sem regras reais)
- O plano antigo (`task-entrega1-scanner-parser.md`) é para a direção antiga (SimC como fonte)
- A spec da linguagem SimC (`docs/linguagem-simc.md`) existe, mas agora SimC é o **target**, não a fonte
- Não existe documentação do subconjunto C++ permitido como entrada
- O Makefile e estrutura de testes existem mas precisam de ajustes

**A Entrega 1 (v1.0) deve construir:**
1. **Scanner completo** que reconhece o subconjunto C++ (lexemas, operadores, palavras-chave)
2. **Parser válido** que valida a sintaxe do subconjunto C++ (sem gerar código ainda)
3. **Fail-Fast** para construções de OO (`class`, `new`, `template`, etc.)
4. **Documentação** atualizada: SimC como target, subconjunto C++ como fonte

---

## Acceptance Criteria

- [ ] `make` compila sem erros (`-Wall -Wextra`, C99)
- [ ] `make test` verde — golden tests de lexer e parser passam 100%
- [ ] `./compilador < programa.cpp` valida sintaxe e reporta OK (exit 0) ou erros com linha/coluna (exit 1)
- [ ] `./compilador -t < programa.cpp` imprime dump de tokens (tipo, lexema, linha:coluna)
- [ ] Scanner ignora **qualquer diretiva de pré-processador** (linhas começando com `#`)
- [ ] Scanner rejeita palavras-chave de OO (`class`, `struct`, `new`, `template`, etc.) com erro claro
- [ ] Parser aceita: `int main() { ... }`, declarações, atribuições, `cout <<`, `cin >>`, `if/else`, `while`, `for`, blocos, expressões, funções
- [ ] Parser recusa: OO, ponteiros, referências, templates, sobrecarga, ternário, `do while`, `switch`
- [ ] `docs/linguagem-simc.md` atualizado indicando SimC como linguagem alvo
- [ ] `docs/subconjunto-cpp.md` criado documentando a linguagem fonte
- [ ] Issues antigas no GitHub foram limpas/reorganizadas
- [ ] Release v1.0 publicada com release notes

---

## Technical Approach

**Decision:** Reescrever lexer e parser para o novo domínio (C++ → SimC), aproveitando a infraestrutura base (Makefile, estrutura de testes).

**Origin:** colaborative (pivot decidido pela equipe)

**Rationale:**
- A infraestrutura Flex/Bison é preservada — mudamos apenas as regras de negócio
- O subconjunto C++ é restrito (procedimental, sem OO), minimizando conflitos shift/reduce
- Fail-Fast para OO desde o início evita gastar tempo implementando algo que será rejeitado

---

## Architecture Fit

```
programa C++ (subconjunto imperativo)
   │
   ▼
[Flex] lexer.l ──tokens──► [Bison] parser.y (validação sintática)
   │                           │
   └── dump -t (artefato v1)   ▼
                         validação sintática (aceita/rejeita + erros)
```

- **v1.0 (esta entrega):** Flex → Bison → validação. Sem geração de código.
- **v2.0 (próxima entrega):** Adicionar semantic actions para gerar código SimC.

---

## Implementation Plan

### O que PRECISA SER LIMPO / AJUSTADO (do estado atual)

| Item | Ação | Prioridade |
|------|------|------------|
| Issues antigas no GitHub | Deletar todas (eram para a direção antiga) | Alta |
| `docs/linguagem-simc.md` | Atualizar: SimC agora é TARGET, não fonte | Alta |
| `lexer/lexer.l` | **Reescrever** (atual ignora tudo) | Alta |
| `parser/parser.y` | **Reescrever** (atual não tem regras) | Alta |
| `src/main.c` | Adicionar modos `-t` e tratamento de erros | Média |
| `Makefile` | Verificar se precisa de `-lfl` (decisão D1 abaixo) | Média |
| `tests/run_tests.sh` | Ajustar para testar C++ (.cpp) em vez de SimC (.simc) | Média |
| `tests/` existentes | Remover ou reescrever (eram para direção antiga) | Média |
| `README.md` | Atualizar com nova direção e comandos | Baixa |
| `PROJECT_CONTEXT.md` | Atualizar §1 (Overview) e §3 (Architecture) | Baixa |

### O que CRIAR DE NOVO

| Item | Descrição | Prioridade |
|------|-----------|------------|
| `docs/subconjunto-cpp.md` | Especificação do subconjunto C++ permitido (tokens, BNF, exemplos) | Alta |
| `lexer/lexer.l` | Scanner completo para C++ subset | Alta |
| `parser/parser.y` | Parser com gramática C++ subset + precedência | Alta |
| `src/main.c` | CLI com modos `-t`, default, `-h` | Alta |
| `tests/lexer/*.cpp` + `.expected` | Golden tests do lexer | Alta |
| `tests/parser/aceita/*.cpp` + `.expected` | Golden tests de aceitação | Alta |
| `tests/parser/erros/*.cpp` + `.expected` | Golden tests de rejeição | Alta |
| `docs/semana01.md`...`semana05.md` | Relatórios semanais | Média |

---

### Decisões Técnicas (registradas neste plano)

| # | Decisão | Regra |
|---|---------|-------|
| D1 | **`-lfl` no Makefile** | Usar `%option noyywrap` no lexer → NÃO precisa de `-lfl`. Se não usar noyywrap, precisa linkar. Decisão: usar `%option noyywrap` (mais portável). |
| D2 | **Diretivas de pré-processador** | Ignorar **qualquer linha** que comece com `#` (não só `#include`/`#define`). Regra flex: `^[ \t]*#.*` → descartar. |
| D3 | **`struct` bloqueado** | `struct` entra na lista de fail-fast (é construção OO). |
| D4 | **`<<` no lexer** | `<<` é token separado (`LSHIFT`), usado APENAS comandos `cout`/`cin`. NÃO é operador de expressão. |
| D5 | **`endl` na gramática** | `endl` é token (`ENDL`), usado em `cout << ... << endl;`. |
| D6 | **Múltiplas variáveis** | `int a, b, c;` → suportado via `lista_identificadores` com vírgulas. |
| D7 | **Operador ternário** | `? :` NÃO suportado no subset. Documentar como "fora do subset". |
| D8 | **`do while` e `switch`** | NÃO suportados no subset. Documentar como "fora do subset". |
| D9 | **Código de erro fail-fast** | Erro de **compilação** (não semântico). Semântico implica análise de tipos, que é Entrega 2. |
| D10 | **Escopo de variáveis** | v1 NÃO valida declaração antes de uso (validação sintática pura). Documentar como limitação. |
| D11 | **Arrays** | NÃO suportados no subset v1. |
| D12 | **Ponteiros e referências** | NÃO suportados no subset. Bloqueados no parser. |

---

### Tasks

#### Bloco A — Gestão e Limpeza (Pré-desenvolvimento)

- [ ] **T1.1** [CLEAN] Deletar todas as issues abertas no GitHub usando `gh issue delete` — limpar quadro para nova direção
- [ ] **T1.2** [CLEAN] Deletar/ajustar releases antigas no GitHub
- [ ] **T1.3** [DOC] Atualizar `README.md` com nova direção (Transpilador C++ → SimC), comandos e exemplos
- [ ] **T1.4** [DOC] Atualizar `PROJECT_CONTEXT.md` §1 (Overview) e §3 (Architecture) para refletir o pivô

#### Bloco B — Documentação da Linguagem

- [ ] **T2.1** [DOC] Criar `docs/subconjunto-cpp.md` — especificação completa:
  - **Tabela de tokens C++** (identificadores, literais, operadores, delimitadores)
  - **Palavras-chave permitidas** vs **bloqueadas** (OO fail-fast — lista explícita)
  - **Gramática BNF completa** do subconjunto (expandida abaixo)
  - **Exemplos de programas válidos e inválidos**
  - **Regras de Escape** em strings (`\"`, `\\`, `\n`, `\t`)
  - **Comentários** (`//` e `/* */`)
  - **O que NÃO é suportado** (explicitar: ponteiros, referências, templates, structs, arrays, ternário, do-while, switch, sobrecarga, herança)
  - **Limitações conhecidas** (v1 não valida escopo/declaração antes de uso)
- [ ] **T2.2** [DOC] Atualizar `docs/linguagem-simc.md` — indicar que SimC é a linguagem ALVO (target)
  - Adicionar seção "Saída Esperada" com exemplos de transpilação
  - Manter spec de tokens e gramática (será usada na Entrega 2 para code gen)
- [ ] **T2.3** [DOC] Decidir e registrar **nome oficial** da linguagem (se "SimC" permanece ou muda)

#### Bloco C — Scanner (Lexer) para C++

- [ ] **T3.1** [ADD] `lexer/lexer.l` — **tokens base**:
  - `IDENT`: `[a-zA-Z_][a-zA-Z0-9_]*` (excluindo palavras reservadas)
  - `NUMBER_INT`: `[0-9]+`
  - `NUMBER_FLOAT`: `[0-9]+\.[0-9]+`
  - `STRING_LITERAL`: `"..."` com escape `\"`, `\\`, `\n`, `\t`
  - Operadores aritméticos: `PLUS` (+), `MINUS` (-), `STAR` (*), `SLASH` (/), `PERCENT` (%)
  - Operadores relacionais: `EQ` (==), `NEQ` (!=), `LT` (<), `GT` (>), `LE` (<=), `GE` (>=)
  - Operadores lógicos: `AND` (&&), `OR` (||), `NOT` (!)
  - Atribuição: `ASSIGN` (=)
  - Delimitadores: `LPAREN` ((), `RPAREN` ()), `LBRACE` ({), `RBRACE` (}), `LBRACKET` ([), `RBRACKET` (]), `SEMI` (;), `COMMA` (,)
  - Stream: `LSHIFT` (<<), `RSHIFT` (>>)
  - Palavras-chave permitidas: `int`, `float`, `string`, `void`, `if`, `else`, `while`, `for`, `return`, `cout`, `cin`, `endl`
  - Comentários: `//` e `/* */` (descartados, não geram tokens)

- [ ] **T3.2** [ADD] `lexer/lexer.l` — **Fail-Fast para OO** (D3):
  - Palavras bloqueadas (geram erro e continuam varredura):
    - OO: `class`, `struct`, `new`, `delete`, `template`, `virtual`, `override`, `pure`
    - Visibilidade: `public`, `private`, `protected`
    - Outros OO: `namespace`, `using`, `this`, `nullptr`, `true`, `false`
    - Herança: `friend`, `operator`
  - Mensagem: `"Erro de compilação: construção OO não suportada '%s' na linha %d, coluna %d"`
  - Continuar varredura (não parar no primeiro erro)

- [ ] **T3.3** [ADD] `lexer/lexer.l` — **ignorar pré-processador** (D2):
  - Regra: `^[ \t]*#.*` → descartar (ignora linhas com `#`)
  - Não gerar token, não reportar erro

- [ ] **T3.4** [ADD] `lexer/lexer.l` — **rastreamento de linha e coluna**:
  - Variável `coluna` resetada a cada `\n`
  - Expor `yylineno` e `coluna` ao parser
  - Formato de posição: `linha:coluna`

- [ ] **T3.5** [ADD] `lexer/lexer.l` — **tratamento de erros léxicos**:
  - Caractere inválido: `"Erro léxico: caractere inválido '%c' na linha %d, coluna %d"`
  - String não fechada: `"Erro léxico: string não fechada iniciada na linha %d"`
  - Comentário não fechado: `"Erro léxico: comentário não fechado iniciado na linha %d"`
  - Número malformado (`1.`): `"Erro léxico: número malformado '%s' na linha %d, coluna %d"`
  - Continuar varredura, exit 1 ao final se houve erros

#### Bloco D — Parser para C++

- [ ] **T4.1** [ADD] `parser/parser.y` — **declarações iniciais**:
  - `%union` com campos: `int intval`, `float floatval`, `char* strval`
  - Declaração de **todos** os tokens do lexer (inclusive os que a gramática não usa — D1 do plano antigo)
  - `%start programa`

- [ ] **T4.2** [ADD] `parser/parser.y` — **gramática completa do subconjunto C++**:

```bnf
programa         : lista_decl

lista_decl       : /* vazio */
                 | lista_decl decl

decl             : decl_funcao
                 | decl_var

decl_funcao      : tipo IDENT LPAREN lista_param RPAREN bloco

decl_var         : tipo lista_id SEMI
                 | tipo lista_id ASSIGN expressao SEMI

tipo             : INT | FLOAT | STRING | VOID

lista_id         : IDENT
                 | lista_id COMMA IDENT

lista_param      : /* vazio */
                 | parametro
                 | parametro COMMA lista_param

parametro        : tipo IDENT

bloco            : LBRACE lista_cmd RBRACE

lista_cmd        : /* vazio */
                 | lista_cmd comando

comando          : decl_var
                 | atribuicao
                 | cmd_if
                 | cmd_while
                 | cmd_for
                 | cmd_return
                 | cmd_cout
                 | cmd_cin
                 | bloco

atribuicao       : IDENT ASSIGN expressao SEMI

cmd_if           : IF LPAREN expressao RPAREN comando
                 | IF LPAREN expressao RPAREN comando ELSE comando

cmd_while        : WHILE LPAREN expressao RPAREN comando

cmd_for          : FOR LPAREN atribuicao expressao SEMI atribuicao RPAREN comando

cmd_return       : RETURN expressao SEMI
                 | RETURN SEMI

cmd_cout         : COUT LSHIFT expr_cout SEMI

expr_cout        : expressao
                 | expr_cout LSHIFT expressao
                 | expr_cout LSHIFT ENDL

cmd_cin          : CIN RSHIFT IDENT SEMI
                 | cmd_cin RSHIFT IDENT

expressao        : expr_or

expr_or          : expr_and
                 | expr_or OR expr_and

expr_and         : expr_not
                 | expr_and AND expr_not

expr_not         : NOT expr_not
                 | expr_comp

expr_comp        : expr_add
                 | expr_comp EQ expr_add
                 | expr_comp NEQ expr_add
                 | expr_comp LT expr_add
                 | expr_comp GT expr_add
                 | expr_comp LE expr_add
                 | expr_comp GE expr_add

expr_add         : expr_mul
                 | expr_add PLUS expr_mul
                 | expr_add MINUS expr_mul

expr_mul         : expr_unario
                 | expr_mul STAR expr_unario
                 | expr_mul SLASH expr_unario
                 | expr_mul PERCENT expr_unario

expr_unario      : MINUS expr_unario
                 | primario

primario         : IDENT
                 | NUMBER_INT
                 | NUMBER_FLOAT
                 | STRING_LITERAL
                 | LPAREN expressao RPAREN
```

- [ ] **T4.3** [ADD] `parser/parser.y` — **cadeia de precedência**:
  ```
  %left OR
  %left AND
  %left EQ NEQ
  %left LT GT LE GE
  %left PLUS MINUS
  %left STAR SLASH PERCENT
  %right NOT UMINUS
  ```
  - `UMINUS` é pseudo-token para unário `-` (não entra na lista `%token`)

- [ ] **T4.4** [ADD] `parser/parser.y` — `%expect 1` para dangling else

- [ ] **T4.5** [ADD] `parser/parser.y` — **recuperação de erros**:
  - Regra `error` em nível de comando: `comando : error SEMI`
  - `yyerrok` + `yyclearin` para continuar após erro
  - Cap de 10 erros por arquivo (variável global incrementada)

- [ ] **T4.6** [ADD] `parser/parser.y` — **yyerror contextual**:
  - Formato: `"erro: %s na linha %d, coluna %d"`
  - Incluir token inesperado quando disponível
  - Usar `yylloc` para posição

#### Bloco E — Main e CLI

- [ ] **T5.1** [ADD] `src/main.c` — **modo `-t/--tokens`**:
  - Chamar `yylex()` em loop, imprimir `linha:coluna TOKEN "lexema"` por linha
  - Imprimir `linha:coluna EOF` ao final
  - Exit 0 se sem erros léxicos, exit 1 se houve

- [ ] **T5.2** [ADD] `src/main.c` — **modo default**:
  - Chamar `yyparse()`
  - Se sucesso: stdout `OK`, exit 0
  - Se erro: stderr mensagens de erro, exit 1

- [ ] **T5.3** [ADD] `src/main.c` — **modo `-h/--help`**:
  - Imprimir mensagem de uso com exemplos
  - Exit 0

- [ ] **T5.4** [ADD] `src/main.c` — **exit codes**:
  - 0: sucesso
  - 1: erros léxicos ou sintáticos
  - 2: uso inválido da CLI

- [ ] **T5.5** [ADD] `src/main.c` — **cap de erros**:
  - Variável global `erro_count`
  - Incrementar a cada erro léxico/sintático
  - Parar reportar após 10 erros (continuar compilando)

#### Bloco F — Testes

- [ ] **T6.1** [TEST] Ajustar `tests/run_tests.sh`:
  - Processar arquivos `.cpp` (não `.simc`)
  - Adicionar suporte a testes de lexer (modo `-t`) e parser (default)

- [ ] **T6.2** [TEST] `tests/lexer/` — **positivos**:
  - Tokens simples (identificadores, números int/float, operadores)
  - Literais (string com escape `\"`, `\n`, `\\`)
  - Palavras-chave (`int`, `float`, `if`, `while`, `for`, `cout`, `cin`, `return`, `void`, `endl`)
  - Comentários `//` e `/* */` entre tokens
  - Diretivas `#include`/`#define` ignoradas

- [ ] **T6.3** [TEST] `tests/lexer/` — **erros**:
  - Palavra-chave OO (`class`, `new`, `template`, `struct`) → mensagem de erro
  - Caractere inválido (`$`, `@`)
  - String aberta
  - Comentário `/*` aberto

- [ ] **T6.4** [TEST] `tests/parser/aceita/` — **programas válidos**:
  - `int main() { return 0; }`
  - Declarações: `int a;`, `float b = 3.14;`, `int x, y, z;`
  - Atribuições: `a = 10;`, `a = b + c * 2;`
  - `cout << "hello";`, `cout << x << y;`, `cout << x << endl;`
  - `cin >> x;`, `cin >> a >> b;`
  - `if/else` simples e aninhado
  - `while (a > 0) { a = a - 1; }`
  - `for (int i = 0; i < 10; i = i + 1) { ... }`
  - Blocos aninhados
  - Expressões com precedência: `a + b * 2`, `(a + b) * 2`, `-a * b`, `!a`
  - Funções: `int soma(int a, int b) { return a + b; }`
  - Múltiplas funções

- [ ] **T6.5** [TEST] `tests/parser/erros/` — **programas inválidos**:
  - Falta `;`: `int a = 10`
  - Falta `)`: `if (a > 0 { ... }`
  - `else` solto (sem `if`)
  - `if` sem parênteses: `if a > 0 { ... }`
  - Uso de `class`, `new`, `template` (fail-fast)
  - Ponteiros: `int *p;`
  - Referências: `int &r = a;`
  - Arrays: `int arr[10];`
  - Ternário: `a = b ? c : d;`
  - `do while`, `switch`
  - Arquivo com 3 erros → 3 mensagens (recuperação)

- [ ] **T6.6** [TEST] `tests/lexer/` — **fumaça**:
  - Arquivo vazio
  - Só comentários
  - Programa com `//` dentro de string (não vira comentário)
  - `cout << "// não é comentário";`

#### Bloco G — Integração e Release

- [ ] **T7.1** [ADD] `make test` unificado (lexer + parser) verde
- [ ] **T7.2** [TEST] Casos de borda finais: vazio, só comentário, demo completo
- [ ] **T7.3** [DOC] Atualizar `PROJECT_CONTEXT.md` com lições aprendidas
- [ ] **T7.4** [DOC] Relatórios semanais (`docs/semana01.md`...`semana05.md`)
- [ ] **T7.5** [DOC] Script de demonstração da P1
- [ ] **T7.6** [REL] Criar milestone **P1** no GitHub
- [ ] **T7.7** [REL] Publicar release **v1.0** com release notes
- [ ] **T7.8** [REL] Tag `v1.0`

---

### Implementation Order

1. **Bloco A (Gestão):** Limpar issues, atualizar docs de orientação — prerequisite para tudo
2. **Bloco B (Documentação):** Definir spec do subconjunto C++ antes de implementar
3. **Bloco C (Scanner):** Lexer completo — o parser depende dos tokens
4. **Bloco D (Parser):** Gramática C++ subset com precedência e recuperação de erros
5. **Bloco E (CLI):** Integrar lexer + parser no main.c com modos de operação
6. **Bloco F (Testes):** Golden tests completos para validação
7. **Bloco G (Integração):** Release final e preparação P1

> **Regra de ouro:** cada tarefa termina com `make` + `make test` verdes antes do commit. Commits pequenos e frequentes (padrão `[ADD]/[FIX]/[DOCS]/[REF]`).

---

### Files to Create/Modify

| File | Action | Purpose |
|------|--------|---------|
| `docs/subconjunto-cpp.md` | CREATE | Especificação da linguagem fonte (C++ subset) |
| `docs/linguagem-simc.md` | MODIFY | Indicar SimC como target, manter spec para code gen futuro |
| `docs/semana01.md`...`semana05.md` | CREATE | Relatórios semanais |
| `lexer/lexer.l` | REWRITE | Scanner completo para C++ subset |
| `parser/parser.y` | REWRITE | Parser com gramática C++ subset |
| `src/main.c` | REWRITE | CLI completa (-t, default, -h) |
| `Makefile` | VERIFY | Verificar se `%option noyywrap` elimina necessidade de `-lfl` |
| `tests/run_tests.sh` | MODIFY | Processar .cpp em vez de .simc |
| `tests/lexer/*.cpp` + `.expected` | CREATE | Golden tests do lexer |
| `tests/parser/aceita/*.cpp` + `.expected` | CREATE | Golden tests de aceitação |
| `tests/parser/erros/*.cpp` + `.expected` | CREATE | Golden tests de rejeição |
| `.gitignore` | VERIFY | Garantir que exclui artefatos gerados |
| `README.md` | MODIFY | Nova direção, comandos, exemplos |
| `PROJECT_CONTEXT.md` | MODIFY | Atualizar Overview e Architecture |

---

### CLI Contract (v1.0)

| Invocação | Saída | Exit |
|-----------|-------|------|
| `./compilador < prog.cpp` | stdout: `OK` | 0 |
| `./compilador < prog.cpp` (com erros) | stderr: `erro: <mensagem>` (máx. 10) | 1 |
| `./compilador -t < prog.cpp` | stdout: dump tokens `linha:coluna TOKEN "lexema"` | 0/1 |
| `./compilador -h` | stdout: uso | 0 |
| flag inválida | stderr: mensagem de uso | 2 |

---

### Limitações Conhecidas (v1 — documentar em docs/subconjunto-cpp.md)

| Limitação | Motivo | Será resolvido em |
|-----------|--------|-------------------|
| Não valida escopo de variáveis | Validador sintático puro | Entrega 2 (análise semântica) |
| Não valida declaração antes de uso | Validador sintático puro | Entrega 2 (análise semântica) |
| Não valida tipos em operações | Validador sintático puro | Entrega 2 (análise semântica) |
| Não gera código SimC | Escopo da v1 | Entrega 2 (code gen) |

---

## Testing Strategy

- **Framework:** Golden tests — `tests/**/*.cpp` (entrada) + `.expected` (saída)
- **Lexer (modo `-t`):** `.expected` = dump de tokens esperado
- **Parser aceita:** `.expected` = `OK` (stdout) + exit 0
- **Parser erros:** `.expected` = mensagens de erro exatas (stderr)
- **Verificação:** `make` e `make test` verdes antes da release

---

## Risks and Considerations

| Risco | Impacto | Mitigação |
|-------|---------|-----------|
| Gramática C++ ambígua | Conflitos shift/reduce | Subconjunto restrito procedural, `%expect 1` para dangling else |
| OO fail-fast incompleto | Construções indesejadas passam | Lista abrangente de 20+ palavras bloqueadas + testes |
| CRLF vs LF | Regras flex quebram | Configurar `core.autocrlf` + `.gitattributes` |
| Escopo creep | Atraso na P1 | DoD claro: Scanner + Parser validando, SEM code gen |
| `<<` ambíguo (stream vs bitwise) | Conflito no parser | `<<` é token separado (`LSHIFT`), usado APENAS em comandos `cout`/`cin` |
| `endl` não tratado | Erros em `cout << x << endl` | Token `ENDL` + regra `expr_cout LSHIFT ENDL` |

---

## Dependencies

- **External:** Flex, Bison, GCC (WSL Ubuntu — `apt install flex bison build-essential`)
- **Internal:** Nenhuma — repositório limpo, tudo é criado nesta entrega

---

## Evidence (filled by tester/reviewer)

- **Test Log:** `<path — filled after testing>`
- **Coverage:** `<path — filled after testing>`
- **Security Scan:** N/A (acadêmico)
- **Review Verdict:** `<APPROVED|CHANGES_REQUESTED — filled after review>`

---

## DoD Final (Definition of Done — v1.0)

- [ ] `make` e `make test` verdes
- [ ] 100% dos golden tests passando (lexer + parser)
- [ ] Scanner rejeita OO com mensagem clara
- [ ] Parser valida subconjunto C++ completo
- [ ] Demo P1 executável ao vivo
- [ ] Release v1.0 publicada + milestone P1 no GitHub
- [ ] Documentação atualizada (SimC target, C++ fonte)

---

_Created by @plan-maker_
_Last updated: 27/08/2026_
_Revision: Correções de gramática, ambiguidade, e decisões técnicas (D1-D12)_
