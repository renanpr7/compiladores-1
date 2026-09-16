%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "diagnostics.h"

int erros_compilacao = 0;
#define MAX_ERROS 10

void yyerror(const char *s);
extern int yylex(void);

static void erro_oo(const char *token, int linha, int coluna) {
    if (erros_compilacao >= MAX_ERROS) return;
    erros_compilacao++;
    if (report_diagnostic()) {
        fprintf(stderr, "Erro de compilacao: construcao OO nao suportada '%s' na linha %d, coluna %d\n",
                token, linha, coluna);
    }
}

static void erro_fora_escopo(const char *token, int linha, int coluna) {
    if (erros_compilacao >= MAX_ERROS) return;
    erros_compilacao++;
    if (report_diagnostic()) {
        fprintf(stderr, "Erro de compilacao: construcao fora do escopo '%s' na linha %d, coluna %d\n",
                token, linha, coluna);
    }
}
%}

%union {
    int int_value;
    float float_value;
    char *string_value;
    char *identifier;
    int expression_category;
}

%token <identifier> IDENT
%token <int_value> NUM_INT
%token <float_value> NUM_FLOAT
%token <string_value> STRING_LIT

%token INT FLOAT BOOL STRING VOID CONST TRUE FALSE
%token IF ELSE WHILE FOR DO BREAK CONTINUE RETURN
%token COUT CIN ENDL

%token PLUS MINUS TIMES DIVIDE MOD
%token INC DEC
%token PLUS_ASSIGN MINUS_ASSIGN TIMES_ASSIGN DIVIDE_ASSIGN
%token EQ NE LT GT LE GE
%token AND OR NOT

%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token SEMI COMMA QUESTION COLON ASSIGN
%token AMP
%token LSHIFT RSHIFT

%token CLASS NEW DELETE TEMPLATE VIRTUAL OVERRIDE PUBLIC PRIVATE PROTECTED
%token NAMESPACE USING THIS NULLPTR FRIEND OPERATOR
%token STRUCT SWITCH CASE DEFAULT

%start programa
%locations
%expect 1

%type <expression_category> expressao primario

/* Precedencia: da menor para a maior. */
%right  ASSIGN PLUS_ASSIGN MINUS_ASSIGN TIMES_ASSIGN DIVIDE_ASSIGN
%right  QUESTION COLON
%left   OR
%left   AND
%left   EQ NE
%left   LT GT LE GE
%left   PLUS MINUS
%left   TIMES DIVIDE MOD
%precedence NOT
%precedence UMINUS

%%

programa:
    lista_decl
;

lista_decl:
    %empty
  | lista_decl decl
;

decl:
    declaracao_variavel
  | tipo_variavel IDENT LPAREN params RPAREN bloco
  | VOID IDENT LPAREN params RPAREN bloco
  | erro_oo
  | erro_fora_escopo
  | error SEMI
;

tipo_variavel:
    INT
  | FLOAT
  | BOOL
  | STRING
;

decl_var_base:
    tipo_variavel lista_declaradores
  | CONST tipo_variavel const_declaradores
;

declaracao_variavel:
    decl_var_base SEMI
;

lista_declaradores:
    declarador
  | lista_declaradores COMMA declarador
;

declarador:
    IDENT
  | IDENT ASSIGN expressao
  | IDENT LBRACKET expressao RBRACKET
  | IDENT LBRACKET expressao RBRACKET LBRACKET expressao RBRACKET
      { yyerror("array de duas dimensoes nao suportado"); YYERROR; }
;

const_declaradores:
    const_declarador
  | const_declaradores COMMA const_declarador
;

const_declarador:
    IDENT ASSIGN expressao
;

params:
    %empty
  | VOID
  | lista_param
;

lista_param:
    lista_param_sem_default
  | lista_param_sem_default COMMA lista_param_default
  | lista_param_default
;

