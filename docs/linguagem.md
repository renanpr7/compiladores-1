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
| `bool` | `int` | `true` → `1`, `false` → `0`; número guardado em `bool` vira `(valor != 0)` |
| `void` | `void` | só como retorno |
| `string s = "Joao";` | `char *s = "Joao";` | só literais, sem concatenação; sem valor, começa em `""` |
| `const int N = 10;` | `const int N = 10;` | atribuir a um `const` é erro semântico |

Um `float` constante que passa do maior valor do tipo, seja literal ou resultado de conta, é erro semântico `valor float fora do intervalo`.

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

Fora de função, o valor inicial e o tamanho de array só podem ter literais e operadores, como o C
exige. Identificador (mesmo `const`), chamada, índice ou atribuição ali é erro semântico:

```cpp
int a = 1;
int b = a + 1;        // erro: valor inicial da variavel global 'b' deve ser constante
const int N = 3;
int v[N];             // erro: tamanho do array global 'v' deve ser constante
int w[2 * 4];         // aceito
```

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

`++` e `--` valem em duas posições: como comando isolado (`i++;`) e no passo do `for`. Dentro de
uma expressão maior (`b = a++ * 2;`) são erro. Só a forma posfixa é aceita: `++i` e `--i` são
erro, inclusive como comando e no passo do `for`.

O alvo de `++`, `--` e das atribuições precisa ser uma variável ou um elemento de array, escrito sem
parênteses. `5++`, `f()++`, `(a + b)++` e `1 = x` são erro de sintaxe, e `(x)++` e `(x) = 5`
também.

Comparações `==` e `!=` entre `string` são traduzidas para `strcmp(a, b) == 0` e `strcmp(a, b) != 0`
(com `#include <string.h>` incluído no C gerado quando houver comparação de strings).

## Arrays

Uma dimensão, declaração e acesso por índice.

```cpp
int v[10];
v[0] = 10;
```

Inicialização com chaves (`int v[3] = {1,2,3};`) e array como parâmetro não são suportados.
Array de `string` é erro semântico: cada `string` precisa começar em `""`, e o C não tem como
fazer isso para todas as posições de um array.

## Funções

`main` só é aceita como `int main()`: `void main`, `main` com parâmetros ou sobrecarregada são erro
semântico.

### Sobrecarga

C não permite duas funções com o mesmo nome. O compilador renomeia pelos tipos dos parâmetros:

```cpp
int   maior(int a, int b)       →   int   maior_ii(int a, int b)
float maior(float a, float b)   →   float maior_ff(float a, float b)

maior(1, 2)                     →   maior_ii(1, 2)
maior(x, y)   // x, y float     →   maior_ff(x, y)
```

Saber qual versão chamar depende do tipo dos argumentos, então a escolha é da análise semântica.

### Referências

C não tem referências. Viram ponteiros, e cada uso é dereferenciado:

