%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "diagnostics.h"

int erros_compilacao = 0;
#define MAX_ERROS 10

NoAST *ast_raiz = NULL;

void yyerror(const char *s);
extern int yylex(void);

static void erro_oo(const char *token, int linha, int coluna) {
    if (erros_compilacao >= MAX_ERROS) return;
    erros_compilacao++;
    if (report_diagnostic())
        fprintf(stderr, "Erro de compilacao: construcao OO nao suportada '%s' na linha %d, coluna %d\n", token, linha, coluna);
}

static void erro_fora_escopo(const char *token, int linha, int coluna) {
    if (erros_compilacao >= MAX_ERROS) return;
    erros_compilacao++;
    if (report_diagnostic())
        fprintf(stderr, "Erro de compilacao: construcao fora do escopo '%s' na linha %d, coluna %d\n", token, linha, coluna);
}

static int eh_alvo(const NoAST *no) {
    return no && !no->eh_parenthesized && (no->tipo == NO_ID || no->tipo == NO_INDICE);
}

static void erro_atribuicao_invalida(int linha, int coluna);

%}

%code requires {
#include "ast.h"
}

%union {
    int int_value;
    float float_value;
    char *string_value;
    char *identifier;
    int expression_category;
    TipoDado tipo;
    NoAST *no;
}

%token <identifier> IDENT
%token <int_value> NUM_INT
%token <float_value> NUM_FLOAT
%token <string_value> STRING_LIT
%token INT FLOAT BOOL STRING VOID CONST TRUE FALSE
%token IF ELSE WHILE FOR DO BREAK CONTINUE RETURN COUT CIN ENDL
%token PLUS MINUS TIMES DIVIDE MOD INC DEC
%token PLUS_ASSIGN MINUS_ASSIGN TIMES_ASSIGN DIVIDE_ASSIGN
%token EQ NE LT GT LE GE AND OR NOT
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET
%token SEMI COMMA QUESTION COLON ASSIGN AMP LSHIFT RSHIFT
%token CLASS NEW DELETE TEMPLATE VIRTUAL OVERRIDE PUBLIC PRIVATE PROTECTED
%token NAMESPACE USING THIS NULLPTR FRIEND OPERATOR STRUCT SWITCH CASE DEFAULT

%start programa
%locations
%expect 1

%type <no> programa lista_decl decl decl_var_base declaracao_variavel
%type <no> lista_declaradores declarador const_declaradores const_declarador
%type <no> params lista_param lista_param_sem_default lista_param_default
%type <no> decl_param_sem_default decl_param_default bloco comandos comando
%type <no> cmd_cout lista_cout alvo_cout cmd_cin lista_cin alvo_cin
%type <no> cmd_if cmd_while cmd_do_while cmd_for for_init for_cond for_passo
%type <no> cmd_inc_dec expressao primario argumentos lista_argumentos
%type <tipo> tipo_variavel
%type <no> erro_oo erro_fora_escopo

%destructor { free($$); } <identifier> <string_value>
/* A raiz aceita tambem passa pelo descarte da pilha Bison; ela fica para o
 * main liberar, portanto nao pode ser destruida duas vezes. */
%destructor { if ($$ != ast_raiz) liberarAST($$); } <no>

/* Precedencia: da menor para a maior. */
%right ASSIGN PLUS_ASSIGN MINUS_ASSIGN TIMES_ASSIGN DIVIDE_ASSIGN
%right QUESTION COLON
%left OR
%left AND
%left EQ NE
%left LT GT LE GE
%left PLUS MINUS
%left TIMES DIVIDE MOD
%precedence NOT
%precedence UMINUS

%%

programa:
    lista_decl { $$ = criarNoPrograma($1); ast_raiz = $$; }
;

lista_decl:
    %empty { $$ = NULL; }
  | lista_decl decl { $$ = anexarIrmao($1, $2); }
;

