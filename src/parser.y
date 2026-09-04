%{
#include <stdio.h>
#include <stdlib.h>

void yyerror(const char *s);
extern int yylex(void);
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
%expect 0

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
;

tipo_variavel:
    INT
  | FLOAT
  | BOOL
  | STRING
;

declaracao_variavel:
    tipo_variavel lista_declaradores SEMI
  | CONST tipo_variavel const_declaradores SEMI
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
    {
        yyerror("array de duas dimensoes nao suportado");
        YYABORT;
    }
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
  | RETURN SEMI
  | RETURN expressao SEMI
  | cmd_cout
  | cmd_cin
  | expressao SEMI
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
  | primario INC
      { yyerror("incremento/decremento nao permitido dentro de expressao"); YYERROR; }
  | primario DEC
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
    fprintf(stderr, "Erro de sintaxe na linha %d, coluna %d: %s\n",
            yylloc.first_line, yylloc.first_column, s);
}
