# A linguagem: o que entra e o que sai

O compilador aceita um subconjunto imperativo de C++ e emite C. Este documento é a referência de
o que é aceito e como cada construção é traduzida.

Boa parte do subconjunto **já é C válido** e sai sem alteração: `int`, `float`, `if`, `while`,
`for`, funções, arrays e todos os operadores. O que realmente muda são quatro coisas — `cout`,
`cin`, `string` e os `#include`.

## Tipos

| C++ | C | Observação |
|-----|---|------------|
| `int` | `int` | idêntico |
| `float` | `float` | idêntico |
| `void` | `void` | idêntico |
| `string nome = "Joao";` | `char nome[] = "Joao";` | só literais, sem concatenação |

## Estruturas de controle

`if` / `else`, `while` e `for` passam sem alteração:

```cpp
if (a > b) { ... } else { ... }
while (a > 0) { ... }
for (i = 0; i < 10; i = i + 1) { ... }
```

## Funções

Declaração, parâmetros, chamada e `return` — tudo idêntico em C.

```cpp
int soma(int a, int b) { return a + b; }
int r = soma(10, 20);
```

## Arrays

Uma dimensão, declaração e acesso por índice:

```cpp
int valores[10];
valores[0] = 10;
int x = valores[0];
```

**Não suportado:** inicialização com chaves (`int a[3] = {1,2,3};`), arrays como parâmetro de
função e arrays multidimensionais.

## Entrada e saída — a parte que muda

### `cout` → `printf`

| C++ | C |
|-----|---|
| `cout << x;` (x é `int`) | `printf("%d", x);` |
| `cout << x;` (x é `float`) | `printf("%f", x);` |
| `cout << "texto";` | `printf("texto");` |
| `cout << x << y;` | `printf("%d%d", x, y);` |
| `cout << x << endl;` | `printf("%d\n", x);` |

Uma cadeia de `<<` vira **um único** `printf`. O `endl` acrescenta `\n` à string de formato.

### `cin` → `scanf`

| C++ | C |
|-----|---|
| `cin >> x;` (x é `int`) | `scanf("%d", &x);` |
| `cin >> x;` (x é `float`) | `scanf("%f", &x);` |
| `cin >> a >> b;` | `scanf("%d%d", &a, &b);` |

Repare no `&` antes da variável — obrigatório em C e fácil de esquecer.

> **Por que isso fica para a Entrega 2:** o especificador de formato (`%d`, `%f`, `%s`) depende do
> tipo da variável, e o compilador só sabe o tipo depois de montar a tabela de símbolos. Na Entrega
> 1 o parser apenas reconhece a construção; a tradução vem junto com a análise semântica.

### Cabeçalhos

Linhas iniciadas por `#` na entrada são descartadas. O compilador emite o cabeçalho que a saída
precisa: se o programa usa `cout` ou `cin`, sai `#include <stdio.h>` no topo.

## Operadores

Todos idênticos em C, com a mesma precedência:

| Categoria | Operadores |
|-----------|------------|
| Aritméticos | `+` `-` `*` `/` `%` |
| Relacionais | `==` `!=` `<` `>` `<=` `>=` |
| Lógicos | `&&` `\|\|` `!` |
| Atribuição | `=` |

Precedência, da maior para a menor: `!` e `-` unário → `*` `/` `%` → `+` `-` → `<` `>` `<=` `>=`
→ `==` `!=` → `&&` → `||`. Todos associam da esquerda para a direita, menos os unários.

## Comentários

`//` até o fim da linha e `/* */` multi-linha. São descartados pelo lexer e não geram tokens.

## Fora do escopo

**Orientação a objetos:** `class`, `struct`, `new`, `delete`, `template`, `virtual`, `override`,
`public`, `private`, `protected`, `namespace`, `using`, `this`, `nullptr`, `friend`, `operator`,
herança, polimorfismo, sobrecarga.

Essas palavras **são reconhecidas** pelo lexer (`class` vira o token `CLASS`) e recusadas pelo
parser, com mensagem clara. Não são ignoradas nem tratadas como identificador.

**Outros:** ponteiros e referências, exceções, STL, lambdas, enums, `typedef`, casts, `do while`,
`switch`, operador ternário `? :`.

## Limitações da Entrega 1

Na Entrega 1 o compilador só valida a sintaxe e monta a AST. Ele **não** verifica escopo, **não**
verifica se a variável foi declarada antes do uso, **não** verifica tipos e **não** gera código C.
Tudo isso é Entrega 2.

## Exemplo completo

Entrada:

```cpp
#include <iostream>

int soma(int a, int b) {
    return a + b;
}

int main() {
    int x = 10;
    int y = 20;
    cout << soma(x, y) << endl;
    return 0;
}
```

Saída esperada:

```c
#include <stdio.h>

int soma(int a, int b) {
    return a + b;
}

int main() {
    int x = 10;
    int y = 20;
    printf("%d\n", soma(x, y));
    return 0;
}
```

Programa recusado:

```cpp
class Pessoa { string nome; };
```

```
Erro de compilacao: construcao OO nao suportada 'class' na linha 1, coluna 1
```
