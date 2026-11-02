#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <stdarg.h>

/* Maximum number of diagnostics printed. Analysis continues past the limit;
   only the messages stop being printed. */
#define MAX_DIAGNOSTICS 10

/* Shared counter across lexer, parser and semantic analyzer; defined in diagnostics.c. */
extern int diagnostic_count;

/* Resets the diagnostic counter. */
void reset_diagnostics(void);

/* Returns 1 if the diagnostic may be printed, and accounts for it. */
int report_diagnostic(void);

/* Formats and prints a diagnostic with category, line and column if within MAX_DIAGNOSTICS:
   "<categoria> na linha <linha>, coluna <coluna>: <mensagem>\n" */
void emit_diagnostic(const char *categoria, int linha, int coluna, const char *fmt, ...);

/* Formats and prints a diagnostic using a va_list if within MAX_DIAGNOSTICS. */
void vemit_diagnostic(const char *categoria, int linha, int coluna, const char *fmt, va_list args);

#endif
