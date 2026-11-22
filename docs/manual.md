# Manual de Uso

Este manual descreve como compilar, executar, depurar e testar o compilador `C++ (subconjunto imperativo) → C99`. A especificação completa do subconjunto da linguagem aceito e das regras de tradução para C está em [docs/linguagem.md](linguagem.md).

## Fluxo de entrada e saída

O compilador opera como um filtro Unix padrão:

- **`stdin` (entrada padrão):** lê o código-fonte C++ exclusivamente da entrada padrão via redirecionamento (`<`) ou *pipe* (`|`).
- **`stdout` (saída padrão):** emite o resultado da fase selecionada (lista de tokens, árvore sintática abstrata, código intermediário TAC, texto de ajuda ou código C99 final).
- **`stderr` (saída de erros):** recebe todas as mensagens de erro de compilação (léxicos, sintáticos e semânticos) e a mensagem de uso quando a invocação na linha de comando é inválida.

O binário **não aceita nome de arquivo como argumento posicional** nem múltiplas flags simultâneas. Qualquer argumento além de uma única flag válida cai na validação `argc != 1` de `src/main.c`, imprime a ajuda em `stderr` e encerra com código de saída `2`:

```bash
./compilador < programa.cpp       # correto: lê programa.cpp via stdin
./compilador programa.cpp         # erro de uso (exit 2): argumento posicional não é aceito
./compilador -t -a < programa.cpp # erro de uso (exit 2): apenas uma opção por execução
```

## Requisitos e compilação

O projeto foi desenvolvido e validado em ambiente Linux / WSL (Ubuntu) com:

- `flex` (2.6+)
- `bison` (3.8+)
- `gcc` com suporte a `-std=c99`
- `make`

Para instalar as dependências no Ubuntu/Debian:

```bash
sudo apt update
sudo apt install flex bison build-essential
```

### Alvos do `Makefile`

| Comando | O que faz |
|---------|-----------|
| `make` | Executa `bison -d` sobre `src/parser.y` (gerando `src/parser.tab.c` e `src/parser.tab.h`), `flex` sobre `src/lexer.l` (gerando `src/lex.yy.c`) e compila todos os módulos em `src/` com `gcc -std=c99 -Wall -Wextra -D_DEFAULT_SOURCE`, produzindo o binário `./compilador`. |
| `make test` | Compila `./compilador` (se necessário) e executa os 151 *golden tests* (`tests/run_tests.sh`) e os 13 testes de interface de linha de comando (`tests/cli/run_cli_tests.sh`). Nos casos positivos de `parser/`, `semantica/`, `tac/` e `codegen/`, o runner também valida o C gerado com `gcc -std=c99 -fsyntax-only`; na suíte `execucao/`, compila o C gerado com `gcc`, executa o binário resultante e compara a saída com o `.expected`. |
| `make test-memory` | Recompila o compilador com AddressSanitizer e UndefinedBehaviorSanitizer (`-g -fsanitize=address,undefined`), executa toda a bateria de *golden tests* e de CLI verificando ausência de vazamentos de memória e acessos inválidos, e ao final recompila o binário padrão sem os sanitizadores. O script `tests/run_tests.sh` também suporta `USE_VALGRIND=1` quando o `valgrind` está instalado. |
| `make clean` | Remove o binário `./compilador` e os arquivos gerados pelo Flex e pelo Bison (`src/lex.yy.c`, `src/parser.tab.c` e `src/parser.tab.h`). |

## Opções de linha de comando e fases do pipeline

O pipeline interno completo é composto por:

```
Flex (léxico) → Bison (sintático + AST) → Análise Semântica → Constant Folding → TAC (-i) / Codegen C99
```

Cada modo de invocação aciona um subconjunto específico dessas fases:

