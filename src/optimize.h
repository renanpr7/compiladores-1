#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include "ast.h"

/* Simplifica expressoes constantes na AST em percurso pos-ordem (constant folding).
   Retorna a quantidade de erros detectados durante a otimizacao (ex: divisao por zero). */
int otimizar_ast(NoAST *raiz);

#endif
