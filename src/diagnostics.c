#include <stdarg.h>
#include <stdio.h>
#include "diagnostics.h"

int diagnostic_count = 0;

void reset_diagnostics(void) {
    diagnostic_count = 0;
}

int report_diagnostic(void) {
    if (diagnostic_count >= MAX_DIAGNOSTICS) return 0;
    ++diagnostic_count;
    return 1;
}

void vemit_diagnostic(const char *categoria, int linha, int coluna, const char *fmt, va_list args) {
    if (!report_diagnostic()) return;
    fprintf(stderr, "%s na linha %d, coluna %d: ",
            categoria ? categoria : "Erro", linha, coluna);
    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
}

void emit_diagnostic(const char *categoria, int linha, int coluna, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vemit_diagnostic(categoria, linha, coluna, fmt, args);
    va_end(args);
}
