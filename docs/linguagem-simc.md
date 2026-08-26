# Linguagem SimC — Especificação v1.0

## 1. Nome Oficial

**SimC** — Linguagem de programação de propósito geral para o curso de Compiladores 1 (Engenharia de Software, UnB).

> **Decisão:** O nome oficial adotado é "SimC". A escolha reflete a simplicidade da linguagem (sim = simples) e sua natureza baseada em C.

## 2. Tabela 1 — Tokens

| # | Token | Descrição | Exemplo |
|---|-------|-----------|---------|
| 1 | `KW_INT` | Palavra reservada: tipo inteiro | `int` |
| 2 | `KW_FLOAT` | Palavra reservada: tipo ponto flutuante | `float` |
| 3 | `KW_RETURN` | Palavra reservada: retorno de função | `return` |
| 4 | `KW_IF` | Palavra reservada: condicional | `if` |
| 5 | `KW_ELSE` | Palavra reservada: ramo alternativo | `else` |
| 6 | `KW_WHILE` | Palavra reservada: loop while | `while` |
| 7 | `KW_FOR` | Palavra reservada: loop for | `for` |
| 8 | `KW_PRINT` | Palavra reservada: saída na tela | `print` |
| 9 | `KW_STRING` | Palavra reservada: tipo string | `string` |
| 10 | `IDENT` | Identificador (letra seguida de letras/dígitos/_ ) | `minha_var`, `x`, `_cont` |
| 11 | `NUMBER_INT` | Literal numérico inteiro | `42`, `0`, `123` |
| 12 | `NUMBER_FLOAT` | Literal numérico com ponto decimal | `3.14`, `0.5`, `1.0` |
| 13 | `STRING_LITERAL` | Literal de string entre aspas duplas | `"hello world"` |
| 14 | `OP_ASSIGN` | Operador de atribuição | `=` |
| 15 | `OP_PLUS` | Operador de soma | `+` |
| 16 | `OP_MINUS` | Operador de subtração | `-` |
| 17 | `OP_MULT` | Operador de multiplicação | `*` |
| 18 | `OP_DIV` | Operador de divisão | `/` |
| 19 | `OP_MOD` | Operador de módulo | `%` |
| 20 | `OP_EQ` | Operador de igualdade | `==` |
| 21 | `OP_NEQ` | Operador de desigualdade | `!=` |
| 22 | `OP_LT` | Operador menor que | `<` |
| 23 | `OP_GT` | Operador maior que | `>` |
| 24 | `OP_LE` | Operador menor ou igual | `<=` |
| 25 | `OP_GE` | Operador maior ou igual | `>=` |
| 26 | `OP_AND` | Operador lógico E | `&&` |
| 27 | `OP_OR` | Operador lógico OU | `\|\|` |
| 28 | `OP_NOT` | Operador lógico NÃO | `!` |
| 29 | `LPAREN` | Parêntese de abertura | `(` |
| 30 | `RPAREN` | Parêntese de fechamento | `)` |
| 31 | `LBRACE` | Chave de abertura | `{` |
| 32 | `RBRACE` | Chave de fechamento | `}` |
| 33 | `SEMICOLON` | Ponto e vírgula | `;` |
| 34 | `COMMA` | Vírgula | `,` |
| 35 | `COMMENT_SINGLE` | Comentário de linha (`//`) | `// isso é um comentário` |
| 36 | `COMMENT_MULTI` | Comentário de bloco (`/* */`) | `/* comentário */` |
| 37 | `LBRACKET` | Colchete de abertura | `[` |
| 38 | `RBRACKET` | Colchete de fechamento | `]` |

### 2.1 Regras dos Tokens

- **IDENT**: Começa com letra (a-z, A-Z) ou `_`, seguido de letras, dígitos ou `_`. Não pode ser palavra reservada.
- **NUMBER_INT**: Sequência de dígitos (0-9). O sinal (+/-) é tratado como operador unário.
- **NUMBER_FLOAT**: Dois inteiros separados por `.`. O sinal (+/-) é tratado como operador unário.
- **STRING_LITERAL**: Texto entre aspas duplas. Suporta escape: `\"` para aspa, `\\` para barra, `\n` para nova linha.
- **Comentários**: Removidos pelo lexer (não geram tokens).
  - `//` — do `//` até o fim da linha.
  - `/* ... */` — pode ser multi-linha.
  - Comentários aninhados não são suportados.

Os tokens `KW_STRING`, `KW_FOR`, `KW_RETURN`, `OP_AND`, `OP_OR`, `LBRACKET` e
`RBRACKET` são reconhecidos pelo lexer, embora não sejam consumidos pela gramática v1.

## 3. Gramática BNF v1.0

