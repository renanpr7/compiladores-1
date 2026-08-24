#include <stdio.h>
#include <stdlib.h>

extern int yyparse(void);

int main(void) {
    int result = yyparse();
    return result;
}