| Invocação | Fases executadas | `stdout` (em sucesso) | Comportamento em erro | Exit code |
|-----------|------------------|-----------------------|-----------------------|-----------|
| `./compilador < prog.cpp` | Léxico → Sintático (AST) → Semântica → Constant Folding → Geração C99 | Código C99 equivalente | Aborta na fase que falhou; `stdout` fica vazio e diagnósticos saem em `stderr` | `0` (sucesso) ou `1` (erro) |
| `./compilador -t < prog.cpp`<br>`./compilador --tokens < prog.cpp` | Apenas Léxico | Um token por linha (`L:C TOKEN "lexema"`) seguido de `L:C EOF` | Erros léxicos saem em `stderr`, mas os tokens válidos continuam sendo impressos em `stdout` até `EOF` | `0` (sucesso) ou `1` (erro léxico) |
| `./compilador -a < prog.cpp`<br>`./compilador --ast < prog.cpp` | Léxico → Sintático (AST) → Constant Folding *(sem Análise Semântica)* | Árvore sintática abstrata indentada (já simplificada pelo *constant folding*) | Aborta se houver erro léxico, sintático ou de *constant folding*; `stdout` fica vazio | `0` (sucesso) ou `1` (erro) |
| `./compilador -i < prog.cpp`<br>`./compilador --tac < prog.cpp` | Léxico → Sintático (AST) → Semântica → Constant Folding → Geração TAC | Código intermediário de três endereços (TAC) | Aborta se houver erro léxico, sintático, semântico ou de *folding*; `stdout` fica vazio | `0` (sucesso) ou `1` (erro) |
| `./compilador -h`<br>`./compilador --help` | Nenhuma (não lê `stdin`) | Texto de ajuda com opções e exemplos | — | `0` |
| Flag inválida ou argumentos extras | Nenhuma (não lê `stdin`) | Vazio | Imprime o texto de ajuda em `stderr` | `2` |

> **Por que `-a` não roda a análise semântica:** a opção `-a` / `--ast` permite inspecionar a estrutura sintática e a simplificação de expressões constantes (*constant folding*) mesmo em fragmentos que usam identificadores não declarados, como nos testes isolados de precedência em `tests/ast/`. Ainda assim, erros detectados durante o *constant folding* (como divisão constante por zero, `float` fora do intervalo ou tamanho negativo de array após a simplificação) fazem `-a` falhar com código `1`.

### Códigos de saída

- **`0`**: execução concluída sem erros (ou exibição de `-h` / `--help`).
- **`1`**: erro de compilação detectado na entrada (léxico, sintático, semântico ou durante o *constant folding*).
- **`2`**: erro de uso na linha de comando (opção desconhecida, mais de uma flag ou arquivo passado sem redirecionamento `<`).

## Exemplo ponta a ponta

Considere o arquivo `exemplo.cpp` abaixo, que exercita passagem por referência (`int&`), parâmetro com valor padrão envolvendo expressão constante (`2 + 3`), atribuição composta (`+=`) e saída com `cout` / `endl`:

```cpp
#include <iostream>
using namespace std;

void somar(int& total, int passo = 2 + 3) {
    total += passo;
}

int main() {
    int x = 10;
    somar(x);
    cout << x << endl;
    return 0;
}
```

### 1. Dump de tokens (`-t` / `--tokens`)

O analisador léxico descarta `#include <iostream>` e `using namespace std;`, registrando linha e coluna exatas (`linha:coluna`) de cada token reconhecido até o `EOF`:

```bash
./compilador -t < exemplo.cpp
```

```text
4:1 VOID "void"
4:6 IDENT "somar"
4:11 LPAREN "("
4:12 INT "int"
4:15 AMP "&"
4:17 IDENT "total"
4:22 COMMA ","
4:24 INT "int"
4:28 IDENT "passo"
4:34 ASSIGN "="
4:36 NUM_INT "2"
4:38 PLUS "+"
4:40 NUM_INT "3"
4:41 RPAREN ")"
4:43 LBRACE "{"
5:5 IDENT "total"
5:11 PLUS_ASSIGN "+="
5:14 IDENT "passo"
5:19 SEMI ";"
6:1 RBRACE "}"
8:1 INT "int"
8:5 IDENT "main"
8:9 LPAREN "("
8:10 RPAREN ")"
8:12 LBRACE "{"
9:5 INT "int"
9:9 IDENT "x"
9:11 ASSIGN "="
9:13 NUM_INT "10"
9:15 SEMI ";"
10:5 IDENT "somar"
10:10 LPAREN "("
10:11 IDENT "x"
10:12 RPAREN ")"
10:13 SEMI ";"
11:5 COUT "cout"
11:10 LSHIFT "<<"
11:13 IDENT "x"
11:15 LSHIFT "<<"
11:18 ENDL "endl"
11:22 SEMI ";"
12:5 RETURN "return"
12:12 NUM_INT "0"
12:13 SEMI ";"
13:1 RBRACE "}"
14:1 EOF
```