lista_param_sem_default:
    decl_param_sem_default
  | lista_param_sem_default COMMA decl_param_sem_default
;

lista_param_default:
    decl_param_default
  | lista_param_default COMMA decl_param_default
;

decl_param_sem_default:
    tipo_variavel IDENT
  | tipo_variavel AMP IDENT
;

decl_param_default:
    tipo_variavel IDENT ASSIGN expressao
;

bloco:
    LBRACE comandos RBRACE
;

comandos:
    %empty
  | comandos comando
;

comando:
    declaracao_variavel
  | bloco
  | RETURN SEMI
  | RETURN expressao SEMI
  | cmd_cout
  | cmd_cin
  | cmd_if
  | cmd_while
  | cmd_do_while
  | cmd_for
  | BREAK SEMI
  | CONTINUE SEMI
  | cmd_inc_dec
  | expressao SEMI
  | erro_oo
  | erro_fora_escopo
  | error SEMI
;

cmd_cout:
    COUT lista_cout SEMI
;

lista_cout:
    LSHIFT alvo_cout
  | lista_cout LSHIFT alvo_cout
;

alvo_cout:
    STRING_LIT
  | ENDL
  | IDENT
  | IDENT LBRACKET expressao RBRACKET
;

cmd_cin:
    CIN lista_cin SEMI
;

lista_cin:
    RSHIFT alvo_cin
  | lista_cin RSHIFT alvo_cin
;

alvo_cin:
    IDENT
  | IDENT LBRACKET expressao RBRACKET
;

cmd_if:
    IF LPAREN expressao RPAREN comando
  | IF LPAREN expressao RPAREN comando ELSE comando
;

cmd_while:
    WHILE LPAREN expressao RPAREN comando
;

cmd_do_while:
    DO comando WHILE LPAREN expressao RPAREN SEMI
;

cmd_for:
    FOR LPAREN for_init SEMI for_cond SEMI for_passo RPAREN comando
;

for_init:
    %empty
  | decl_var_base
  | expressao
;

for_cond:
    %empty
  | expressao
;

for_passo:
    %empty
  | expressao
  | primario INC
  | primario DEC
;

cmd_inc_dec:
    primario INC SEMI
  | primario DEC SEMI
;

erro_oo:
    CLASS   { erro_oo("class",    @1.first_line, @1.first_column); }
  | NEW     { erro_oo("new",      @1.first_line, @1.first_column); }
  | DELETE  { erro_oo("delete",   @1.first_line, @1.first_column); }
  | TEMPLATE  { erro_oo("template", @1.first_line, @1.first_column); }
  | VIRTUAL { erro_oo("virtual",  @1.first_line, @1.first_column); }
  | OVERRIDE { erro_oo("override", @1.first_line, @1.first_column); }
  | PUBLIC  { erro_oo("public",   @1.first_line, @1.first_column); }
  | PRIVATE { erro_oo("private",  @1.first_line, @1.first_column); }
  | PROTECTED { erro_oo("protected", @1.first_line, @1.first_column); }
  | NAMESPACE { erro_oo("namespace", @1.first_line, @1.first_column); }
  | USING   { erro_oo("using",    @1.first_line, @1.first_column); }
  | THIS    { erro_oo("this",     @1.first_line, @1.first_column); }
  | NULLPTR { erro_oo("nullptr",  @1.first_line, @1.first_column); }
  | FRIEND  { erro_oo("friend",   @1.first_line, @1.first_column); }
  | OPERATOR { erro_oo("operator", @1.first_line, @1.first_column); }
;

erro_fora_escopo:
    STRUCT  { erro_fora_escopo("struct",  @1.first_line, @1.first_column); }
  | SWITCH  { erro_fora_escopo("switch",  @1.first_line, @1.first_column); }
  | CASE    { erro_fora_escopo("case",    @1.first_line, @1.first_column); }
  | DEFAULT { erro_fora_escopo("default", @1.first_line, @1.first_column); }
;

