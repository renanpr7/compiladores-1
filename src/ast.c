#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

static void sem_memoria(void) {
    fprintf(stderr, "Erro: memoria insuficiente ao construir a AST\n");
    exit(EXIT_FAILURE);
}

static NoAST *novo_no(TipoNo tipo, int linha, int coluna) {
    NoAST *no = calloc(1, sizeof *no);
    if (!no) sem_memoria();
    no->tipo = tipo;
    no->linha = linha;
    no->coluna = coluna;
    return no;
}

static NoAST *com_filhos(TipoNo tipo, int linha, int coluna,
                         NoAST *f0, NoAST *f1, NoAST *f2, NoAST *f3) {
    NoAST *no = novo_no(tipo, linha, coluna);
    no->filho[0] = f0;
    no->filho[1] = f1;
    no->filho[2] = f2;
    no->filho[3] = f3;
    return no;
}

static char *copiar(const char *texto) {
    char *copia = strdup(texto);
    if (!copia) sem_memoria();
    return copia;
}

NoAST *criarNoPrograma(NoAST *declaracoes) {
    return com_filhos(NO_PROGRAMA, 1, 1, declaracoes, NULL, NULL, NULL);
}

NoAST *criarNoFuncao(TipoDado retorno, const char *nome, NoAST *parametros, NoAST *corpo,
                     int linha, int coluna) {
    NoAST *no = com_filhos(NO_FUNCAO, linha, coluna, parametros, corpo, NULL, NULL);
    no->tipo_dado = retorno;
    no->texto = copiar(nome);
    return no;
}

NoAST *criarNoParam(TipoDado tipo, const char *nome, int linha, int coluna) {
    NoAST *no = novo_no(NO_PARAMETRO, linha, coluna);
    no->tipo_dado = tipo;
    no->texto = copiar(nome);
    return no;
}

NoAST *criarNoParamRef(TipoDado tipo, const char *nome, int linha, int coluna) {
    NoAST *no = criarNoParam(tipo, nome, linha, coluna);
    no->eh_referencia = 1;
    return no;
}

NoAST *criarNoParamPadrao(TipoDado tipo, const char *nome, NoAST *padrao, int linha, int coluna) {
    NoAST *no = criarNoParam(tipo, nome, linha, coluna);
    no->filho[0] = padrao;
    return no;
}

NoAST *criarNoDeclaracao(TipoDado tipo, int eh_const, NoAST *declaradores, int linha, int coluna) {
    NoAST *no = com_filhos(NO_DECLARACAO, linha, coluna, declaradores, NULL, NULL, NULL);
    no->tipo_dado = tipo;
    no->eh_const = eh_const;
    return no;
}

NoAST *criarNoDeclarador(const char *nome, NoAST *inicial, NoAST *tamanho, int linha, int coluna) {
    NoAST *no = com_filhos(NO_DECLARADOR, linha, coluna, inicial, tamanho, NULL, NULL);
    no->texto = copiar(nome);
    return no;
}

NoAST *criarNoBloco(NoAST *comandos, int linha, int coluna) {
    return com_filhos(NO_BLOCO, linha, coluna, comandos, NULL, NULL, NULL);
}

NoAST *criarNoIf(NoAST *condicao, NoAST *entao, NoAST *senao, int linha, int coluna) {
    return com_filhos(NO_IF, linha, coluna, condicao, entao, senao, NULL);
}

NoAST *criarNoWhile(NoAST *condicao, NoAST *corpo, int linha, int coluna) {
    return com_filhos(NO_WHILE, linha, coluna, condicao, corpo, NULL, NULL);
}

NoAST *criarNoDoWhile(NoAST *corpo, NoAST *condicao, int linha, int coluna) {
    return com_filhos(NO_DO_WHILE, linha, coluna, corpo, condicao, NULL, NULL);
}

NoAST *criarNoFor(NoAST *inicio, NoAST *condicao, NoAST *passo, NoAST *corpo,
                  int linha, int coluna) {
    return com_filhos(NO_FOR, linha, coluna, inicio, condicao, passo, corpo);
}

NoAST *criarNoReturn(NoAST *expressao, int linha, int coluna) {
    return com_filhos(NO_RETURN, linha, coluna, expressao, NULL, NULL, NULL);
}

NoAST *criarNoBreak(int linha, int coluna) {
    return novo_no(NO_BREAK, linha, coluna);
}

