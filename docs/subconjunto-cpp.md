# Subconjunto de C++ Suportado — Especificação v1.0

> **Versão:** 1.0 | **Data:** 2026-08-28
> **Objetivo:** Documentar o subconjunto de C++ que o compilador aceita como entrada.

---

## 1. Visão Geral

O compilador aceita um **subconjunto imperativo do C++**, excluindo todos os recursos de programação orientada a objetos. A linguagem-alvo é **SimC**.

```
C++ (subconjunto imperativo) → Compilador → SimC
```

---

## 2. Tipos Suportados

| Tipo | Descrição | Exemplo |
|------|-----------|---------|
| `int` | Número inteiro | `int x = 10;` |
| `float` | Número de ponto flutuante | `float pi = 3.14;` |
| `string` | Texto (literal) | `string nome = "Joao";` |
| `void` | Sem valor de retorno | `void imprime() { ... }` |

---

## 3. Estruturas de Controle

### 3.1 Condicional

```cpp
if (condicao) {
    // bloco verdadeiro
} else {
    // bloco falso (opcional)
}
```

### 3.2 Loop While

```cpp
while (condicao) {
    // corpo do loop
}
```

### 3.3 Loop For

```cpp
for (inicio; condicao; incremento) {
    // corpo do loop
}
```

---

## 4. Funções

```cpp
// Declaração
int soma(int a, int b) {
    return a + b;
}

// Chamada
int resultado = soma(10, 20);
```

**Regras:**
- Parâmetros: tipos `int`, `float`, `string`, `void`
- Retorno: tipo explícito ou `void`
- Múltiplos parâmetros separados por vírgula

---

## 5. Arrays Básicos

### 5.1 Declaração

```cpp
int valores[10];
float medias[5];
```

### 5.2 Acesso por Índice

```cpp
// Atribuição
valores[0] = 10;
valores[1] = 20;

// Leitura
int x = valores[0];
```

### 5.3 O que NÃO é suportado

- Inicialização com literais: `int arr[3] = {1, 2, 3};` ❌
- Arrays como parâmetros de funções ❌
- Arrays multidimensionais ❌
- Tamanho dinâmico ❌

---

## 6. Entrada e Saída (I/O)

### 6.1 Saída — `cout`

| C++ (entrada) | SimC (saída) |
|---------------|--------------|
| `std::cout << x;` | `print(x);` |
| `std::cout << "hello";` | `print("hello");` |
| `std::cout << x << y;` | `print(x, y);` |
| `std::cout << x << endl;` | `print(x);` |

**Regras:**
- `cout` sempre seguido de `<<`
- Pode encadear múltiplos `<<`
- `endl` gera newline (não precisa de parâmetro adicional)

### 6.2 Entrada — `cin`

| C++ (entrada) | SimC (saída) |
|---------------|--------------|
| `std::cin >> x;` | `input(x);` |
| `std::cin >> a >> b;` | `input(a); input(b);` |

**Regras:**
- `cin` sempre seguido de `>>`
- Pode encadear múltiplos `>>`

---

## 7. Operadores

### 7.1 Aritméticos

| Operador | Significado | Exemplo |
|----------|-------------|---------|
| `+` | Soma | `a + b` |
| `-` | Subtração | `a - b` |
| `*` | Multiplicação | `a * b` |
| `/` | Divisão | `a / b` |
| `%` | Módulo | `a % b` |

### 7.2 Relacionais

| Operador | Significado | Exemplo |
|----------|-------------|---------|
| `==` | Igual | `a == b` |
| `!=` | Diferente | `a != b` |
| `<` | Menor que | `a < b` |
| `>` | Maior que | `a > b` |
| `<=` | Menor ou igual | `a <= b` |
| `>=` | Maior ou igual | `a >= b` |

### 7.3 Lógicos

| Operador | Significado | Exemplo |
|----------|-------------|---------|
| `&&` | E lógico | `a && b` |
| `\|\|` | OU lógico | `a \|\| b` |
| `!` | NÃO lógico | `!a` |

### 7.4 Atribuição

| Operador | Significado | Exemplo |
|----------|-------------|---------|
| `=` | Atribuição simples | `a = 10` |

### 7.5 Precedência (maior para menor)

| Precedência | Operadores | Associatividade |
|-------------|------------|-----------------|
| 1 (maior) | `!`, unário `-` | Direita para esquerda |
| 2 | `*`, `/`, `%` | Esquerda para direita |
| 3 | `+`, `-` | Esquerda para direita |
| 4 | `<`, `>`, `<=`, `>=` | Esquerda para direita |
| 5 | `==`, `!=` | Esquerda para direita |
| 6 | `&&` | Esquerda para direita |
| 7 (menor) | `\|\|` | Esquerda para direita |

---

## 8. Delimitadores

| Token | Símbolo |
|-------|---------|
| Parêntese | `(` `)` |
| Chave | `{` `}` |
| Colchete | `[` `]` |
| Ponto e vírgula | `;` |
| Vírgula | `,` |

---

## 9. Comentários

```cpp
// Comentário de linha

/*
   Comentário de bloco
   pode ser multi-linha
*/
```

Comentários são ignorados pelo compilador (não geram tokens).

---

## 10. Diretivas de Pré-processador

```cpp
#include <iostream>
#define MAX 100
```

Todas as linhas começando com `#` são **ignoradas** pelo compilador.

---

## 11. Fora do Escopo (Não Suportado)

### 11.1 Programação Orientada a Objetos

- `class`
- `struct`
- `new` / `delete`
- `template`
- `virtual` / `override`
- `public` / `private` / `protected`
- `namespace` / `using`
- `this` / `nullptr`
- Herança
- Polimorfismo
- Sobrecarga de operadores

### 11.2 Recursos Avançados

- Ponteiros (`*`, `&`)
- Referências
- Exceções (`try`, `catch`, `throw`)
- STL (vector, map, etc.)
- Lambdas
- Enums
- Typedef / using
- Casts (`static_cast`, etc.)

### 11.3 Controle Não Suportado

- `do while`
- `switch` / `case`
- Ternário (`? :`)
- `break` / `continue` (fora de loops)
- `goto`

---

## 12. Limitações Conhecidas (v1)

| Limitação | Motivo | Será resolvido em |
|-----------|--------|-------------------|
| Não valida escopo de variáveis | Validador sintático puro | Entrega 2 (análise semântica) |
| Não valida declaração antes de uso | Validador sintático puro | Entrega 2 (análise semântica) |
| Não valida tipos em operações | Validador sintático puro | Entrega 2 (análise semântica) |
| Não gera código SimC | Escopo da v1 | Entrega 2 (code gen) |

---

## 13. Exemplos

### 13.1 Programa Válido

```cpp
#include <iostream>

int soma(int a, int b) {
    return a + b;
}

int main() {
    int x = 10;
    int y = 20;
    int resultado = soma(x, y);
    
    cout << resultado << endl;
    
    return 0;
}
```

### 13.2 Programa com Arrays

```cpp
int main() {
    int valores[5];
    valores[0] = 10;
    valores[1] = 20;
    valores[2] = 30;
    
    int x = valores[0];
    cout << x << endl;
    
    return 0;
}
```

### 13.3 Programa Inválido (OO)

```cpp
class Pessoa {  // ERRO: construção OO não suportada
    string nome;
};

int main() {
    return 0;
}
```

**Erro esperado:** `Erro de compilacao: construcao OO nao suportada 'class'`

---

*Criado em 28/08/2026*