expressao:
    expressao ASSIGN expressao %prec ASSIGN
      {
          if (!$1) {
              YYLTYPE location = yylloc;
              yylloc = @2;
              yyerror("syntax error");
              yylloc = location;
              YYERROR;
          }
          $$ = 0;
      }
  | expressao PLUS_ASSIGN expressao %prec PLUS_ASSIGN
      {
          if (!$1) {
              YYLTYPE location = yylloc;
              yylloc = @2;
              yyerror("syntax error");
              yylloc = location;
              YYERROR;
          }
          $$ = 0;
      }
  | expressao MINUS_ASSIGN expressao %prec MINUS_ASSIGN
      {
          if (!$1) {
              YYLTYPE location = yylloc;
              yylloc = @2;
              yyerror("syntax error");
              yylloc = location;
              YYERROR;
          }
          $$ = 0;
      }
  | expressao TIMES_ASSIGN expressao %prec TIMES_ASSIGN
      {
          if (!$1) {
              YYLTYPE location = yylloc;
              yylloc = @2;
              yyerror("syntax error");
              yylloc = location;
              YYERROR;
          }
          $$ = 0;
      }
  | expressao DIVIDE_ASSIGN expressao %prec DIVIDE_ASSIGN
      {
          if (!$1) {
              YYLTYPE location = yylloc;
              yylloc = @2;
              yyerror("syntax error");
              yylloc = location;
              YYERROR;
          }
          $$ = 0;
      }
  | expressao QUESTION expressao COLON expressao %prec COLON
      { $$ = 0; }
  | expressao OR expressao %prec OR
      { $$ = 0; }
  | expressao AND expressao %prec AND
      { $$ = 0; }
  | expressao EQ expressao %prec EQ
      { $$ = 0; }
  | expressao NE expressao %prec NE
      { $$ = 0; }
  | expressao LT expressao %prec LT
      { $$ = 0; }
  | expressao GT expressao %prec GT
      { $$ = 0; }
  | expressao LE expressao %prec LE
      { $$ = 0; }
  | expressao GE expressao %prec GE
      { $$ = 0; }
  | expressao PLUS expressao %prec PLUS
      { $$ = 0; }
  | expressao MINUS expressao %prec MINUS
      { $$ = 0; }
  | expressao TIMES expressao %prec TIMES
      { $$ = 0; }
  | expressao DIVIDE expressao %prec DIVIDE
      { $$ = 0; }
  | expressao MOD expressao %prec MOD
      { $$ = 0; }
  | MINUS expressao %prec UMINUS
      { $$ = 0; }
  | NOT expressao %prec NOT
      { $$ = 0; }
  | INC expressao %prec NOT
     { yyerror("incremento/decremento nao permitido dentro de expressao"); YYERROR; }
  | DEC expressao %prec NOT
     { yyerror("incremento/decremento nao permitido dentro de expressao"); YYERROR; }
  | primario
      { $$ = $1; }
;

primario:
    NUM_INT { $$ = 0; }
  | NUM_FLOAT { $$ = 0; }
  | STRING_LIT { $$ = 0; }
  | TRUE { $$ = 0; }
  | FALSE { $$ = 0; }
  | IDENT { $$ = 1; }
  | IDENT LBRACKET expressao RBRACKET
      { $$ = 1; }
  | IDENT LPAREN argumentos RPAREN
      { $$ = 0; }
  | LPAREN expressao RPAREN
      { $$ = 0; }
;

argumentos:
    %empty
  | lista_argumentos
;

lista_argumentos:
    expressao
  | lista_argumentos COMMA expressao
;

%%

void yyerror(const char *s) {
    if (erros_compilacao >= MAX_ERROS) return;
    erros_compilacao++;
    if (report_diagnostic()) {
        fprintf(stderr, "Erro de sintaxe na linha %d, coluna %d: %s\n",
                yylloc.first_line, yylloc.first_column, s);
    }
}
