# Feature Requirement — Compilador C++ para SimC

> **Status:** Finalizado | **Data:** 2026-08-27 | **Projeto:** Compilador C++ → SimC
> **Tipo:** Refatoração de Arquitetura (Pivô)
> Documento de requisito (o que / por que). NAO contem especificacao tecnica — o COMO
> (arquitetura, contrato de API, modelo de dados) e do @tech-lead / orchestrator.
> Campos `(obrigatorio)` sao o minimo pro orchestrator/plan-maker planejar sem chutar.
> Vazio desconhecido = `> _A definir_`.

---

## 1. Identificacao
- **Nome:** Pivô para Compilador C++ (Subconjunto) -> SimC
- **Tipo:** refactor / feature
- **Prioridade:** alta
- **Issue/link relacionado:** _A definir_ (As issues antigas serão substituídas)

## 2. Problema & Objetivo *(obrigatorio)*
**Problema:** A compilação de SimC para C++ introduziu a complexidade indesejada de gerar código Orientado a Objetos (OO).
**Objetivo:** Inverter o fluxo. O usuário fornecerá um código fonte restrito em C++ (imperativo) que será compilado/traduzido para o SimC, preservando a simplicidade estruturada do projeto.
**User Story (JTBD):** Quando o estudante/professor quiser demonstrar o compilador, ele quer usar a sintaxe familiar básica de C++ como entrada para gerar o código final SimC, evitando totalmente a complexidade de linguagens OO.

## 3. Contexto & Motivacao
A disciplina de Compiladores exige a construção de um pipeline (Scanner/Parser). Tentar abraçar a OO e a semântica complexa do C++ no backend é muito arriscado. Limitar a entrada a um subconjunto imperativo do C++ e gerar código em uma linguagem enxuta (SimC) entrega valor pedagógico e mantém o prazo viável.

## 4. Fluxo Esperado (Happy Path) *(obrigatorio)*
1. Usuário escreve código `.cpp` básico (apenas `int main()`, loops, ifs, variáveis base e `cout <<`).
2. O Lexer processa os tokens (ignorando `#include`).
3. O Parser analisa a sintaxe contra as regras da gramática imperativa restrita.
4. O Compilador gera um arquivo `.simc` válido e estruturado.

## 5. Criterios de Aceite *(obrigatorio)*
- [ ] Quando o compilador ler `int main() { ... }`, entao ele deve ser capaz de compilar corretamente para SimC.
- [ ] Quando ler `cout << var;`, entao ele deve gerar a string `print(var);`.
- [ ] Quando ler `cin >> var;`, entao ele deve gerar a string `input(var);`.
- [ ] Quando ler `cout << x << y;`, entao ele deve gerar a string `print(x, y);`.
- [ ] Quando ler uma diretiva como `#include <iostream>`, entao o Lexer deve ignorar a linha e prosseguir.
- [ ] Quando ler qualquer sintaxe OO restrita (ex: `class`, `new`, `template`), entao a compilação deve falhar com a mensagem: "Erro: Funcionalidade de Orientação a Objetos não suportada".

## 6. Escopo (MoSCoW)
| Prioridade | O que e | Justificativa |
|-----------|---------|---------------|
| **Must** (v1) | Parsing 1:1 de ints, floats, loops e ifs | Construções procedurais vitais |
| **Must** (v1) | Regra gramatical para `int main()` | Exigência mínima para o arquivo fonte ser C++ |
| **Must** (v1) | *Fail-fast* para palavras OO no Lexer | Proteger a gramática do Parser |
| **Must** (v1) | Tradução do `cout` de via única (simples) | Saída de dados |
| **Must** (v1) | Tradução do `cin` de via única (simples) | Entrada de dados |
| **Should** (v1) | Arrays básicos (declaração + acesso por índice) | Armazenamento de dados |
| **Won't** (agora) | Compilar bibliotecas, classes, herança | Escopo fora da realidade da disciplina |

## 7. Regras de Negocio *(obrigatorio se houver logica)*
- O Lexer identifica os tokens (incluindo `class → CLASS`, `struct → STRUCT`).
- O Bison valida a estrutura do programa (gramática).
- A análise semântica verifica se o programa faz sentido e rejeita construções não suportadas.
- A validação Anti-OO ocorre na fase de análise semântica (não apenas no Lexer).

## 8. Edge Cases & Estados de Erro
| Cenario | Comportamento Esperado |
|---------|----------------------|
| [Uso de palavras chaves como struct/class] | Compilação aborta imediatamente, informando limitação estrutural. |
| [Encadeamento de cout: cout << x << y;] | Deve ser suportado pela regra gramatical `expr_cout LSHIFT expressao`. |
| [Acesso a array: arr[0]] | Deve ser suportado pela regra `IDENT LBRACKET expressao RBRACKET`. |

## 9. Nao-Objetivos (Out of Scope)
- Executar o código SimC gerado na máquina alvo.
- Escrever um frontend que entenda todo o padrão do C++ moderno (C++17/C++20).
- Suportar ponteiros, referências, herança, polimorfismo.

## 10. Metricas de Sucesso
| Metrica | Alvo | Como Medir |
|---------|------|-----------|
| Golden Tests de Aceite | 100% de sucesso | Rodar `make test` |

## 11. Dependencias & Riscos
- **Riscos:** Reescrever o parser `.y` e reestruturar os golden tests tomará tempo na próxima sprint. O Lexer `.l` precisa ser minuciosamente limpo.

## 12. Referencias
| Tipo | Link / Descricao |
|------|-----------------|
| ADR (Tech Lead) | `PROJECT_CONTEXT.md` criado pelo @tech-lead |
| Subconjunto C++ | `docs/subconjunto-cpp.md` — especificação completa |
