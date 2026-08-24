#!/bin/bash
# Golden test runner para SimC
# Compara saida real vs .expected para cada .simc
# Exit 0 se todos passam, exit 1 se qualquer falha

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
COMPILADOR="$ROOT_DIR/compilador"

PASS=0
FAIL=0
TOTAL=0

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

run_test() {
    local simc_file="$1"
    local expected_file="${simc_file%.simc}.expected"
    local test_name=$(basename "$simc_file" .simc)

    TOTAL=$((TOTAL + 1))

    if [ ! -f "$expected_file" ]; then
        echo -e "${RED}FAIL${NC} $test_name (arquivo .expected nao encontrado)"
        FAIL=$((FAIL + 1))
        return
    fi

    # Gera saida real
    actual=$("$COMPILADOR" < "$simc_file" 2>&1) || true
    expected=$(cat "$expected_file")

    if [ "$actual" = "$expected" ]; then
        echo -e "${GREEN}PASS${NC} $test_name"
        PASS=$((PASS + 1))
    else
        echo -e "${RED}FAIL${NC} $test_name"
        echo "  Esperado:"
        echo "$expected" | sed 's/^/    /'
        echo "  Obtido:"
        echo "$actual" | sed 's/^/    /'
        FAIL=$((FAIL + 1))
    fi
}

echo "=== Golden Tests SimC ==="
echo ""

# Rodar testes do lexer
for f in "$SCRIPT_DIR/lexer/"*.simc; do
    [ -f "$f" ] && run_test "$f"
done

# Rodar testes de aceitacao do parser
for f in "$SCRIPT_DIR/parser/aceita/"*.simc; do
    [ -f "$f" ] && run_test "$f"
done

# Rodar testes de erro do parser
for f in "$SCRIPT_DIR/parser/erros/"*.simc; do
    [ -f "$f" ] && run_test "$f"
done

echo ""
echo "=== Resultado: $PASS/$TOTAL passaram, $FAIL falharam ==="

if [ "$FAIL" -gt 0 ]; then
    exit 1
fi

exit 0
