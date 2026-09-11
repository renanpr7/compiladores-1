# Compilador C++ (subconjunto) -> C
# Pipeline: bison -d -> flex -> gcc -std=c99 -Wall -Wextra

CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -D_DEFAULT_SOURCE
SRC     = src
TARGET  = compilador

all: $(TARGET)

$(TARGET): $(SRC)/main.c $(SRC)/lex.yy.c $(SRC)/parser.tab.c $(SRC)/ast.c $(SRC)/ast.h $(SRC)/tipos.h $(SRC)/diagnostics.h
	$(CC) $(CFLAGS) -I$(SRC) -o $@ $(filter %.c,$^)

# Flex gera o analisador lexico; precisa dos tokens que o Bison define
$(SRC)/lex.yy.c: $(SRC)/lexer.l $(SRC)/parser.tab.h
	flex -o $@ $(SRC)/lexer.l

# Bison gera o analisador sintatico (.c) e a lista de tokens (.h)
$(SRC)/parser.tab.c $(SRC)/parser.tab.h: $(SRC)/parser.y
	bison -d -o $(SRC)/parser.tab.c $(SRC)/parser.y

clean:
	rm -f $(TARGET) $(SRC)/lex.yy.c $(SRC)/parser.tab.c $(SRC)/parser.tab.h

test: $(TARGET)
	@bash tests/run_tests.sh
	@bash tests/cli/run_cli_tests.sh

.PHONY: all clean test
