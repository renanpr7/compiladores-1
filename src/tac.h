#ifndef TAC_H
#define TAC_H

#include <stddef.h>
#include <stdio.h>
#include "tipos.h"

typedef enum {
    TAC_FUNC_BEGIN,
    TAC_FUNC_END,
    TAC_LABEL,
    TAC_GOTO,
    TAC_IFZ,
    TAC_COPY,
    TAC_BINOP,
    TAC_UNOP,
    TAC_ARR_LOAD,
    TAC_ARR_STORE,
    TAC_PARAM,
    TAC_CALL,
    TAC_RETURN,
    TAC_PRINT,
    TAC_READ
} TipoInstrucaoTAC;

typedef struct {
    TipoInstrucaoTAC tipo;
    char *op;
    char *arg1;
    char *arg2;
    char *result;
} InstrucaoTAC;

typedef struct {
    InstrucaoTAC *instrucoes;
    size_t quantidade;
    size_t capacidade;
    int proximo_temp;
    int proximo_label;
} ProgramaTAC;

ProgramaTAC *criar_programa_tac(void);
char *novo_temporario_tac(ProgramaTAC *prog);
char *novo_label_tac(ProgramaTAC *prog);
int emitir_instrucao_tac(ProgramaTAC *prog, TipoInstrucaoTAC tipo,
                         const char *op, const char *arg1,
                         const char *arg2, const char *result);
ProgramaTAC *gerar_tac(const NoAST *raiz);
void imprimir_tac(const ProgramaTAC *prog, FILE *saida);
void liberar_tac(ProgramaTAC *prog);

#endif