### 2. Árvore sintática abstrata após *constant folding* (`-a` / `--ast`)

Com `-a`, o parser monta a AST e o otimizador dobra a expressão constante `2 + 3` do parâmetro padrão para `NUM 5`. Quando um filho opcional anterior a outro presente está vazio — como a lista de parâmetros de `main` antes do `BLOCO` — a impressão marca a posição com `(vazio)`:

```bash
./compilador -a < exemplo.cpp
```

```text
PROGRAMA
  FUNCAO void somar
    PARAMETRO int& total
    PARAMETRO int passo
      NUM 5
    BLOCO
      ATRIBUICAO +=
        ID total
        ID passo
  FUNCAO int main
    (vazio)
    BLOCO
      DECLARACAO int
        DECLARADOR x
          NUM 10
      CHAMADA somar
        ID x
      COUT
        ID x
        ENDL
      RETURN
        NUM 0
```

### 3. Código intermediário de três endereços (`-i` / `--tac`)

Com `-i`, a análise semântica roda antes do *constant folding*, clonando o argumento padrão omitido (`2 + 3`, dobrado em seguida para `5`) diretamente na chamada `somar(x)`. A atribuição composta `total += passo` é decomposta usando o temporário `_t1`:

```bash
./compilador -i < exemplo.cpp
```

```text
func somar:
    _t1 = total + passo
    total = _t1
endfunc somar
func main:
    x = 10
    param x
    param 5
    call somar, 2
    print x
    print "\n"
    return 0
endfunc main
```

### 4. Geração de código C99 e execução com `gcc`

Sem flags, o compilador emite o código C99 final em `stdout`. A referência `int& total` vira `int *total` com dereferenciação `(*total)` no corpo e passagem de endereço `&x` na chamada, o argumento padrão `5` é preenchido no ponto da chamada e `cout << x << endl;` vira `printf("%d\n", x);`:

```bash
./compilador < exemplo.cpp > exemplo.c
cat exemplo.c
```

```c
#include <stdio.h>

void somar(int *total, int passo) {
    (*total) += passo;
}

int main(void) {
    int x = 10;
    somar(&x, 5);
    printf("%d\n", x);
    return 0;
}
```

Compilando e executando o C99 gerado com `gcc`:

```bash
gcc -std=c99 exemplo.c -o exemplo && ./exemplo
```

```text
15
```

## Diagnósticos de erro e limite de mensagens

Todos os diagnósticos são enviados para `stderr` com linha e coluna. O compilador impõe um limite global de **10 mensagens por execução** (`MAX_DIAGNOSTICS = 10` em `src/diagnostics.h`), compartilhado entre o analisador léxico, o sintático, o semântico e o *constant folding*. A partir do 11º erro, a análise continua até o fim da fase sem imprimir novas mensagens e o processo termina com código `1`.

### Formatos das mensagens por categoria

- **Erro léxico** (caractere inválido, número malformado, literal de string ou comentário de bloco não fechado):
  ```text
  Erro lexico: caractere invalido '@' na linha 1, coluna 1
  Erro lexico: numero malformado '010' na linha 2, coluna 9
  Erro lexico: string nao fechada na linha 3, coluna 12
  ```
- **Construção orientada a objetos ou fora do escopo** (reconhecida pelo lexer e recusada por regras dedicadas na gramática do Bison):
  ```text
  Erro de compilacao: construcao OO nao suportada 'class' na linha 1, coluna 1
  Erro de compilacao: construcao fora do escopo 'struct' na linha 1, coluna 1
  ```
- **Erro de sintaxe** (estrutura gramatical inválida, `++` prefixo, atribuição a expressão que não é variável/elemento de array, item de `cout` sem parênteses obrigatórios ou array 2D):
  ```text
  Erro de sintaxe na linha 1, coluna 6: syntax error
  Erro de sintaxe na linha 2, coluna 5: incremento/decremento so na forma i++ ou i--, como comando
  Erro de sintaxe na linha 3, coluna 13: item de cout com comparacao, &&, ||, ?: ou atribuicao precisa de parenteses
  Erro de sintaxe na linha 4, coluna 13: array de duas dimensoes nao suportado
  ```
