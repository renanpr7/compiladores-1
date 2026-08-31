# A linguagem

O compilador aceita um subconjunto imperativo de C++ e emite C equivalente. Este documento define
o que é aceito e como cada construção é traduzida.

Boa parte do subconjunto já é C válido e sai quase sem alteração. O que exige tradução de verdade
são as três construções que só existem em C++: sobrecarga de funções, referências e parâmetros com
valor padrão.

## Tipos

| C++ | C | |
|-----|---|---|
| `int` | `int` | |
| `float` | `float` | |
| `bool` | `int` | `true` → `1`, `false` → `0` |
| `void` | `void` | só como retorno |
| `string s = "Joao";` | `char s[] = "Joao";` | só literais, sem concatenação |
| `const int N = 10;` | `const int N = 10;` | atribuir a um `const` é erro semântico |

## Estruturas de controle

`if` / `else`, `while`, `do while` e `for`. Todas existem em C e saem sem alteração.

```cpp
if (a > b) { ... } else { ... }
while (a > 0) { ... }
do { ... } while (a > 0);
for (int i = 0; i < n; i++) { ... }
```

`break` e `continue` valem dentro de laço. Fora de um laço, são erro semântico.

## Escopo

Cada bloco `{ }` abre um escopo. Uma variável declarada dentro de um bloco não existe fora dele e
pode sombrear uma de fora.

```cpp
int x = 1;
{
    int x = 2;   // outra variável
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
| Incremento | `++` `--` |
| Ternário | `cond ? a : b` |

Precedência, da maior para a menor: unários `!` e `-` → `*` `/` `%` → `+` `-` → `<` `>` `<=` `>=`
→ `==` `!=` → `&&` → `||` → `? :` → `=` `+=` `-=` `*=` `/=`.

Todos associam da esquerda para a direita, exceto os unários, o ternário e as atribuições.

`a += b` é reescrito como `a = a + b`.

`++` e `--` valem em duas posições: como comando isolado (`i++;`) e no passo do `for`. Nas duas,
viram `i = i + 1`. Dentro de uma expressão maior (`b = a++ * 2;`) são erro.

## Arrays

Uma dimensão, declaração e acesso por índice.

```cpp
int v[10];
v[0] = 10;
```

Inicialização com chaves (`int v[3] = {1,2,3};`) e array como parâmetro não são suportados.

## Funções

### Sobrecarga

C não permite duas funções com o mesmo nome. O compilador renomeia pelos tipos dos parâmetros:

```cpp
int   maior(int a, int b)       →   int   maior_ii(int a, int b)
float maior(float a, float b)   →   float maior_ff(float a, float b)

maior(1, 2)                     →   maior_ii(1, 2)
maior(1.0, 2.0)                 →   maior_ff(1.0, 2.0)
```

Saber qual versão chamar depende do tipo dos argumentos, então a escolha é da análise semântica.

### Referências

C não tem referências. Viram ponteiros, e cada uso é dereferenciado:

```cpp
void dobra(int& x) {       →   void dobra(int* x) {
    x = x * 2;             →       *x = (*x) * 2;
}                          →   }

int a = 5;                 →   int a = 5;
dobra(a);                  →   dobra(&a);
```

### Parâmetros com valor padrão

C não tem. O compilador preenche o argumento que faltou, no ponto da chamada:

```cpp
int soma(int a, int b = 10)  →   int soma(int a, int b)
soma(5)                      →   soma(5, 10)
soma(5, 3)                   →   soma(5, 3)
```

## Entrada e saída

Uma cadeia de `<<` vira um único `printf`. O `endl` acrescenta `\n` à string de formato.

| C++ | C |
|-----|---|
| `cout << x;` (`int`) | `printf("%d", x);` |
| `cout << x;` (`float`) | `printf("%f", x);` |
| `cout << "texto";` | `printf("texto");` |
| `cout << x << y;` | `printf("%d%d", x, y);` |
| `cout << x << endl;` | `printf("%d\n", x);` |
| `cin >> x;` (`int`) | `scanf("%d", &x);` |
| `cin >> a >> b;` | `scanf("%d%d", &a, &b);` |

O especificador (`%d`, `%f`, `%s`) depende do tipo da variável, então a tradução de E/S só é
possível depois da tabela de símbolos.

## Cabeçalhos e qualificação

O lexer descarta, sem gerar token:

- qualquer linha começando por `#`
- `using namespace std;`
- o prefixo `std::` em `std::cout`, `std::cin` e `std::endl`

As duas últimas garantem que o programa de entrada seja C++ que compila no `g++`: um `cout` sem
`using namespace std;` e sem `std::` não é código válido. As duas formas abaixo são equivalentes
para o compilador:

```cpp
using namespace std;        std::cout << x << std::endl;
cout << x << endl;
```

Se o programa usa `cout` ou `cin`, a saída recebe `#include <stdio.h>` no topo.

## Comentários

`//` até o fim da linha e `/* */` multilinha. Descartados pelo lexer.

## Fora do escopo

**Orientação a objetos:** `class`, `new`, `delete`, `template`, `virtual`, `override`, `public`,
`private`, `protected`, `namespace`, `using`, `this`, `nullptr`, `friend`, `operator`, herança,
polimorfismo e sobrecarga de operadores. A única exceção é `using namespace std;`, descrita acima.

**Construções que C++ e C escrevem igual**, e que por isso não exercitam tradução: `struct`,
`switch` / `case` / `default` e arrays de duas dimensões.

Os dois grupos acima são reconhecidos pelo lexer e recusados pelo parser com mensagem própria.
Nada é ignorado em silêncio nem tratado como identificador.

**Outros:** ponteiros explícitos, exceções, STL, lambdas, enums, `typedef`, casts e `goto`. Não têm
token próprio: o lexer os devolve como identificador e o erro aparece na estrutura.

## Exemplo

```cpp
#include <iostream>
using namespace std;

int   maior(int a, int b)      { return a > b ? a : b; }
float maior(float a, float b)  { return a > b ? a : b; }

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

```c
#include <stdio.h>

int   maior_ii(int a, int b)      { return a > b ? a : b; }
float maior_ff(float a, float b)  { return a > b ? a : b; }

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
