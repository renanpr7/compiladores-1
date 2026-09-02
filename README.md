# Compilador C++ → C

Projeto da disciplina **Compiladores 1** (FGA0003 — Engenharia de Software, UnB), em Flex e Bison.

O compilador lê um subconjunto imperativo de C++, sem orientação a objetos, e gera C equivalente.
A saída é C de verdade: compila com `gcc` e executa.

```
C++ (subconjunto)  →  Flex  →  Bison  →  AST  →  C
```

```bash
./compilador < programa.cpp > programa.c
gcc programa.c -o programa && ./programa
```

O subconjunto inclui de propósito sobrecarga de funções, referências e parâmetros com valor padrão.
Nenhuma tem equivalente direto em C, então o compilador traduz em vez de copiar. O que é aceito e
como cada construção é traduzida está em [docs/linguagem.md](docs/linguagem.md).

> **Estado atual:** Sprint 1. O lexer e o parser são esqueletos — não reconhecem tokens nem validam
> gramática. O andamento está nas [issues](https://github.com/renanpr7/compiladores-1/issues).

## Ambiente

Testado em WSL/Ubuntu com Flex 2.6.4, Bison 3.8.2, GCC 13.3.0 e Make 4.3.

```bash
sudo apt update
sudo apt install flex bison build-essential
```

## Comandos

```bash
make          # gera ./compilador
make test     # roda os golden tests
make clean    # remove os artefatos gerados
```

O compilador lê da entrada padrão e escreve na saída padrão:

```bash
./compilador -t < programa.cpp    # dump de tokens
./compilador -a < programa.cpp    # dump da AST
./compilador -i < programa.cpp    # dump do código de três endereços
./compilador    < programa.cpp    # gera o C
```

## Testes

Cada caso é um `.cpp` com um `.expected` ao lado. O runner compara a saída e o código de retorno:
casos dentro de uma pasta `erros/` devem terminar com 1, os demais com 0.

| Pasta | O que verifica | Como |
|-------|----------------|------|
| `lexer/` | os tokens reconhecidos | `./compilador -t` |
| `parser/aceita/` | programas válidos são aceitos | `./compilador` |
| `parser/erros/` | programas inválidos são recusados | `./compilador` |
| `ast/` | a árvore montada | `./compilador -a` |
| `semantica/` | escopo, tipos, declaração antes do uso | `./compilador` |
| `codegen/` | o C gerado | `./compilador` |
| `execucao/` | o C gerado compila e roda | `gcc` + execução |

`execucao/` é a suíte que prova o compilador de ponta a ponta: pega o C emitido, compila com `gcc`
e compara o que o programa imprime.

Toda issue entrega os próprios testes. `make test` verde é critério de aceite.

## Estrutura

```
src/
  lexer.l       regras do Flex: texto vira tokens
  parser.y      gramática do Bison: valida a estrutura e monta a AST
  ast.c ast.h   a árvore e as funções que a percorrem
  main.c        interface de linha de comando
tests/
  run_tests.sh  executa os golden tests de todas as fases
docs/
  linguagem.md  o C++ que aceitamos e o C que emitimos
Makefile
```

`lex.yy.c`, `parser.tab.c` e `parser.tab.h` são gerados pelo Flex e pelo Bison durante o build e
não são versionados.

## Decisões técnicas

| # | Decisão | Motivo |
|---|---------|--------|
| 1 | O subconjunto exclui OO, mas inclui o que C++ tem e C não | Classes e herança ficam de fora. Sobrecarga, referências e parâmetros padrão entram: são onde o compilador traduz de verdade. |
| 2 | Fora do subconjunto também fica o que C++ e C escrevem igual | `struct`, `switch` e arrays de duas dimensões seriam cópia da entrada. O esforço rende mais nas construções que exigem tradução. |
| 3 | A representação interna é uma AST | Semântica, código intermediário e otimização precisam de uma estrutura para percorrer. |
| 4 | O C é emitido a partir da AST | O gerador percorre a árvore direto, e o *constant folding* também roda sobre ela. O TAC é uma fase à parte, com saída própria em `-i`. |
| 5 | `<<` e `>>` só valem em `cout` e `cin` | Tokens próprios, nunca operadores de expressão. Evita ambiguidade na gramática. |
| 6 | O lexer não recusa construções da linguagem | Flex identifica tokens, Bison verifica a estrutura. `class` vira o token `CLASS` e o parser recusa com mensagem clara. Erro léxico — caractere inválido, string não fechada — continua sendo do lexer. |
| 7 | O lexer descarta linhas `#`, `using namespace std;` e o prefixo `std::` | O compilador emite o `#include` que a saída precisa. Aceitar as duas formas de qualificar mantém a entrada como C++ que compila no `g++`. |
| 8 | Sobrecarga vira renomeação por tipo | C não aceita dois nomes iguais. `maior(int,int)` vira `maior_ii`. Escolher a versão certa exige a tabela de símbolos. |
| 9 | Referências viram ponteiros | `int& x` vira `int* x`, e cada uso vira `*x`. |
| 10 | `%option noyywrap` no lexer | Dispensa linkar `-lfl`. |

## Equipe

| Papel | Quem |
|-------|------|
| Líder — envia os formulários P1 e P2 | *a definir* |
| Membros | ArthurDevWorks, Brun00000000, ItaloSamP, LeonardoLopesJr, renanpr7 |
| Número da equipe | 6 |

O número decide a data da apresentação, e a ordem se inverte entre P1 e P2:

| | Equipes 1–8 | Equipes 9–16 |
|---|---|---|
| P1 | 28/09 | 30/09 |
| P2 | 11/11 | 09/11 |

## Planejamento

O trabalho é acompanhado pelas [issues](https://github.com/renanpr7/compiladores-1/issues),
organizadas em [milestones](https://github.com/renanpr7/compiladores-1/milestones) por sprint.
Criamos issues de uma sprint por vez.

| Sprint | Período | Foco | Marco |
|--------|---------|------|-------|
| 1 | até 07/09 | Analisador léxico | |
| 2 | 08/09 a 21/09 | Analisador sintático e CLI | Formulário P1 até 23/09 · apresentação de 5 min em 28 ou 30/09 |
| 3 | 22/09 a 11/10 | AST | |
| 4 | 12/10 a 02/11 | Tabela de símbolos, escopo, tipos e sobrecarga · primeira geração de C de ponta a ponta | Formulário P2 até 04/11 · apresentação em 09 ou 11/11 |
| 5 | 05/11 a 15/11 | TAC, *constant folding* e geração de C completa | Entrega do compilador pelo Teams · repositório congelado em 15/11 |
| 6 | 16/11 a 02/12 | Ajustes e manual de uso | Entrevista final em 30/11 ou 02/12 |

## Convenções

**Branches:** `[ÁREA]-descricao-curta`, onde a área é `LEX`, `SIN`, `SEM`, `GER` ou `DOC`.
Exemplo: `SIN-precedencia-operadores`. Uma branch por tarefa; depois do merge ela morre.

**Commits:** `[ADD]`, `[FIX]`, `[DOCS]` ou `[REF]` no início.
Exemplo: `[ADD] reconhecimento de tokens numericos`.

```bash
git checkout main && git pull origin main
git checkout -b LEX-minha-tarefa
make && make test
git commit -m "[ADD] descricao curta"
git push origin LEX-minha-tarefa
```

Merge em `main` sempre por Pull Request.

## Referências

[Repositório da disciplina](https://github.com/sergioaafreitas/COMP1), com exemplos de Flex e Bison
por semana: `04` e `05` para o parser, `06` para AST e tabela de símbolos, `08` para o TAC, `09`
para o *constant folding*.