- **Erro semântico e de *constant folding*** (escopo, tipos incompatíveis, sobrecarga ambígua ou sem candidato, atribuição a `const`, `break`/`continue` fora de laço, divisão constante por zero, *overflow* de `float` constante ou tamanho negativo de array):
  ```text
  Erro semantico na linha 2, coluna 12: identificador 'x' nao declarado
  Erro semantico na linha 4, coluna 5: nenhum candidato compativel para chamada de 'maior'
  Erro semantico na linha 5, coluna 13: divisao por zero
  Erro semantico na linha 6, coluna 15: valor float fora do intervalo
  Erro semantico na linha 7, coluna 9: tamanho negativo no array 'v'
  ```

## Subconjunto da linguagem e limitações conhecidas

A referência completa de tipos, operadores, estruturas de controle, escopo, arrays 1D, sobrecarga com *name mangling*, referências via ponteiros, parâmetros com valor padrão e entrada/saída está em [docs/linguagem.md](linguagem.md). Abaixo estão resumidas as restrições e diferenças em relação ao `g++`.

### Construções válidas no `g++` mas recusadas pelo compilador (exit `1`)

- **Orientação a objetos e construções fora do escopo:** `class`, `new`, `delete`, `template`, `virtual`, `override`, `public`, `private`, `protected`, `namespace`, `using` (exceto `using namespace std;`), `this`, `nullptr`, `friend`, `operator`, `struct`, `switch`/`case`/`default`, arrays 2D (`v[2][3]`), inicialização de array com chaves (`{1, 2}`), array como parâmetro e ponteiros explícitos.
- **Sem conversão implícita `int` ↔ `float` em chamadas de função:** o tipo de cada argumento deve coincidir exatamente com o tipo do parâmetro declarado (`float f(float x)` chamada como `f(3)` resulta em erro semântico).
- **Sem protótipos de função:** toda função deve ser definida com corpo `{ ... }` antes da primeira chamada.
- **`const` restrito a variáveis:** `const` não é aceito em parâmetros nem no tipo de retorno de funções.
- **Incremento e decremento restritos:** apenas as formas posfixas `i++` e `i--` são aceitas, somente como comando isolado ou no passo do `for` (proibidos dentro de expressões maiores e na forma prefixa `++i`/`--i`), e o alvo de `++`, `--` e atribuições não pode estar entre parênteses (`(x) = 1` e `(x)++` são recusados).
- **`bool` fora de operações aritméticas:** valores `bool` (incluindo resultados de comparações e de `!`) não entram em `+`, `-`, `*`, `/`, `%`, `-` unário nem em `+=`, `-=`, `*=`, `/=`.
- **Restrições de `string`:** `string` armazena apenas literais (sem concatenação com `+`), não pode ser lida com `cin >> s` e não pode ser elemento de array (`string v[5]`).
- **Expressões constantes em escopo global e valores padrão:** inicializadores de variáveis globais, tamanhos de arrays globais e valores padrão de parâmetros só aceitam literais e operadores (qualquer identificador, mesmo `const`, é recusado).
- **Formato estrito de literais numéricos:** literais `float` exigem dígitos antes e depois do ponto, sem sufixo `f` nem notação científica (`2.5f`, `1e3`, `.5` e `5.` são recusados); inteiros com mais de um dígito iniciados por `0` (`010`) são recusados como erro léxico em vez de interpretados como octal.

### Construções aceitas cujo resultado difere do `g++`

- **Literais de ponto flutuante têm precisão de `float` (32 bits):** em C++ um literal como `1.0` é `double`, mas neste compilador é armazenado e dobrado pelo *constant folding* em `float` de 32 bits, e casa diretamente com parâmetros `float` na resolução de sobrecarga.
- **Colisão de nomes no C99 gerado:** o compilador não reserva identificadores da biblioteca padrão ou exclusividades do C99. Declarar variáveis ou funções com nomes como `printf`, `scanf`, `strcmp`, símbolos de `<stdio.h>` (como `remove`), palavras-chave exclusivas do C99 (como `restrict`) ou nomes que coincidam com o sufixo gerado pelo *name mangling* de uma função sobrecarregada (`f_i` ao lado de `f(int)`) pode produzir código C que o `gcc` rejeita.
