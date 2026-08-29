# Project Context — Compilador C++ → SimC

> **Last Updated:** 2026-08-28 | **Maintained By:** AI Agent Team (@tech-lead)
> **Architecture:** Compiler Pipeline (Frontend C++ Subset → Target SimC)

---

## 1. Project Overview

O projeto da disciplina de Compiladores 1 consiste em um **Compilador**. A ferramenta lê um código-fonte restrito escrito em C++ (subconjunto imperativo, sem orientação a objetos) e gera código equivalente na linguagem acadêmica **SimC** (linguagem simplificada com loops, condicionais, int e float). O objetivo é demonstrar o pipeline de compilação contornando a complexidade de abstrações OO.

### 1.1 Justificativa

O projeto implementa um compilador source-to-source, que segue o pipeline clássico de compiladores conforme definido na literatura (Aho, Sethi, Ullman — *Compiladores: Princípios, Técnicas e Ferramentas*). As etapas são: Análise Léxica (Flex) → Análise Sintática (Bison) → Análise Semântica → Geração de Código.

---

## 2. Technology Stack — Dev Commands

| Layer | Technology |
|-------|-----------|
| Linguagem | C |
| Lexer Generator | Flex (2.6.4+) |
| Parser Generator | Bison (3.8.2+) |
| Compilador | GCC (13.3.0+) |
| Build Tool | Make |

**Dev Commands:**

| Command | Description |
|---------|------------|
| `make` | Compila o projeto e gera o binário do compilador |
| `make test` | Roda os golden tests na pasta `tests/` |
| `make clean` | Limpa os artefatos de build gerados (lex.yy.c, parser.tab.c, etc) |

---

## 3. Architecture

**Pattern:** Compiler Pipeline (Single Pass / Syntax-Directed Translation ou AST)


Um compilador também se divide:

```
┌─────────────────────────────────────────────────────────────────┐
│                   FRONTEND - Lexer + Parser                     │
│                  (entende o código-fonte)                       │
│                                                                 │
│   C++ (subconjunto)                                             │
│        ↓                                                        │
│      Flex (lexer.l)        → identifica tokens                  │
│        ↓                                                        │
│     Tokens                                                      │
│        ↓                                                        │
│     Bison (parser.y)       → verifica estrutura                 │
│        ↓                                                        │
│       AST                                                       │
│        ↓                                                        │
│ Análise semântica          → verifica significado               │
└─────────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────────┐
│                        BACKEND                                  │
│                  (gera o código de saída)                       │
│                                                                 │
│  Geração de SimC             → emite código                     │
│        ↓                                                        │
│      SimC                                                       │
└─────────────────────────────────────────────────────────────────┘
```

**Frontend:** Responsável por **entender** o código-fonte. Lê o código C++, quebra em tokens, valida a estrutura e verifica o significado. É como o "tradutor" que compreende o que o programador escreveu.

**Backend:** Responsável por **gerar** o código de saída. Pega a representação interna (AST) e a traduz para a linguagem-alvo (SimC). É como o "escritor" que produz o resultado final.

### 3.1 Responsabilidades

| Camada | Responsabilidade |
|--------|-----------------|
| **Flex** | Identifica os tokens (incluindo `class → CLASS`, `struct → STRUCT`) |
| **Bison** | Verifica a estrutura do programa (gramática) |
| **Análise semântica** | Verifica se o programa faz sentido e rejeita construções não suportadas |

### 3.2 Notas

- O Lexer reconhece palavras-chave de OO (`class`, `struct`, etc.) como tokens, mas o Parser/semântica as rejeita.
- Ignora diretivas de pré-processador (`#include`, `#define`).
- Fail-Fast para construções de OO (`class`, `new`, `template`).

---

## 4. Data Model

- TBD: A definir se o backend gerará o código durante o parsing (Syntax-Directed) ou se montaremos uma AST para caminhamento.

---

## 5. Coding Standards & Conventions

- **Nomenclatura (C):** `snake_case` para variáveis e funções.
- **Prefixos de Commit:** `[ADD]`, `[FIX]`, `[DOCS]`, `[REF]`.
- **Nomenclatura de Branch:** `[ÁREA]-[descrição-curta]` (ex: `LEX-subconjunto-cpp`).

---

## 6. Testing Strategy

- **Framework:** Scripts Bash / Golden Tests (`make test`).
- **Estratégia:** Arquivos `.cpp` na pasta de testes devem gerar arquivos `.simc` que são comparados com a saída esperada (golden files).

---

## 10. Common Pitfalls & Lessons Learned

- **2026-08-27:** Pivô de Arquitetura. Abandonamos a compilação *de* SimC *para* C++ devido à complexidade insustentável de mapear estruturas para Orientação a Objetos em Flex/Bison. O projeto inverteu: C++ Imperativo (Subconjunto) vira a linguagem fonte, e SimC vira o target, reduzindo drasticamente o risco técnico.
- **2026-08-28:** Revisão de terminologia e documentação. Clarificação das responsabilidades entre Flex, Bison e análise semântica. Definição do subconjunto C++ suportado em `docs/subconjunto-cpp.md`.

---
*Created by @ItaloSamP on 2026-08-27*
*Updated by @ArthurDevWorks on 2026-08-28*
