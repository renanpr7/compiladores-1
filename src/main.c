#include <stdio.h>
#include <string.h>
#include "parser.tab.h"

extern int yyparse(void);
extern int yylex(void);
extern char *yytext;
extern int lexer_line;
extern int lexer_column;
extern int lexical_errors;
extern const char *lexer_lexeme;

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

static int dump_tokens(void) {
    int token;
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    while ((token = yylex()) != 0) {
        printf("%d:%d %s \"%s\"\n", yylloc.first_line, yylloc.first_column,
               token_name(token), lexer_lexeme ? lexer_lexeme : yytext);
    }
    printf("%d:%d EOF\n", lexer_line, lexer_column);
    return lexical_errors ? 1 : 0;
}

int main(int argc, char **argv) {
    int tokens = argc > 1 && (!strcmp(argv[1], "-t") || !strcmp(argv[1], "--tokens"));
    if (tokens) return dump_tokens();
    int result = yyparse();
    return lexical_errors || result != 0 ? 1 : 0;
}
