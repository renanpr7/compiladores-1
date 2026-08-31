# A linguagem: o que entra e o que sai

O compilador aceita um subconjunto imperativo de C++ e emite C. Este documento é a referência do
que é aceito e de como cada construção é traduzida.

Parte do subconjunto já é C válido e sai quase sem alteração. A parte interessante são as
construções que **só existem em C++** — sobrecarga, referências, parâmetros com valor padrão — onde
o compilador precisa realmente traduzir, e não copiar.

## Tipos

| C++ | C | Observação |
|-----|---|------------|
| `int` | `int` | |
| `float` | `float` | |
| `bool` | `int` | `true` → `1`, `false` → `0` |
| `void` | `void` | só como retorno |
| `string nome = "Joao";` | `char nome[] = "Joao";` | só literais, sem concatenação |
| `const int N = 10;` | `const int N = 10;` | atribuir a um `const` é erro semântico |

## Estruturas de controle

`if` / `else`, `while`, `for`, `do while` e `switch`. Todas existem em C e saem sem alteração.

```cpp
if (a > b) { ... } else { ... }
while (a > 0) { ... }
do { ... } while (a > 0);
for (int i = 0; i < n; i++) { ... }

switch (op) {
    case 1: ...; break;
    case 2: ...; break;
    default: ...;
}
```

`break` e `continue` são suportados dentro de laços e do `switch`. Usá-los fora de um laço é erro
semântico.

## Escopo

Cada bloco `{ }` abre um escopo novo. Uma variável declarada dentro de um bloco não existe fora
dele, e pode sombrear uma de fora.

```cpp
int x = 1;
{
    int x = 2;   // outra variável, sombreia a de fora
}
                 // aqui x vale 1
```

Usar variável não declarada, ou declarar duas vezes no mesmo escopo, é erro semântico.

## Operadores

| Categoria | Operadores |
|-----------|------------|
| Aritméticos | `+` `-` `*` `/` `%` |
| Relacionais | `==` `!=` `<` `>` `<=` `>=` |
| Lógicos | `&&` `\|\|` `!` |
| Atribuição | `=` `+=` `-=` `*=` `/=` |
| Incremento | `++` `--` (prefixo e sufixo) |
| Ternário | `cond ? a : b` |

Precedência, da maior para a menor: `++` `--` `!` e `-` unário → `*` `/` `%` → `+` `-` →
`<` `>` `<=` `>=` → `==` `!=` → `&&` → `||` → `? :` → `=` `+=` `-=` `*=` `/=`.

Todos associam da esquerda para a direita, exceto os unários, o ternário e as atribuições.

`a += b` é reescrito como `a = a + b`. `i++` como expressão isolada vira `i = i + 1`; dentro de uma
expressão maior, o compilador gera o temporário necessário para preservar a ordem.

## Arrays

Uma ou duas dimensões, declaração e acesso por índice:

```cpp
int valores[10];
int matriz[3][4];
valores[0] = 10;
matriz[1][2] = 7;
```

**Não suportado:** inicialização com chaves (`int a[3] = {1,2,3};`) e arrays como parâmetro.

## Structs

Registros com campos, sem métodos. Existem nas duas linguagens e saem sem alteração, mas obrigam a
tabela de símbolos a guardar os campos de cada tipo.

```cpp
struct Ponto {
    int x;
    int y;
};

Ponto p;
p.x = 10;
```

**Não suportado:** métodos, construtores, herança, `private`/`public`.

## Funções — onde C++ e C divergem

### Sobrecarga

C não permite duas funções com o mesmo nome. O compilador renomeia com base nos tipos dos
parâmetros (*name mangling*):

```cpp
int  maior(int a, int b)       →   int   maior_ii(int a, int b)
float maior(float a, float b)  →   float maior_ff(float a, float b)

maior(1, 2)                    →   maior_ii(1, 2)
maior(1.0, 2.0)                →   maior_ff(1.0, 2.0)
```

Escolher qual versão chamar exige saber o tipo dos argumentos — trabalho da análise semântica.

### Referências

C não tem referências. Viram ponteiros, e todo uso da variável é dereferenciado:

```cpp
void dobra(int& x) {       →   void dobra(int* x) {
    x = x * 2;             →       *x = (*x) * 2;
}                          →   }

int a = 5;                 →   int a = 5;
dobra(a);                  →   dobra(&a);
```

### Parâmetros com valor padrão

C não tem. O compilador preenche o argumento que faltou no ponto da chamada:

```cpp
int soma(int a, int b = 10)  →   int soma(int a, int b)
soma(5)                      →   soma(5, 10)
soma(5, 3)                   →   soma(5, 3)
```

## Entrada e saída

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
| `cin >> a >> b;` | `scanf("%d%d", &a, &b);` |

Repare no `&` antes da variável — obrigatório em C e fácil de esquecer.

> O especificador de formato (`%d`, `%f`, `%s`) depende do tipo da variável, então o compilador só
> consegue traduzir I/O depois de montar a tabela de símbolos.

### Cabeçalhos

Linhas iniciadas por `#` na entrada são descartadas. O compilador emite o cabeçalho de que a saída
precisa: se o programa usa `cout` ou `cin`, sai `#include <stdio.h>` no topo.

## Comentários

`//` até o fim da linha e `/* */` multi-linha. Descartados pelo lexer, não geram tokens.

## Fora do escopo

**Orientação a objetos:** `class`, `new`, `delete`, `template`, `virtual`, `override`, `public`,
`private`, `protected`, `namespace`, `using`, `this`, `nullptr`, `friend`, `operator`, herança,
polimorfismo, sobrecarga de operadores, métodos em `struct`.

Essas palavras **são reconhecidas** pelo lexer (`class` vira o token `CLASS`) e recusadas pelo
parser com mensagem clara. Não são ignoradas nem tratadas como identificador.

**Outros:** ponteiros explícitos, exceções, STL, lambdas, enums, `typedef`, casts, `goto`,
arrays com mais de duas dimensões.

## Exemplo completo

Entrada:

```cpp
#include <iostream>

int maior(int a, int b) { return a > b ? a : b; }
float maior(float a, float b) { return a > b ? a : b; }

void dobra(int& x) { x = x * 2; }

int main() {
    int a = 3;
    dobra(a);
    for (int i = 0; i < 2; i++) {
        cout << maior(a, i) << endl;
    }
    return 0;
}
```

Saída:

```c
#include <stdio.h>

int maior_ii(int a, int b) { return a > b ? a : b; }
float maior_ff(float a, float b) { return a > b ? a : b; }

void dobra(int* x) { *x = (*x) * 2; }

int main() {
    int a = 3;
    dobra(&a);
    for (int i = 0; i < 2; i = i + 1) {
        printf("%d\n", maior_ii(a, i));
    }
    return 0;
}
```

Programa recusado:

```cpp
class Pessoa { int idade; };
```

```
Erro de compilacao: construcao OO nao suportada 'class' na linha 1, coluna 1
```
