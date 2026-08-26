%{
#include <stdio.h>
#include "parser.tab.h"

int yylcol = 1;
int lexer_token_line = 1;
int lexer_token_column = 1;
int lexer_has_errors = 0;
const char *lexer_token_name = NULL;

static int comment_line;
static int comment_column;

static void advance_position(const char *text, int length) {
    int index;

    for (index = 0; index < length; index++) {
        if (text[index] == '\n') {
            yylineno++;
            yylcol = 1;
        } else {
            yylcol++;
        }
    }
}

static void lexer_error(const char *message) {
    fprintf(stderr, "erro: %s na linha %d, coluna %d\n",
            message, lexer_token_line, lexer_token_column);
    lexer_has_errors = 1;
}

#define YY_USER_ACTION do { \
    lexer_token_line = yylineno; \
    lexer_token_column = yylcol; \
    advance_position(yytext, yyleng); \
} while (0);

#define RETURN_TOKEN(token) do { \
    lexer_token_name = #token; \
    return token; \
} while (0)
%}

%option noyywrap noinput nounput
%x COMMENT

%%

"int"           { RETURN_TOKEN(KW_INT); }
"float"         { RETURN_TOKEN(KW_FLOAT); }
"string"        { RETURN_TOKEN(KW_STRING); }
"print"         { RETURN_TOKEN(KW_PRINT); }
"if"            { RETURN_TOKEN(KW_IF); }
"else"          { RETURN_TOKEN(KW_ELSE); }
"while"         { RETURN_TOKEN(KW_WHILE); }
"for"           { RETURN_TOKEN(KW_FOR); }
"return"        { RETURN_TOKEN(KW_RETURN); }

"//"[^\r\n]*   { /* ignora comentário de linha */ }
"/*"            {
                    comment_line = lexer_token_line;
                    comment_column = lexer_token_column;
                    BEGIN(COMMENT);
                }
<COMMENT>"*/"   { BEGIN(INITIAL); }
<COMMENT>\n     { }
<COMMENT>.      { }
<COMMENT><<EOF>> {
                    fprintf(stderr, "erro: comentário não fechado na linha %d, coluna %d\n",
                            comment_line, comment_column);
                    lexer_has_errors = 1;
                    BEGIN(INITIAL);
                    return EOF;
                }

\"([^\\\"\n]|\\[\"\\nt])*\" { RETURN_TOKEN(STRING_LITERAL); }
\"([^\\\"\n]|\\.)*              { lexer_error("string não fechada"); }

[0-9]+([eE][+-]?[0-9]+|\.[0-9]*[eE][+-]?[0-9]+) {
                    char message[128];
                    snprintf(message, sizeof(message), "número malformado '%s'", yytext);
                    lexer_error(message);
                }
[0-9]+\.       {
                    char message[128];
                    snprintf(message, sizeof(message), "número malformado '%s'", yytext);
                    lexer_error(message);
                }
[0-9]+[a-zA-Z_][a-zA-Z0-9_]* {
                    char message[128];
                    snprintf(message, sizeof(message), "identificador inválido '%s'", yytext);
                    lexer_error(message);
                }
[0-9]+\.[0-9]+ { RETURN_TOKEN(NUMBER_FLOAT); }
[0-9]+         { RETURN_TOKEN(NUMBER_INT); }
[a-zA-Z_][a-zA-Z0-9_]* { RETURN_TOKEN(IDENT); }

"==" { RETURN_TOKEN(OP_EQ); }
"!=" { RETURN_TOKEN(OP_NEQ); }
"<=" { RETURN_TOKEN(OP_LE); }
">=" { RETURN_TOKEN(OP_GE); }
"&&" { RETURN_TOKEN(OP_AND); }
"||" { RETURN_TOKEN(OP_OR); }
"="  { RETURN_TOKEN(OP_ASSIGN); }
"+"  { RETURN_TOKEN(OP_PLUS); }
"-"  { RETURN_TOKEN(OP_MINUS); }
"*"  { RETURN_TOKEN(OP_MULT); }
"/"  { RETURN_TOKEN(OP_DIV); }
"%"  { RETURN_TOKEN(OP_MOD); }
"<"  { RETURN_TOKEN(OP_LT); }
">"  { RETURN_TOKEN(OP_GT); }
"!"  { RETURN_TOKEN(OP_NOT); }
"("  { RETURN_TOKEN(LPAREN); }
")"  { RETURN_TOKEN(RPAREN); }
"{"  { RETURN_TOKEN(LBRACE); }
"}"  { RETURN_TOKEN(RBRACE); }
"["  { RETURN_TOKEN(LBRACKET); }
"]"  { RETURN_TOKEN(RBRACKET); }
";"  { RETURN_TOKEN(SEMICOLON); }
","  { RETURN_TOKEN(COMMA); }

[ \t\r]+ { }
\n        { }
.         {
              char message[128];
              snprintf(message, sizeof(message), "caractere inválido '%s'", yytext);
              lexer_error(message);
          }
<<EOF>> { return EOF; }

%%
