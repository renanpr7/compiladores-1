%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"
#include "diagnostics.h"

int lexer_line = 1;
int lexer_column = 1;
int lexical_errors = 0;
const char *lexer_lexeme = NULL;

static int token_line;
static int token_column;
static int string_line;
static int string_column;
static int comment_line;
static int comment_column;
static char *string_buffer;
static size_t string_length;
static size_t string_capacity;

static void advance_position(const char *text, int length) {
    for (int i = 0; i < length; ++i) {
        if (text[i] == '\n') {
            ++lexer_line;
            lexer_column = 1;
        } else {
            ++lexer_column;
        }
    }
}

static void append_string(const char *text, int length) {
    size_t needed = string_length + (size_t) length + 1;
    if (needed > string_capacity) {
        size_t capacity = string_capacity ? string_capacity : 32;
        while (capacity < needed) capacity *= 2;
        char *grown = realloc(string_buffer, capacity);
        if (!grown) {
            fprintf(stderr, "Erro lexico: memoria insuficiente na linha %d, coluna %d\n",
                    string_line, string_column);
            exit(EXIT_FAILURE);
        }
        string_buffer = grown;
        string_capacity = capacity;
    }
    memcpy(string_buffer + string_length, text, (size_t) length);
    string_length += (size_t) length;
    string_buffer[string_length] = '\0';
}

static void begin_string(void) {
    string_line = token_line;
    string_column = token_column;
    string_length = 0;
    append_string(yytext, yyleng);
}

static void report_lexical_error(const char *detail, int line, int column) {
    if (report_diagnostic()) {
        fprintf(stderr, "Erro lexico: %s na linha %d, coluna %d\n", detail, line, column);
    }
    ++lexical_errors;
}

#define YY_USER_ACTION \
    do { \
        token_line = lexer_line; \
        token_column = lexer_column; \
        lexer_lexeme = yytext; \
        yylloc.first_line = yylloc.last_line = token_line; \
        yylloc.first_column = yylloc.last_column = token_column; \
        advance_position(yytext, yyleng); \
    } while (0);
%}

%option noyywrap noinput nounput
%x STRING_STATE COMMENT_STATE

%%

[ \t\r]*\#[^\n]*             { }
[ \t\r]*using[ \t]+namespace[ \t]+std[ \t]*; { }
std::                           { }
\/\/[^\n]*                     { }
\/\*                            { comment_line = token_line; comment_column = token_column; BEGIN(COMMENT_STATE); }

<COMMENT_STATE>[^*\n]+          { }
<COMMENT_STATE>\*+[^*/\n]*      { }
<COMMENT_STATE>\n                { }
<COMMENT_STATE>\*+\/            { BEGIN(INITIAL); }
<COMMENT_STATE><<EOF>>           {
    report_lexical_error("comentario nao fechado", comment_line, comment_column);
    BEGIN(INITIAL);
    return 0;
}

\"                              { BEGIN(STRING_STATE); begin_string(); }
<STRING_STATE>[^\\\"\n]+        { append_string(yytext, yyleng); }
<STRING_STATE>\\.               { append_string(yytext, yyleng); }
<STRING_STATE>\"                 {
    append_string(yytext, yyleng);
    BEGIN(INITIAL);
    yylval.string_value = strdup(string_buffer);
    free(string_buffer);
    string_buffer = NULL;
    string_capacity = string_length = 0;
    lexer_lexeme = yylval.string_value;
    yylloc.first_line = string_line;
    yylloc.first_column = string_column;
    return STRING_LIT;
}
<STRING_STATE>\n                 {
    report_lexical_error("string nao fechada", string_line, string_column);
    free(string_buffer);
    string_buffer = NULL;
    string_capacity = string_length = 0;
    BEGIN(INITIAL);
}
<STRING_STATE><<EOF>>             {
    report_lexical_error("string nao fechada", string_line, string_column);
    free(string_buffer);
    string_buffer = NULL;
    string_capacity = string_length = 0;
    BEGIN(INITIAL);
    return 0;
}