NoAST *criarNoContinue(int linha, int coluna) {
    return novo_no(NO_CONTINUE, linha, coluna);
}

NoAST *criarNoCout(NoAST *itens, int linha, int coluna) {
    return com_filhos(NO_COUT, linha, coluna, itens, NULL, NULL, NULL);
}

NoAST *criarNoCin(NoAST *alvos, int linha, int coluna) {
    return com_filhos(NO_CIN, linha, coluna, alvos, NULL, NULL, NULL);
}

NoAST *criarNoEndl(int linha, int coluna) {
    return novo_no(NO_ENDL, linha, coluna);
}

NoAST *criarNoIncDec(Operador operador, NoAST *alvo, int linha, int coluna) {
    NoAST *no = com_filhos(NO_INC_DEC, linha, coluna, alvo, NULL, NULL, NULL);
    no->operador = operador;
    return no;
}

NoAST *criarNoAtribuicao(Operador operador, NoAST *alvo, NoAST *valor, int linha, int coluna) {
    NoAST *no = com_filhos(NO_ATRIBUICAO, linha, coluna, alvo, valor, NULL, NULL);
    no->operador = operador;
    return no;
}

NoAST *criarNoOp(Operador operador, NoAST *esq, NoAST *dir, int linha, int coluna) {
    NoAST *no = com_filhos(NO_OP_BINARIO, linha, coluna, esq, dir, NULL, NULL);
    no->operador = operador;
    return no;
}

NoAST *criarNoOpUnario(Operador operador, NoAST *operando, int linha, int coluna) {
    NoAST *no = com_filhos(NO_OP_UNARIO, linha, coluna, operando, NULL, NULL, NULL);
    no->operador = operador;
    return no;
}

NoAST *criarNoTernario(NoAST *condicao, NoAST *se_verdadeiro, NoAST *se_falso,
                       int linha, int coluna) {
    return com_filhos(NO_TERNARIO, linha, coluna, condicao, se_verdadeiro, se_falso, NULL);
}

NoAST *criarNoChamada(const char *nome, NoAST *argumentos, int linha, int coluna) {
    NoAST *no = com_filhos(NO_CHAMADA, linha, coluna, argumentos, NULL, NULL, NULL);
    no->texto = copiar(nome);
    return no;
}

NoAST *criarNoIndice(const char *nome, NoAST *indice, int linha, int coluna) {
    NoAST *no = com_filhos(NO_INDICE, linha, coluna, indice, NULL, NULL, NULL);
    no->texto = copiar(nome);
    return no;
}

NoAST *criarNoId(const char *nome, int linha, int coluna) {
    NoAST *no = novo_no(NO_ID, linha, coluna);
    no->texto = copiar(nome);
    return no;
}

NoAST *criarNoNum(int valor, int linha, int coluna) {
    NoAST *no = novo_no(NO_NUM, linha, coluna);
    no->valor_int = valor;
    return no;
}

NoAST *criarNoFloat(float valor, int linha, int coluna) {
    NoAST *no = novo_no(NO_FLOAT, linha, coluna);
    no->valor_float = valor;
    return no;
}

NoAST *criarNoBool(int valor, int linha, int coluna) {
    NoAST *no = novo_no(NO_BOOL, linha, coluna);
    no->valor_int = valor != 0;
    return no;
}

NoAST *criarNoString(const char *literal, int linha, int coluna) {
    NoAST *no = novo_no(NO_STRING, linha, coluna);
    no->texto = copiar(literal);
    return no;
}

NoAST *anexarIrmao(NoAST *lista, NoAST *novo) {
    if (!lista) return novo;
    NoAST *ultimo = lista;
    while (ultimo->proximo) ultimo = ultimo->proximo;
    ultimo->proximo = novo;
    return lista;
}

/* Sem default: -Wswitch avisa se um TipoNo novo ficar sem nome. */
static const char *nome_no(TipoNo tipo) {
    switch (tipo) {
#define NOME(t) case NO_##t: return #t
        NOME(PROGRAMA); NOME(FUNCAO); NOME(PARAMETRO); NOME(DECLARACAO); NOME(DECLARADOR);
        NOME(BLOCO); NOME(IF); NOME(WHILE); NOME(DO_WHILE); NOME(FOR); NOME(RETURN);
        NOME(BREAK); NOME(CONTINUE); NOME(COUT); NOME(CIN); NOME(ENDL); NOME(INC_DEC);
        NOME(ATRIBUICAO); NOME(OP_BINARIO); NOME(OP_UNARIO); NOME(TERNARIO); NOME(CHAMADA);
        NOME(INDICE); NOME(ID); NOME(NUM); NOME(FLOAT); NOME(BOOL); NOME(STRING);
#undef NOME
    }
    return "?";
}

