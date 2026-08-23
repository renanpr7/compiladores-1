# Compilador SimC - Makefile
# Pipeline: bison -d -> flex -> gcc -std=c99 -Wall -Wextra

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -D_DEFAULT_SOURCE
BISON = bison
FLEX = flex

# Artefatos gerados
PARSER_TAB_C = parser.tab.c
PARSER_TAB_H = parser.tab.h
LEX_YY_C = lex.yy.c
TARGET = compilador

all: $(TARGET)

$(TARGET): src/main.c $(LEX_YY_C) $(PARSER_TAB_C)
	$(CC) $(CFLAGS) -o $@ src/main.c $(LEX_YY_C) $(PARSER_TAB_C)

$(LEX_YY_C): lexer/lexer.l $(PARSER_TAB_H)
	$(FLEX) lexer/lexer.l

$(PARSER_TAB_C) $(PARSER_TAB_H): parser/parser.y
	$(BISON) -d -o $(PARSER_TAB_C) parser/parser.y

clean:
	rm -f $(TARGET) $(LEX_YY_C) $(PARSER_TAB_C) $(PARSER_TAB_H) *.o

test: $(TARGET)
	@bash tests/run_tests.sh

.PHONY: all clean test
