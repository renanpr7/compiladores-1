# Compilador C++ → C

Projeto da disciplina **Compiladores 1** (FGA0003 — Engenharia de Software, UnB), construído com
Flex e Bison.

O compilador lê um **subconjunto imperativo de C++** (sem orientação a objetos) e gera **código C**
equivalente. A saída é C de verdade: compila com `gcc` e executa.

```
C++ (subconjunto)  →  Flex  →  Bison  →  AST  →  código de três endereços  →  C
```

```bash
./compilador < programa.cpp > programa.c
gcc programa.c -o programa && ./programa
```

O que muda na tradução são poucas construções — `cout`, `cin`, `string` e os `#include`. O resto do
subconjunto já é C válido e passa direto. O trabalho está no pipeline, não na distância entre as
duas linguagens. Detalhes em [docs/linguagem.md](docs/linguagem.md).

> **Estado atual:** Entrega 1 em andamento. O lexer e o parser ainda são esqueletos — não
> reconhecem tokens nem validam gramática. Acompanhe pelas
> [issues](https://github.com/renanpr7/compiladores-1/issues).

## Ambiente

Testado em WSL/Ubuntu com Flex 2.6.4, Bison 3.8.2, GCC 13.3.0 e Make 4.3.

```bash
sudo apt update
sudo apt install flex bison build-essential
```

## Comandos

```bash
make          # compila e gera ./compilador
make test     # roda os golden tests
make clean    # remove os artefatos gerados
```

Para rodar um teste isolado:

```bash
./compilador -t < tests/lexer/hello.cpp     # dump de tokens
./compilador    < tests/lexer/hello.cpp     # validação sintática
./compilador -a < tests/lexer/hello.cpp     # dump da AST
```

## Testes

Cada caso é um arquivo `.cpp` com um `.expected` ao lado. O runner compara a saída e confere o
código de retorno — casos dentro de uma pasta `erros/` devem terminar com 1, os demais com 0.

Há uma suíte por fase do compilador:

| Pasta | O que verifica | Como |
|-------|----------------|------|
| `lexer/` | os tokens reconhecidos | `./compilador -t` |
| `parser/aceita/` | programas válidos são aceitos | `./compilador` |
| `parser/erros/` | programas inválidos são recusados | `./compilador` |
| `ast/` | a árvore montada | `./compilador -a` |
| `semantica/` | escopo, tipos, declaração antes do uso | `./compilador` |
| `codegen/` | o C gerado | `./compilador` |
| `execucao/` | o C gerado **compila com `gcc` e roda** | `gcc` + execução |

A suíte `execucao/` é a que prova que o compilador funciona de ponta a ponta: pega o C emitido,
compila e compara o que o programa imprime.

Toda issue entrega os próprios testes. `make test` verde é critério de aceite de qualquer tarefa.

## Estrutura

```
src/
  lexer.l       regras do Flex: transforma texto em tokens
  parser.y      gramática do Bison: valida a estrutura e monta a AST
  main.c        entrada do programa e interface de linha de comando
tests/
  run_tests.sh  executa os golden tests de todas as fases
  lexer/        dump de tokens
  parser/       aceita/ e erros/
  ast/          dump da árvore
  semantica/    erros de escopo, tipo e declaração
  codegen/      o C gerado
  execucao/     o C gerado compila com gcc e roda
docs/
  linguagem.md  o C++ que aceitamos e o C que emitimos
Makefile
```

Os arquivos que o Flex e o Bison geram (`lex.yy.c`, `parser.tab.c`, `parser.tab.h`) aparecem em
`src/` durante o build e não são versionados.

## Decisões técnicas

| # | Decisão | Motivo |
|---|---------|--------|
| 1 | O subconjunto exclui orientação a objetos, mas inclui o que C++ tem e C não | Classes e herança ficam de fora. Sobrecarga, referências e parâmetros padrão entram: são onde o compilador traduz de verdade, em vez de copiar a entrada. |
| 2 | A representação interna é uma AST | As fases de semântica, código intermediário e otimização precisam de uma estrutura para percorrer. |
| 3 | Entre a AST e a saída existe código de três endereços | É onde a otimização acontece; emitir C direto da AST não deixaria nada para otimizar. |
| 4 | `<<` e `>>` só valem em `cout`/`cin` | Tratados como tokens próprios, nunca como operadores de expressão — evita ambiguidade na gramática. |
| 5 | O Lexer não rejeita construções de OO | Flex identifica tokens, Bison verifica a estrutura. `class` vira o token `CLASS` e o parser recusa. |
| 6 | Linhas iniciadas por `#` são descartadas | O compilador emite o `#include <stdio.h>` de que a saída precisa. |
| 7 | Sobrecarga vira renomeação por tipo (*name mangling*) | C não aceita duas funções com o mesmo nome. `maior(int,int)` vira `maior_ii`. Escolher a versão certa exige a tabela de símbolos. |
| 8 | Referências viram ponteiros | C não tem referências. `int& x` vira `int* x` e cada uso vira `*x`. |
| 9 | `struct` é suportado, sem métodos | É registro, não OO, e obriga a tabela de símbolos a guardar os campos de cada tipo. |
| 10 | `%option noyywrap` no lexer | Dispensa linkar `-lfl`, mais portável entre distribuições. |

## Planejamento

O trabalho é acompanhado pelas [issues](https://github.com/renanpr7/compiladores-1/issues),
organizadas em [milestones](https://github.com/renanpr7/compiladores-1/milestones) por sprint.
Criamos issues de uma sprint por vez, em vez de planejar o semestre inteiro de antemão.

| Sprint | Período | Foco |
|--------|---------|------|
| 1 | até 07/09 | Analisador léxico |
| 2 | 08/09 a 21/09 | Analisador sintático — **P1 em 28-30/09** |
| 3 | 22/09 a 11/10 | AST e tabela de símbolos |
| 4 | 12/10 a 02/11 | Tabela de símbolos, escopo, tipos e resolução de sobrecarga |
| 5 | 05/11 a 15/11 | Código de três endereços, otimização e geração de C — **P2 em 09-11/11** |
| 6 | 16/11 a 02/12 | Integração e entrevista final |

## Convenções

**Branches:** `[ÁREA]-descricao-curta`, onde a área é `LEX` (léxica), `SIN` (sintática),
`SEM` (semântica), `GER` (geração) ou `DOC`. Exemplo: `SIN-precedencia-operadores`.
Uma branch por tarefa; depois do merge ela morre.

**Commits:** comece com `[ADD]`, `[FIX]`, `[DOCS]` ou `[REF]`.
Exemplo: `[ADD] reconhecimento de tokens numericos`.

**Fluxo:**

```bash
git checkout main && git pull origin main
git checkout -b LEX-minha-tarefa
# edite, e antes de commitar:
make && make test
git commit -m "[ADD] descricao curta"
git push origin LEX-minha-tarefa
# abra o Pull Request no GitHub
```
