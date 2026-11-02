# Compilador C++ (subconjunto) -> C
# Pipeline: bison -d -> flex -> gcc -std=c99 -Wall -Wextra

CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -D_DEFAULT_SOURCE
MEMFLAGS = -g -fsanitize=address,undefined
SRC     = src
TARGET  = compilador
SOURCES = $(SRC)/main.c $(SRC)/diagnostics.c $(SRC)/lex.yy.c $(SRC)/parser.tab.c $(SRC)/ast.c $(SRC)/tabela.c $(SRC)/semantica.c $(SRC)/optimize.c $(SRC)/tac.c $(SRC)/codegen.c
HEADERS = $(SRC)/ast.h $(SRC)/tabela.h $(SRC)/semantica.h $(SRC)/optimize.h $(SRC)/tac.h $(SRC)/codegen.h $(SRC)/tipos.h $(SRC)/diagnostics.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -I$(SRC) -o $@ $(SOURCES)

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

test-memory: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(MEMFLAGS) -I$(SRC) -o $(TARGET) $(SOURCES)
	@RUN_MEMORY_CHECK=1 bash tests/run_tests.sh
	@bash tests/cli/run_cli_tests.sh
	@$(CC) $(CFLAGS) -I$(SRC) -o $(TARGET) $(SOURCES)

.PHONY: all clean test test-memory
