%{
#include <stdio.h>
#include <stdlib.h>

void yyerror(const char *s);
extern int yylex(void);
%}

%token IDENT
%token NUMBER_INT
%token NUMBER_FLOAT
%token STRING_LITERAL

%token KW_INT
%token KW_FLOAT
%token KW_STRING
%token KW_RETURN
%token KW_IF
%token KW_ELSE
%token KW_WHILE
%token KW_FOR
%token KW_PRINT

%expect 0

%%

programa:
    /* vazio */
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro de sintaxe: %s\n", s);
}
