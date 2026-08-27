# Project Context — Transpilador SimC

> **Last Updated:** 2026-08-27 | **Maintained By:** AI Agent Team (@tech-lead)
> **Architecture:** Compiler Pipeline (Frontend C++ Subset -> Target SimC)

---

## 1. Project Overview

O projeto da disciplina de Compiladores 1 consiste em um **Transpilador**. A ferramenta lê um código-fonte restrito escrito em C++ (subconjunto imperativo, sem orientação a objetos) e gera código equivalente na linguagem acadêmica **SimC** (linguagem simplificada com loops, condicionais, int e float). O objetivo é demonstrar o pipeline de compilação contornando a complexidade de abstrações OO.

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

1. **Frontend (Lexer + Parser):** Processa o subconjunto C++. Ignora diretivas de pré-processador (`#include`). Aborta imediatamente via *Fail-Fast* se encontrar palavras-chave de Orientação a Objetos (`class`, `new`, `template`).
2. **Backend (Code Generator):** Emite strings de código válido para SimC. Por exemplo, mapeia `std::cout << x;` para `print(x);`.

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

---
*Created by @tech-lead on 2026-08-27*