```cpp
void dobra(int& x) {       →   void dobra(int *x) {
    x = x * 2;             →       (*x) = ((*x) * 2);
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

Como o valor é copiado para o ponto da chamada, ele só pode ter literais e operadores. Um nome ali
pode não existir na chamada, ou ser outra variável que o sombreia, então `int f(int a, int b = a)`
e `int h(int x = g)` são erro semântico.

## Entrada e saída

Uma cadeia de `<<` vira um único `printf` e o `endl` acrescenta `\n` à string de formato — exceto
quando algum item contém chamada de função ou atribuição, caso em que cada item gera seu próprio
`printf` (levando o `\n` do `endl` seguinte) para preservar a ordem de avaliação. Da mesma forma, uma
cadeia de `>>` vira um único `scanf`, exceto quando algum alvo é elemento de array, emitindo um
`scanf` por alvo.

| C++ | C |
|-----|---|
| `cout << x;` (`int`) | `printf("%d", x);` |
| `cout << x;` (`float`) | `printf("%f", x);` |
| `cout << "texto";` | `printf("%s", "texto");` |
| `cout << x << y;` | `printf("%d%d", x, y);` |
| `cout << x << endl;` | `printf("%d\n", x);` |
| `cin >> x;` (`int`) | `scanf("%d", &x);` |
| `cin >> a >> b;` | `scanf("%d%d", &a, &b);` |

O especificador (`%d`, `%f`, `%s`) depende do tipo da variável, então a tradução de E/S só é
possível depois da tabela de símbolos.

Um item de `cout` com comparação, `&&`, `||`, `? :` ou atribuição precisa de parênteses:
`cout << (a && b);`. Em C++, o `<<` tem precedência maior que esses operadores, e `cout << a && b`
imprime só o `a`. Sem os parênteses, é erro de sintaxe.

`cin` lê `int`, `float` e `bool`. Como `string` só guarda literais, ler uma `string` é erro
semântico. Para `bool`, a entrada deve ser `0` ou `1`: o valor lido não é normalizado.

Na inicialização, na atribuição com `=`, no `return` de função `bool` e no valor padrão, um `int`
ou `float` que vai para um `bool` é trocado por `(valor != 0)`, como faz o C++: `bool b = 5;` vira
`int b = 1;` e `b = x;` vira `b = (x != 0);`.

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

A saída sempre começa com `#include <stdio.h>`.

## Comentários

`//` até o fim da linha e `/* */` multilinha. Descartados pelo lexer.

## Limitações

O g++ aceita estas construções, mas aqui elas são recusadas com exit 1:

- **Conversão de tipo na chamada.** O argumento precisa ter exatamente o tipo do parâmetro: com
  `float metade(float x)`, a chamada `metade(3)` é erro semântico (`nenhum candidato compativel`).
- **Protótipo.** `int f(int x);` é erro de sintaxe. A função precisa estar definida antes da
  primeira chamada.
- **`const` em parâmetro e em retorno.** `void f(const int x)`, `void f(const string& s)` e
  `const int f()` são erro de sintaxe.
- **`bool` em conta.** `bool` não entra em `+` `-` `*` `/` `%`, no `-` unário nem nas atribuições
  compostas: `b + 1`, `-b`, `b += 1` e `x += b` são erro semântico (`tipos incompativeis`). O
  resultado de comparação e de `!` também é `bool`, então `(a < b) + 1` dá o mesmo erro.
- **Formato do literal float.** Precisa de dígitos dos dois lados do ponto, sem sufixo nem
  expoente: `2.5f`, `1e3`, `.5` e `5.` são recusados.

Estas são aceitas, mas o resultado difere do g++:

- **Literal float é `float`.** Em C++ ele é `double`. O valor é guardado com precisão de `float`
  (32 bits), e o constant folding também calcula em `float`: `cout << 16777217.0;` vira
  `printf("%f", 16777216.0);` e `cout << 100000.0 / 3.0;` vira `printf("%f", 33333.332);`. Na
  sobrecarga, `maior(1.0, 2.0)` chama `maior_ff`; no g++ a chamada é ambígua.
- **Nomes que o C gerado já usa.** O compilador não os recusa, e o C pode não compilar: `printf` ou
  `scanf` como variável num escopo que usa `cout` ou `cin`; função ou variável global com nome do
  `<stdio.h>`, como `remove`; `strcmp` quando o programa compara `string`; palavra reservada só do
  C, como `restrict`; e nome no formato das sobrecargas renomeadas, como `f_i` ao lado de uma `f`
  sobrecarregada.

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

int maior_ii(int a, int b) {
    return ((a > b) ? a : b);
}

float maior_ff(float a, float b) {
    return ((a > b) ? a : b);
}

void dobra(int *x) {
    (*x) = ((*x) * 2);
}

int main(void) {
    int a = 3;
    dobra(&a);
    for (int i = 0; (i < 2); i++) {
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
Erro de sintaxe na linha 1, coluna 7: syntax error
```