decl:
    declaracao_variavel { $$ = $1; }
  | tipo_variavel IDENT LPAREN params RPAREN bloco
      { $$ = criarNoFuncao($1, $2, $4, $6, @2.first_line, @2.first_column); free($2); }
  | VOID IDENT LPAREN params RPAREN bloco
      { $$ = criarNoFuncao(TIPO_VOID, $2, $4, $6, @2.first_line, @2.first_column); free($2); }
  | erro_oo { $$ = $1; }
  | erro_fora_escopo { $$ = $1; }
  | error SEMI { $$ = NULL; }
;

tipo_variavel:
    INT { $$ = TIPO_INT; }
  | FLOAT { $$ = TIPO_FLOAT; }
  | BOOL { $$ = TIPO_BOOL; }
  | STRING { $$ = TIPO_STRING; }
;

decl_var_base:
    tipo_variavel lista_declaradores
      { $$ = criarNoDeclaracao($1, 0, $2, @1.first_line, @1.first_column); }
  | CONST tipo_variavel const_declaradores
      { $$ = criarNoDeclaracao($2, 1, $3, @1.first_line, @1.first_column); }
;

declaracao_variavel:
    decl_var_base SEMI { $$ = $1; }
;

lista_declaradores:
    declarador { $$ = $1; }
  | lista_declaradores COMMA declarador { $$ = anexarIrmao($1, $3); }
;

declarador:
    IDENT { $$ = criarNoDeclarador($1, NULL, NULL, @1.first_line, @1.first_column); free($1); }
  | IDENT ASSIGN expressao
      { $$ = criarNoDeclarador($1, $3, NULL, @1.first_line, @1.first_column); free($1); }
  | IDENT LBRACKET expressao RBRACKET
      { $$ = criarNoDeclarador($1, NULL, $3, @1.first_line, @1.first_column); free($1); }
  | IDENT LBRACKET expressao RBRACKET LBRACKET expressao RBRACKET
      { (void)$1; (void)$3; (void)$6; $$ = NULL; yyerror("array de duas dimensoes nao suportado"); YYERROR; }
;

const_declaradores:
    const_declarador { $$ = $1; }
  | const_declaradores COMMA const_declarador { $$ = anexarIrmao($1, $3); }
;

const_declarador:
    IDENT ASSIGN expressao
      { $$ = criarNoDeclarador($1, $3, NULL, @1.first_line, @1.first_column); free($1); }
;

params:
    %empty { $$ = NULL; }
  | VOID { $$ = NULL; }
  | lista_param { $$ = $1; }
;

lista_param:
    lista_param_sem_default { $$ = $1; }
  | lista_param_sem_default COMMA lista_param_default { $$ = anexarIrmao($1, $3); }
  | lista_param_default { $$ = $1; }
;

lista_param_sem_default:
    decl_param_sem_default { $$ = $1; }
  | lista_param_sem_default COMMA decl_param_sem_default { $$ = anexarIrmao($1, $3); }
;

lista_param_default:
    decl_param_default { $$ = $1; }
  | lista_param_default COMMA decl_param_default { $$ = anexarIrmao($1, $3); }
;

decl_param_sem_default:
    tipo_variavel IDENT
      { $$ = criarNoParam($1, $2, @2.first_line, @2.first_column); free($2); }
  | tipo_variavel AMP IDENT
      { $$ = criarNoParamRef($1, $3, @3.first_line, @3.first_column); free($3); }
;

decl_param_default:
    tipo_variavel IDENT ASSIGN expressao
      { $$ = criarNoParamPadrao($1, $2, $4, @2.first_line, @2.first_column); free($2); }
;

bloco:
    LBRACE comandos RBRACE { $$ = criarNoBloco($2, @1.first_line, @1.first_column); }
;

comandos:
    %empty { $$ = NULL; }
  | comandos comando { $$ = anexarIrmao($1, $2); }
;

