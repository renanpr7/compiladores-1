#ifndef OPTIMIZE_H
#define OPTIMIZE_H

#include "ast.h"

/* Simplifica expressoes constantes na AST em percurso pos-ordem (constant folding). */
NoAST *otimizar_ast(NoAST *raiz);

#endif
