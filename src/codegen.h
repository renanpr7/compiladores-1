#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "ast.h"

/*
 * Gera codigo C99 a partir da raiz da AST e escreve no stream de saida.
 * Retorna 0 em caso de sucesso, ou valor diferente de zero em caso de erro.
 */
int gerar_codigo(const NoAST *raiz, FILE *saida);

#endif