comando:
    declaracao_variavel { $$ = $1; }
  | bloco { $$ = $1; }
  | RETURN SEMI { $$ = criarNoReturn(NULL, @1.first_line, @1.first_column); }
  | RETURN expressao SEMI { $$ = criarNoReturn($2, @1.first_line, @1.first_column); }
  | cmd_cout { $$ = $1; }
  | cmd_cin { $$ = $1; }
  | cmd_if { $$ = $1; }
  | cmd_while { $$ = $1; }
  | cmd_do_while { $$ = $1; }
  | cmd_for { $$ = $1; }
  | BREAK SEMI { $$ = criarNoBreak(@1.first_line, @1.first_column); }
  | CONTINUE SEMI { $$ = criarNoContinue(@1.first_line, @1.first_column); }
  | cmd_inc_dec { $$ = $1; }
  | expressao SEMI { $$ = $1; }
  | erro_oo { $$ = $1; }
  | erro_fora_escopo { $$ = $1; }
  | error SEMI { $$ = NULL; }
;

cmd_cout:
    COUT lista_cout SEMI { $$ = criarNoCout($2, @1.first_line, @1.first_column); }
;

lista_cout:
    LSHIFT alvo_cout { $$ = $2; }
  | lista_cout LSHIFT alvo_cout { $$ = anexarIrmao($1, $3); }
;

alvo_cout:
    ENDL { $$ = criarNoEndl(@1.first_line, @1.first_column); }
  | expressao { $$ = $1; }
;

cmd_cin:
    CIN lista_cin SEMI { $$ = criarNoCin($2, @1.first_line, @1.first_column); }
;

lista_cin:
    RSHIFT alvo_cin { $$ = $2; }
  | lista_cin RSHIFT alvo_cin { $$ = anexarIrmao($1, $3); }
;

alvo_cin:
    IDENT { $$ = criarNoId($1, @1.first_line, @1.first_column); free($1); }
  | IDENT LBRACKET expressao RBRACKET
      { $$ = criarNoIndice($1, $3, @1.first_line, @1.first_column); free($1); }
;

cmd_if:
    IF LPAREN expressao RPAREN comando
      { $$ = criarNoIf($3, $5, NULL, @1.first_line, @1.first_column); }
  | IF LPAREN expressao RPAREN comando ELSE comando
      { $$ = criarNoIf($3, $5, $7, @1.first_line, @1.first_column); }
;

cmd_while:
    WHILE LPAREN expressao RPAREN comando
      { $$ = criarNoWhile($3, $5, @1.first_line, @1.first_column); }
;

cmd_do_while:
    DO comando WHILE LPAREN expressao RPAREN SEMI
      { $$ = criarNoDoWhile($2, $5, @1.first_line, @1.first_column); }
;

cmd_for:
    FOR LPAREN for_init SEMI for_cond SEMI for_passo RPAREN comando
      { $$ = criarNoFor($3, $5, $7, $9, @1.first_line, @1.first_column); }
;

for_init:
    %empty { $$ = NULL; }
  | decl_var_base { $$ = $1; }
  | expressao { $$ = $1; }
;

for_cond:
    %empty { $$ = NULL; }
  | expressao { $$ = $1; }
;

for_passo:
    %empty { $$ = NULL; }
  | expressao { $$ = $1; }
  | primario INC { $$ = criarNoIncDec(OP_INC, $1, @2.first_line, @2.first_column); }
  | primario DEC { $$ = criarNoIncDec(OP_DEC, $1, @2.first_line, @2.first_column); }
;

cmd_inc_dec:
    primario INC SEMI { $$ = criarNoIncDec(OP_INC, $1, @2.first_line, @2.first_column); }
  | primario DEC SEMI { $$ = criarNoIncDec(OP_DEC, $1, @2.first_line, @2.first_column); }
;

