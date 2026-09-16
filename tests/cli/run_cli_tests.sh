#!/bin/bash
set -u

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
COMPILADOR="$ROOT_DIR/compilador"
[ -x "$ROOT_DIR/compilador.exe" ] && COMPILADOR="$ROOT_DIR/compilador.exe"
TMP_DIR=$(mktemp -d)
trap 'rm -rf "$TMP_DIR"' EXIT

PASS=0
FAIL=0

check() {
    local name="$1" expected_status="$2" actual_status="$3" condition="$4"
    if [ "$actual_status" -eq "$expected_status" ] && eval "$condition"; then
        echo "PASS $name"
        PASS=$((PASS + 1))
    else
        echo "FAIL $name (exit $actual_status)"
        FAIL=$((FAIL + 1))
    fi
}

printf 'int x;\n' > "$TMP_DIR/valid.cpp"
"$COMPILADOR" < "$TMP_DIR/valid.cpp" > "$TMP_DIR/default.out" 2> "$TMP_DIR/default.err"
status=$?
check "default success" 0 "$status" \
    "[ \"\$(tr -d \\"\\r\\" < \"$TMP_DIR/default.out\")\" = OK ] && [ ! -s \"$TMP_DIR/default.err\" ]"

"$COMPILADOR" -t < "$TMP_DIR/valid.cpp" > "$TMP_DIR/tokens.out" 2> "$TMP_DIR/tokens.err"
status=$?
check "short token flag" 0 "$status" \
    "grep -q '1:1 INT \"int\"' \"$TMP_DIR/tokens.out\" && grep -q 'EOF' \"$TMP_DIR/tokens.out\""

"$COMPILADOR" --tokens < "$TMP_DIR/valid.cpp" > "$TMP_DIR/tokens-long.out" 2> "$TMP_DIR/tokens-long.err"
status=$?
check "long token flag" 0 "$status" \
    "cmp -s \"$TMP_DIR/tokens.out\" \"$TMP_DIR/tokens-long.out\""

"$COMPILADOR" --help > "$TMP_DIR/help.out" 2> "$TMP_DIR/help.err"
status=$?
check "help" 0 "$status" \
    "grep -q 'Uso:' \"$TMP_DIR/help.out\" && grep -q -- '-a.*nao implementada' \"$TMP_DIR/help.out\" && grep -q -- '-i.*nao implementada' \"$TMP_DIR/help.out\""

"$COMPILADOR" --not-an-option > "$TMP_DIR/unknown.out" 2> "$TMP_DIR/unknown.err"
status=$?
check "unknown option" 2 "$status" \
    "[ ! -s \"$TMP_DIR/unknown.out\" ] && grep -q 'Uso:' \"$TMP_DIR/unknown.err\""

printf 'int x\n' > "$TMP_DIR/syntax.cpp"
"$COMPILADOR" < "$TMP_DIR/syntax.cpp" > "$TMP_DIR/syntax.out" 2> "$TMP_DIR/syntax.err"
status=$?
check "syntax failure" 1 "$status" \
    "[ ! -s \"$TMP_DIR/syntax.out\" ] && grep -q 'Erro de sintaxe' \"$TMP_DIR/syntax.err\""

printf '@ @ @ @ @ @ @ @ @ @ @ @\n' > "$TMP_DIR/many-errors.cpp"
"$COMPILADOR" -t < "$TMP_DIR/many-errors.cpp" > "$TMP_DIR/many-errors.out" 2> "$TMP_DIR/many-errors.err"
status=$?
error_count=$(grep -c 'Erro lexico' "$TMP_DIR/many-errors.err" || true)
check "diagnostic limit" 1 "$status" \
    "[ \"$error_count\" -eq 10 ] && grep -q 'EOF' \"$TMP_DIR/many-errors.out\""

echo "$PASS/$((PASS + FAIL)) CLI tests passed"
[ "$FAIL" -eq 0 ]