([0-9]+\.[0-9]*\.[0-9]+)|([0-9]+\.[0-9]+\.[0-9]*)|[0-9]+\. {
    char detail[256];
    snprintf(detail, sizeof detail, "numero malformado '%s'", yytext);
    report_lexical_error(detail, token_line, token_column);
}
[0-9]+[A-Za-z_][A-Za-z0-9_]* {
    char detail[256];
    snprintf(detail, sizeof detail, "numero malformado '%s'", yytext);
    report_lexical_error(detail, token_line, token_column);
}
[0-9]+\.[0-9]+                 { yylval.float_value = (float) atof(yytext); return NUM_FLOAT; }
[0-9]+                          { yylval.int_value = atoi(yytext); return NUM_INT; }

"int"                           { return INT; }
"float"                         { return FLOAT; }
"bool"                          { return BOOL; }
"string"                        { return STRING; }
"void"                          { return VOID; }
"const"                         { return CONST; }
"true"                          { return TRUE; }
"false"                         { return FALSE; }
"if"                            { return IF; }
"else"                          { return ELSE; }
"while"                         { return WHILE; }
"for"                           { return FOR; }
"do"                            { return DO; }
"break"                         { return BREAK; }
"continue"                      { return CONTINUE; }
"return"                        { return RETURN; }
"cout"                          { return COUT; }
"cin"                           { return CIN; }
"endl"                          { return ENDL; }

"class"                         { return CLASS; }
"new"                           { return NEW; }
"delete"                        { return DELETE; }
"template"                      { return TEMPLATE; }
"virtual"                       { return VIRTUAL; }
"override"                      { return OVERRIDE; }
"public"                        { return PUBLIC; }
"private"                       { return PRIVATE; }
"protected"                     { return PROTECTED; }
"namespace"                     { return NAMESPACE; }
"using"                         { return USING; }
"this"                          { return THIS; }
"nullptr"                       { return NULLPTR; }
"friend"                        { return FRIEND; }
"operator"                      { return OPERATOR; }
"struct"                        { return STRUCT; }
"switch"                        { return SWITCH; }
"case"                          { return CASE; }
"default"                       { return DEFAULT; }

[A-Za-z_][A-Za-z0-9_]*           { yylval.identifier = strdup(yytext); return IDENT; }

"++"                            { return INC; }
"--"                            { return DEC; }
"+="                            { return PLUS_ASSIGN; }
"-="                            { return MINUS_ASSIGN; }
"*="                            { return TIMES_ASSIGN; }
"/="                            { return DIVIDE_ASSIGN; }
"=="                            { return EQ; }
"!="                            { return NE; }
"<="                            { return LE; }
">="                            { return GE; }
"&&"                            { return AND; }
"||"                            { return OR; }
"<<"                            { return LSHIFT; }
">>"                            { return RSHIFT; }

"+"                             { return PLUS; }
"-"                             { return MINUS; }
"*"                             { return TIMES; }
"/"                             { return DIVIDE; }
"%"                             { return MOD; }
"<"                             { return LT; }
">"                             { return GT; }
"!"                             { return NOT; }
"("                             { return LPAREN; }
")"                             { return RPAREN; }
"{"                             { return LBRACE; }
"}"                             { return RBRACE; }
"["                             { return LBRACKET; }
"]"                             { return RBRACKET; }
";"                             { return SEMI; }
","                             { return COMMA; }
"?"                             { return QUESTION; }
":"                             { return COLON; }
"="                             { return ASSIGN; }
"&"                             { return AMP; }

[ \t\r\n]+                     { }
.                               {
    char detail[64];
    snprintf(detail, sizeof detail, "caractere invalido '%s'", yytext);
    report_lexical_error(detail, token_line, token_column);
}
<<EOF>>                          { return 0; }

%%