erro_oo:
    CLASS { erro_oo("class", @1.first_line, @1.first_column); $$ = NULL; }
  | NEW { erro_oo("new", @1.first_line, @1.first_column); $$ = NULL; }
  | DELETE { erro_oo("delete", @1.first_line, @1.first_column); $$ = NULL; }
  | TEMPLATE { erro_oo("template", @1.first_line, @1.first_column); $$ = NULL; }
  | VIRTUAL { erro_oo("virtual", @1.first_line, @1.first_column); $$ = NULL; }
  | OVERRIDE { erro_oo("override", @1.first_line, @1.first_column); $$ = NULL; }
  | PUBLIC { erro_oo("public", @1.first_line, @1.first_column); $$ = NULL; }
  | PRIVATE { erro_oo("private", @1.first_line, @1.first_column); $$ = NULL; }
  | PROTECTED { erro_oo("protected", @1.first_line, @1.first_column); $$ = NULL; }
  | NAMESPACE { erro_oo("namespace", @1.first_line, @1.first_column); $$ = NULL; }
  | USING { erro_oo("using", @1.first_line, @1.first_column); $$ = NULL; }
  | THIS { erro_oo("this", @1.first_line, @1.first_column); $$ = NULL; }
  | NULLPTR { erro_oo("nullptr", @1.first_line, @1.first_column); $$ = NULL; }
  | FRIEND { erro_oo("friend", @1.first_line, @1.first_column); $$ = NULL; }
  | OPERATOR { erro_oo("operator", @1.first_line, @1.first_column); $$ = NULL; }
;

erro_fora_escopo:
    STRUCT { erro_fora_escopo("struct", @1.first_line, @1.first_column); $$ = NULL; }
  | SWITCH { erro_fora_escopo("switch", @1.first_line, @1.first_column); $$ = NULL; }
  | CASE { erro_fora_escopo("case", @1.first_line, @1.first_column); $$ = NULL; }
  | DEFAULT { erro_fora_escopo("default", @1.first_line, @1.first_column); $$ = NULL; }
;

expressao:
    expressao ASSIGN expressao %prec ASSIGN
      { if (!eh_alvo($1)) { erro_atribuicao_invalida(@2.first_line, @2.first_column); YYERROR; } $$ = criarNoAtribuicao(OP_ATRIB, $1, $3, @2.first_line, @2.first_column); }
  | expressao PLUS_ASSIGN expressao %prec PLUS_ASSIGN
      { if (!eh_alvo($1)) { erro_atribuicao_invalida(@2.first_line, @2.first_column); YYERROR; } $$ = criarNoAtribuicao(OP_ATRIB_SOMA, $1, $3, @2.first_line, @2.first_column); }
  | expressao MINUS_ASSIGN expressao %prec MINUS_ASSIGN
      { if (!eh_alvo($1)) { erro_atribuicao_invalida(@2.first_line, @2.first_column); YYERROR; } $$ = criarNoAtribuicao(OP_ATRIB_SUB, $1, $3, @2.first_line, @2.first_column); }
  | expressao TIMES_ASSIGN expressao %prec TIMES_ASSIGN
      { if (!eh_alvo($1)) { erro_atribuicao_invalida(@2.first_line, @2.first_column); YYERROR; } $$ = criarNoAtribuicao(OP_ATRIB_MULT, $1, $3, @2.first_line, @2.first_column); }
  | expressao DIVIDE_ASSIGN expressao %prec DIVIDE_ASSIGN
      { if (!eh_alvo($1)) { erro_atribuicao_invalida(@2.first_line, @2.first_column); YYERROR; } $$ = criarNoAtribuicao(OP_ATRIB_DIV, $1, $3, @2.first_line, @2.first_column); }
  | expressao QUESTION expressao COLON expressao %prec COLON
      { $$ = criarNoTernario($1, $3, $5, @2.first_line, @2.first_column); }
  | expressao OR expressao %prec OR { $$ = criarNoOp(OP_OU, $1, $3, @2.first_line, @2.first_column); }
  | expressao AND expressao %prec AND { $$ = criarNoOp(OP_E, $1, $3, @2.first_line, @2.first_column); }
  | expressao EQ expressao %prec EQ { $$ = criarNoOp(OP_IGUAL, $1, $3, @2.first_line, @2.first_column); }
  | expressao NE expressao %prec NE { $$ = criarNoOp(OP_DIFERENTE, $1, $3, @2.first_line, @2.first_column); }
  | expressao LT expressao %prec LT { $$ = criarNoOp(OP_MENOR, $1, $3, @2.first_line, @2.first_column); }
  | expressao GT expressao %prec GT { $$ = criarNoOp(OP_MAIOR, $1, $3, @2.first_line, @2.first_column); }
  | expressao LE expressao %prec LE { $$ = criarNoOp(OP_MENOR_IGUAL, $1, $3, @2.first_line, @2.first_column); }
  | expressao GE expressao %prec GE { $$ = criarNoOp(OP_MAIOR_IGUAL, $1, $3, @2.first_line, @2.first_column); }
  | expressao PLUS expressao %prec PLUS { $$ = criarNoOp(OP_SOMA, $1, $3, @2.first_line, @2.first_column); }
  | expressao MINUS expressao %prec MINUS { $$ = criarNoOp(OP_SUB, $1, $3, @2.first_line, @2.first_column); }
  | expressao TIMES expressao %prec TIMES { $$ = criarNoOp(OP_MULT, $1, $3, @2.first_line, @2.first_column); }
  | expressao DIVIDE expressao %prec DIVIDE { $$ = criarNoOp(OP_DIV, $1, $3, @2.first_line, @2.first_column); }
  | expressao MOD expressao %prec MOD { $$ = criarNoOp(OP_MOD, $1, $3, @2.first_line, @2.first_column); }
  | MINUS expressao %prec UMINUS { $$ = criarNoOpUnario(OP_NEG, $2, @1.first_line, @1.first_column); }
  | NOT expressao %prec NOT { $$ = criarNoOpUnario(OP_NAO, $2, @1.first_line, @1.first_column); }
  | INC expressao %prec NOT { (void)$2; $$ = NULL; yyerror("incremento/decremento nao permitido dentro de expressao"); YYERROR; }
  | DEC expressao %prec NOT { (void)$2; $$ = NULL; yyerror("incremento/decremento nao permitido dentro de expressao"); YYERROR; }
  | primario { $$ = $1; }
