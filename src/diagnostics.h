#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

/* Maximum number of diagnostics printed. Analysis continues past the limit;
   only the messages stop being printed. */
#define MAX_DIAGNOSTICS 10

/* Shared counter across lexer and parser; defined in main.c. */
extern int diagnostic_count;

/* Returns 1 if the diagnostic may be printed, and accounts for it. */
int report_diagnostic(void);

#endif
