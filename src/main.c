#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yyparse(void);
extern int yylex(void);
extern char *yytext;
extern int yyleng;
extern int yylineno;
extern int yylcol;
extern int lexer_token_line;
extern int lexer_token_column;
extern int lexer_has_errors;
extern const char *lexer_token_name;

static void print_escaped(const char *text, int length) {
    int index;

    for (index = 0; index < length; index++) {
        switch (text[index]) {
            case '\\':
                fputs("\\\\", stdout);
                break;
            case '\"':
                fputs("\\\"", stdout);
                break;
            case '\n':
                fputs("\\n", stdout);
                break;
            case '\t':
                fputs("\\t", stdout);
                break;
            default:
                putchar(text[index]);
                break;
        }
    }
}

static int dump_tokens(void) {
    int token;

    while ((token = yylex()) != EOF) {
        const char *lexeme = yytext;
        int length = yyleng;

        if (lexer_token_name == NULL) {
            continue;
        }

        if (strcmp(lexer_token_name, "STRING_LITERAL") == 0 && length >= 2) {
            lexeme++;
            length -= 2;
        }

        printf("%d:%d %s \"", lexer_token_line, lexer_token_column, lexer_token_name);
        print_escaped(lexeme, length);
        puts("\"");
        fflush(stdout);
    }

    printf("%d:%d EOF\n", yylineno, yylcol);
    fflush(stdout);
    return lexer_has_errors ? EXIT_FAILURE : EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        return yyparse();
    }

    if (argc == 2 &&
        (strcmp(argv[1], "-t") == 0 || strcmp(argv[1], "--tokens") == 0)) {
        return dump_tokens();
    }

    fprintf(stderr, "Uso: %s [-t|--tokens]\n", argv[0]);
    return EXIT_FAILURE;
}