;

primario:
    NUM_INT { $$ = criarNoNum($1, @1.first_line, @1.first_column); }
  | NUM_FLOAT { $$ = criarNoFloat($1, @1.first_line, @1.first_column); }
  | STRING_LIT { $$ = criarNoString($1, @1.first_line, @1.first_column); free($1); }
  | TRUE { $$ = criarNoBool(1, @1.first_line, @1.first_column); }
  | FALSE { $$ = criarNoBool(0, @1.first_line, @1.first_column); }
  | IDENT { $$ = criarNoId($1, @1.first_line, @1.first_column); free($1); }
  | IDENT LBRACKET expressao RBRACKET
      { $$ = criarNoIndice($1, $3, @1.first_line, @1.first_column); free($1); }
  | IDENT LPAREN argumentos RPAREN
      { $$ = criarNoChamada($1, $3, @1.first_line, @1.first_column); free($1); }
  | LPAREN expressao RPAREN { $$ = $2; if ($$) $$->eh_parenthesized = 1; }
;

argumentos:
    %empty { $$ = NULL; }
  | lista_argumentos { $$ = $1; }
;

lista_argumentos:
    expressao { $$ = $1; }
  | lista_argumentos COMMA expressao { $$ = anexarIrmao($1, $3); }
;

%%

static void erro_atribuicao_invalida(int linha, int coluna) {
    YYLTYPE anterior = yylloc;
    yylloc.first_line = yylloc.last_line = linha;
    yylloc.first_column = yylloc.last_column = coluna;
    yyerror("syntax error");
    yylloc = anterior;
}

void yyerror(const char *s) {
    if (erros_compilacao >= MAX_ERROS) return;
    erros_compilacao++;
    if (report_diagnostic())
        fprintf(stderr, "Erro de sintaxe na linha %d, coluna %d: %s\n", yylloc.first_line, yylloc.first_column, s);
}
