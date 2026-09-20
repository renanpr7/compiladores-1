#ifndef TIPOS_H
#define TIPOS_H

#define AST_MAX_FILHOS 4

typedef enum {
    NO_PROGRAMA,
    NO_FUNCAO,
    NO_PARAMETRO,
    NO_DECLARACAO,
    NO_DECLARADOR,

    NO_BLOCO,
    NO_IF,
    NO_WHILE,
    NO_DO_WHILE,
    NO_FOR,
    NO_RETURN,
    NO_BREAK,
    NO_CONTINUE,
    NO_COUT,
    NO_CIN,
    NO_ENDL,
    NO_INC_DEC,

    NO_ATRIBUICAO,
    NO_OP_BINARIO,
    NO_OP_UNARIO,
    NO_TERNARIO,
    NO_CHAMADA,
    NO_INDICE,
    NO_ID,
    NO_NUM,
    NO_FLOAT,
    NO_BOOL,
    NO_STRING
} TipoNo;

typedef enum {
    TIPO_NENHUM,
    TIPO_INT,
    TIPO_FLOAT,
    TIPO_BOOL,
    TIPO_STRING,
    TIPO_VOID
} TipoDado;

typedef enum {
    OP_NENHUM,
    OP_SOMA, OP_SUB, OP_MULT, OP_DIV, OP_MOD,
    OP_IGUAL, OP_DIFERENTE, OP_MENOR, OP_MAIOR, OP_MENOR_IGUAL, OP_MAIOR_IGUAL,
    OP_E, OP_OU, OP_NAO, OP_NEG,
    OP_ATRIB, OP_ATRIB_SOMA, OP_ATRIB_SUB, OP_ATRIB_MULT, OP_ATRIB_DIV,
    OP_INC, OP_DEC
} Operador;

typedef struct NoAST {
    TipoNo tipo;
    TipoDado tipo_dado;
    Operador operador;
    int valor_int;
    float valor_float;
    char *texto;
    int eh_const;
    int eh_referencia;
    int eh_parenthesized;
    struct NoAST *filho[AST_MAX_FILHOS];
    struct NoAST *proximo;
    int linha;
    int coluna;
} NoAST;

#endif
