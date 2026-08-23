%{
#define _POSIX_C_SOURCE 200809L
#include "parser.tab.h"
%}

%option noyywrap noinput nounput

%%

.|\n    { /* ignore all characters for now */ }
<<EOF>> { return EOF; }

%%
