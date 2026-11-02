#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"
#include "ast.h"
#include "diagnostics.h"
#include "semantica.h"
#include "optimize.h"
#include "tac.h"
#include "codegen.h"

extern int yyparse(void);
extern int yylex(void);
extern int yylex_destroy(void);
extern char *yytext;
extern int lexer_line;
extern int lexer_column;
extern int lexical_errors;
extern const char *lexer_lexeme;
extern int erros_compilacao;
extern NoAST *ast_raiz;

static const char *token_name(int token) {
    switch (token) {
#define TOKEN(name) case name: return #name
        TOKEN(IDENT); TOKEN(NUM_INT); TOKEN(NUM_FLOAT); TOKEN(STRING_LIT);
        TOKEN(INT); TOKEN(FLOAT); TOKEN(BOOL); TOKEN(STRING); TOKEN(VOID); TOKEN(CONST);
        TOKEN(TRUE); TOKEN(FALSE); TOKEN(IF); TOKEN(ELSE); TOKEN(WHILE); TOKEN(FOR);
        TOKEN(DO); TOKEN(BREAK); TOKEN(CONTINUE); TOKEN(RETURN); TOKEN(COUT); TOKEN(CIN);
        TOKEN(ENDL); TOKEN(PLUS); TOKEN(MINUS); TOKEN(TIMES); TOKEN(DIVIDE); TOKEN(MOD);
        TOKEN(INC); TOKEN(DEC); TOKEN(PLUS_ASSIGN); TOKEN(MINUS_ASSIGN); TOKEN(TIMES_ASSIGN);
        TOKEN(DIVIDE_ASSIGN); TOKEN(EQ); TOKEN(NE); TOKEN(LT); TOKEN(GT); TOKEN(LE); TOKEN(GE);
        TOKEN(AND); TOKEN(OR); TOKEN(NOT); TOKEN(LPAREN); TOKEN(RPAREN); TOKEN(LBRACE);
        TOKEN(RBRACE); TOKEN(LBRACKET); TOKEN(RBRACKET); TOKEN(SEMI); TOKEN(COMMA);
        TOKEN(QUESTION); TOKEN(COLON); TOKEN(ASSIGN); TOKEN(AMP); TOKEN(LSHIFT); TOKEN(RSHIFT);
        TOKEN(CLASS); TOKEN(NEW); TOKEN(DELETE); TOKEN(TEMPLATE); TOKEN(VIRTUAL); TOKEN(OVERRIDE);
        TOKEN(PUBLIC); TOKEN(PRIVATE); TOKEN(PROTECTED); TOKEN(NAMESPACE); TOKEN(USING);
        TOKEN(THIS); TOKEN(NULLPTR); TOKEN(FRIEND); TOKEN(OPERATOR); TOKEN(STRUCT); TOKEN(SWITCH);
        TOKEN(CASE); TOKEN(DEFAULT);
#undef TOKEN
    default: return "UNKNOWN";
    }
}

static void limpar_recursos(void) {
    liberarAST(ast_raiz);
    ast_raiz = NULL;
    yylex_destroy();
}

static int dump_tokens(void) {
    int token;
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    while ((token = yylex()) != 0) {
        printf("%d:%d %s \"%s\"\n", yylloc.first_line, yylloc.first_column,
               token_name(token), lexer_lexeme ? lexer_lexeme : yytext);
        if (token == IDENT) {
            free(yylval.identifier);
            yylval.identifier = NULL;
            lexer_lexeme = NULL;
        } else if (token == STRING_LIT) {
            free(yylval.string_value);
            yylval.string_value = NULL;
            lexer_lexeme = NULL;
        }
    }
    printf("%d:%d EOF\n", lexer_line, lexer_column);
    yylex_destroy();
    return lexical_errors ? 1 : 0;
}

static void print_help(FILE *stream) {
    fprintf(stream,
            "Uso: compilador [opcao]\n"
            "\n"
            "Opcoes:\n"
            "  -t, --tokens  imprime os tokens lidos\n"
            "  -h, --help    mostra esta ajuda\n"
            "  -a, --ast     imprime a arvore sintatica abstrata\n"
            "  -i, --tac     imprime o codigo intermediario (TAC)\n"
            "\n"
            "Exemplos:\n"
            "  compilador < programa.cpp\n"
            "  compilador --tokens < programa.cpp\n"
            "  compilador --ast < programa.cpp\n"
            "  compilador --tac < programa.cpp\n");
}

int main(int argc, char **argv) {
    int result;

    if (argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) {
        print_help(stdout);
        return 0;
    }
    if (argc == 2 && (!strcmp(argv[1], "-t") || !strcmp(argv[1], "--tokens"))) {
        return dump_tokens() == 0 && !lexical_errors ? 0 : 1;
    }
    if (argc == 2 && (!strcmp(argv[1], "-a") || !strcmp(argv[1], "--ast"))) {
        result = yyparse();
        if (lexical_errors || result != 0 || erros_compilacao > 0) {
            limpar_recursos();
            return 1;
        }
        if (otimizar_ast(ast_raiz) > 0) {
            liberarAST(ast_raiz);
            ast_raiz = NULL;
            return 1;
        }
        imprimirAST(ast_raiz);
        limpar_recursos();
        return 0;
    }
    if (argc == 2 && (!strcmp(argv[1], "-i") || !strcmp(argv[1], "--tac"))) {
        result = yyparse();
        if (lexical_errors || result != 0 || erros_compilacao > 0) {
            limpar_recursos();
            return 1;
        }
        if (analisar_semantica(ast_raiz) > 0) {
            limpar_recursos();
            return 1;
        }
        if (otimizar_ast(ast_raiz) > 0) {
            liberarAST(ast_raiz);
            ast_raiz = NULL;
            return 1;
        }
        ProgramaTAC *tac = gerar_tac(ast_raiz);
        imprimir_tac(tac, stdout);
        liberar_tac(tac);
        limpar_recursos();
        return 0;
    }
    if (argc != 1) {
        print_help(stderr);
        return 2;
    }

    result = yyparse();
    if (lexical_errors || result != 0 || erros_compilacao > 0) {
        limpar_recursos();
        return 1;
    }
    if (analisar_semantica(ast_raiz) > 0) {
        limpar_recursos();
        return 1;
    }
    if (otimizar_ast(ast_raiz) > 0) {
        liberarAST(ast_raiz);
        ast_raiz = NULL;
        return 1;
    }
    gerar_codigo(ast_raiz, stdout);
    limpar_recursos();
    return 0;
}