static const char *nome_tipo(TipoDado tipo) {
    switch (tipo) {
    case TIPO_NENHUM: return "";
    case TIPO_INT:    return "int";
    case TIPO_FLOAT:  return "float";
    case TIPO_BOOL:   return "bool";
    case TIPO_STRING: return "string";
    case TIPO_VOID:   return "void";
    }
    return "?";
}

static const char *simbolo(Operador operador) {
    switch (operador) {
    case OP_NENHUM:       return "";
    case OP_SOMA:         return "+";
    case OP_SUB:          return "-";
    case OP_MULT:         return "*";
    case OP_DIV:          return "/";
    case OP_MOD:          return "%";
    case OP_IGUAL:        return "==";
    case OP_DIFERENTE:    return "!=";
    case OP_MENOR:        return "<";
    case OP_MAIOR:        return ">";
    case OP_MENOR_IGUAL:  return "<=";
    case OP_MAIOR_IGUAL:  return ">=";
    case OP_E:            return "&&";
    case OP_OU:           return "||";
    case OP_NAO:          return "!";
    case OP_NEG:          return "-";
    case OP_ATRIB:        return "=";
    case OP_ATRIB_SOMA:   return "+=";
    case OP_ATRIB_SUB:    return "-=";
    case OP_ATRIB_MULT:   return "*=";
    case OP_ATRIB_DIV:    return "/=";
    case OP_INC:          return "++";
    case OP_DEC:          return "--";
    }
    return "?";
}

static void imprimir_lista(const NoAST *no, int nivel);

static void imprimir_no(const NoAST *no, int nivel) {
    printf("%*s%s", 2 * nivel, "", nome_no(no->tipo));
    switch (no->tipo) {
    case NO_FUNCAO:
        printf(" %s %s", nome_tipo(no->tipo_dado), no->texto);
        break;
    case NO_PARAMETRO:
        printf(" %s%s %s", nome_tipo(no->tipo_dado), no->eh_referencia ? "&" : "", no->texto);
        break;
    case NO_DECLARACAO:
        printf(" %s%s", no->eh_const ? "const " : "", nome_tipo(no->tipo_dado));
        break;
    case NO_DECLARADOR: case NO_CHAMADA: case NO_INDICE: case NO_ID: case NO_STRING:
        printf(" %s", no->texto);
        break;
    case NO_INC_DEC: case NO_ATRIBUICAO: case NO_OP_BINARIO: case NO_OP_UNARIO:
        printf(" %s", simbolo(no->operador));
        break;
    case NO_NUM:
        printf(" %d", no->valor_int);
        break;
    case NO_FLOAT:
        printf(" %g", no->valor_float);
        break;
    case NO_BOOL:
        printf(" %s", no->valor_int ? "true" : "false");
        break;
    default:
        break;
    }
    putchar('\n');

    /* Filho ausente antes de um presente vira "(vazio)", para a posicao nao se perder. */
    int ultimo = -1;
    for (int i = 0; i < AST_MAX_FILHOS; ++i)
        if (no->filho[i]) ultimo = i;
    for (int i = 0; i <= ultimo; ++i) {
        if (no->filho[i]) imprimir_lista(no->filho[i], nivel + 1);
        else printf("%*s(vazio)\n", 2 * (nivel + 1), "");
    }
}

static void imprimir_lista(const NoAST *no, int nivel) {
    for (; no; no = no->proximo)
        imprimir_no(no, nivel);
}

void imprimirAST(NoAST *raiz) {
    imprimir_lista(raiz, 0);
}

void liberarAST(NoAST *raiz) {
    while (raiz) {
        NoAST *proximo = raiz->proximo;
        for (int i = 0; i < AST_MAX_FILHOS; ++i)
            liberarAST(raiz->filho[i]);
        free(raiz->texto);
        free(raiz);
        raiz = proximo;
    }
}
