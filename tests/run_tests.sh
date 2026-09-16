#!/bin/bash
# Executor dos golden tests.
#
# Cada caso e um arquivo .cpp com um .expected ao lado. O runner roda o
# compilador, compara a saida e confere o codigo de retorno.
#
# Convencao de codigo de retorno: casos dentro de uma pasta "erros" devem
# terminar com 1; todos os outros com 0.
#
# Fases (o professor pede teste automatizado para cada uma):
#   lexer/       dump de tokens          ./compilador -t
#   parser/      validacao sintatica     ./compilador
#   ast/         dump da arvore          ./compilador -a
#   semantica/   erros de significado    ./compilador
#   codegen/     codigo C gerado         ./compilador
#   execucao/    o C gerado compila e roda com gcc

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
COMPILADOR="$ROOT_DIR/compilador"
[ -x "$ROOT_DIR/compilador.exe" ] && COMPILADOR="$ROOT_DIR/compilador.exe"

PASS=0; FAIL=0; TOTAL=0
RED='\033[0;31m'; GREEN='\033[0;32m'; DIM='\033[2m'; NC='\033[0m'

# $1 arquivo .cpp   $2 flag do compilador
run_test() {
    local cpp_file="$1" flag="$2"
    local expected_file="${cpp_file%.cpp}.expected"
    local name="${cpp_file#$SCRIPT_DIR/}"
    TOTAL=$((TOTAL + 1))

    if [ ! -f "$expected_file" ]; then
        echo -e "${RED}FAIL${NC} $name ${DIM}(falta o .expected)${NC}"
        FAIL=$((FAIL + 1)); return
    fi

    # Pastas chamadas "erros" esperam falha; o resto espera sucesso
    local want_exit=0
    case "$cpp_file" in */erros/*) want_exit=1 ;; esac

    local actual got_exit
    actual=$("$COMPILADOR" $flag < "$cpp_file" 2>&1); got_exit=$?

    local expected
    expected=$(cat "$expected_file")
    # Compare without caring whether either side uses LF or CRLF.
    actual=${actual//$'\r'/}
    expected=${expected//$'\r'/}
    if [ "$actual" != "$expected" ]; then
        echo -e "${RED}FAIL${NC} $name ${DIM}(saida diferente)${NC}"
        diff <(printf '%s\n' "$expected") <(printf '%s\n' "$actual") | sed 's/^/      /' | head -12
        FAIL=$((FAIL + 1)); return
    fi
    if [ "$got_exit" -ne "$want_exit" ]; then
        echo -e "${RED}FAIL${NC} $name ${DIM}(exit $got_exit, esperado $want_exit)${NC}"
        FAIL=$((FAIL + 1)); return
    fi
    echo -e "${GREEN}PASS${NC} $name"
    PASS=$((PASS + 1))
}

# Compila o C gerado e confere o que o programa imprime ao rodar
run_execucao() {
    local cpp_file="$1"
    local expected_file="${cpp_file%.cpp}.expected"
    local name="${cpp_file#$SCRIPT_DIR/}"
    local tmp; tmp=$(mktemp -d)
    TOTAL=$((TOTAL + 1))

    if ! "$COMPILADOR" < "$cpp_file" > "$tmp/saida.c" 2>"$tmp/erro"; then
        echo -e "${RED}FAIL${NC} $name ${DIM}(compilador falhou)${NC}"
        sed 's/^/      /' "$tmp/erro" | head -5
        FAIL=$((FAIL + 1)); rm -rf "$tmp"; return
    fi
    if ! gcc "$tmp/saida.c" -o "$tmp/prog" 2>"$tmp/gcc.log"; then
        echo -e "${RED}FAIL${NC} $name ${DIM}(gcc rejeitou o C gerado)${NC}"
        sed 's/^/      /' "$tmp/gcc.log" | head -8
        FAIL=$((FAIL + 1)); rm -rf "$tmp"; return
    fi
    if [ "$("$tmp/prog")" = "$(cat "$expected_file")" ]; then
        echo -e "${GREEN}PASS${NC} $name ${DIM}(compilou e rodou)${NC}"
        PASS=$((PASS + 1))
    else
        echo -e "${RED}FAIL${NC} $name ${DIM}(programa imprimiu outra coisa)${NC}"
        diff <(cat "$expected_file") <("$tmp/prog") | sed 's/^/      /' | head -12
        FAIL=$((FAIL + 1))
    fi
    rm -rf "$tmp"
}

# $1 titulo   $2 pasta   $3 flag   $4 funcao
suite() {
    local dir="$SCRIPT_DIR/$2"
    [ -d "$dir" ] || return
    local files; files=$(find "$dir" -name '*.cpp' | sort)
    [ -n "$files" ] || return
    echo; echo "-- $1"
    while IFS= read -r f; do "${4:-run_test}" "$f" "$3"; done <<< "$files"
}

echo "=== Golden Tests ==="
suite "Analise lexica"    lexer      -t
suite "Analise sintatica" parser     ""
suite "AST"               ast        -a
suite "Analise semantica" semantica  ""
suite "Geracao de codigo" codegen    ""
suite "Execucao"          execucao   "" run_execucao

echo
echo "=== $PASS/$TOTAL passaram, $FAIL falharam ==="
[ "$FAIL" -gt 0 ] && exit 1
exit 0