```bnf
<programa>      ::= <comando>*

<comando>       ::= <declaracao>
                   | <atribuicao>
                   | <comando_if>
                   | <comando_while>
                   | <comando_for>
                   | <comando_print>
                   | <comando_return>
                   | <bloco>

<bloco>         ::= '{' <comando>* '}'

<declaracao>    ::= <tipo> IDENT '=' <expressao> ';'
                   | <tipo> IDENT ';'

<atribuicao>    ::= IDENT '=' <expressao> ';'

<comando_if>    ::= KW_IF '(' <expressao> ')' <bloco>
                   | KW_IF '(' <expressao> ')' <bloco> KW_ELSE <bloco>

<comando_while> ::= KW_WHILE '(' <expressao> ')' <bloco>

<comando_for>   ::= KW_FOR '(' <atribuicao> <expressao> ';' <atribuicao> ')' <bloco>

<comando_print> ::= KW_PRINT '(' <expressao> ')' ';'

<comando_return>::= KW_RETURN <expressao> ';'

<tipo>          ::= KW_INT | KW_FLOAT

<expressao>     ::= <expressao_or>

<expressao_or>  ::= <expressao_and> (OP_OR <expressao_and>)*

<expressao_and> ::= <expressao_not> (OP_AND <expressao_not>)*

<expressao_not> ::= OP_NOT <expressao_not> | <expressao_comp>

<expressao_comp>::= <expressao_add> ((OP_EQ | OP_NEQ | OP_LT | OP_GT | OP_LE | OP_GE) <expressao_add>)*

<expressao_add> ::= <expressao_mul> ((OP_PLUS | OP_MINUS) <expressao_mul>)*

<expressao_mul> ::= <expressao_unario> ((OP_MULT | OP_DIV | OP_MOD) <expressao_unario>)*

<expressao_unario>::= OP_MINUS <expressao_unario> | <expressao_primaria>

<expressao_primaria>::= NUMBER_INT
                       | NUMBER_FLOAT
                       | STRING_LITERAL
                       | IDENT
                       | IDENT '(' <lista_args> ')'
                       | '(' <expressao> ')'

<lista_args>    ::= <expressao> (',' <expressao>)*
                   | /* vazio */
```

### 3.1 Precedência de Operadores (maior para menor)

| Precedência | Operadores | Associatividade |
|-------------|------------|-----------------|
| 1 (maior) | `!`, unary `-` | Direita para esquerda |
| 2 | `*`, `/`, `%` | Esquerda para direita |
| 3 | `+`, `-` | Esquerda para direita |
| 4 | `<`, `>`, `<=`, `>=` | Esquerda para direita |
| 5 | `==`, `!=` | Esquerda para direita |
| 6 | `&&` | Esquerda para direita |
| 7 (menor) | `\|\|` | Esquerda para direita |

## 4. Exemplos

### 4.1 Exemplo Válido — Hello World

```c
// Hello World em SimC
print("Hello, World!");
```

**Saída esperada:**
```
Hello, World!
```

### 4.2 Exemplo Válido — Cálculo de Média

```c
/* Calcula a media de dois numeros */
int a = 10;
int b = 20;
float media = (a + b) / 2;
print(media);
```

**Saída esperada:**
```
15
```

### 4.3 Exemplo com Erro — Divisão por Zero (erro em tempo de execução)

```c
int x = 10;
int y = 0;
int resultado = x / y;
print(resultado);
```

> **Nota:** Este programa possui erro semântico (divisão por zero). O compilador v1.0 não detecta isso — será tratado na fase de análise semântica.

### 4.4 Exemplo com Erro de Sintaxe

```c
int x = 10  // erro: falta o ponto e vírgula
print(x);
```

> **Erro esperado:** `Erro de sintaxe: syntax error` (falta `;` após a declaração).

## 5. Convenções da Linguagem

### 5.1 Strings com Escape

| Sequência | Significado |
|-----------|-------------|
| `\"` | Aspas duplas |
| `\\` | Barra invertida |
| `\n` | Nova linha |
| `\t` | Tabulação |

### 5.2 Comentários

- **Simples:** `// texto até o fim da linha`
- **Bloco:** `/* texto que pode ser multi-linha */`
- Comentários são removidos pelo lexer e não geram tokens.

### 5.3 Tipos (v1.0)

| Tipo | Tamanho | Valores |
|------|---------|---------|
| `int` | 32 bits | Inteiro com sinal |
| `float` | 32 bits | Ponto flutuante |

> **Nota:** Tipos `char`, `string` e arrays serão adicionados em versões futuras.

### 5.4 Nomenclatura

- Variáveis: `snake_case` (ex: `minha_variavel`)
- Funções: `snake_case` (ex: `calcula_media`)
- Constantes: `UPPER_SNAKE_CASE` (ex: `MAX_SIZE`) —尚未implementado
