#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "optimize.h"
#include "diagnostics.h"

static int eh_literal_numerico(const NoAST *no) {
    return no && (no->tipo == NO_NUM || no->tipo == NO_FLOAT);
}

static int eh_literal_escalar(const NoAST *no) {
    return no && (no->tipo == NO_NUM || no->tipo == NO_FLOAT || no->tipo == NO_BOOL);
}

static float valor_como_float(const NoAST *no) {
    if (!no) return 0.0f;
    if (no->tipo == NO_FLOAT) return no->valor_float;
    return (float)no->valor_int;
}

static int valor_como_bool(const NoAST *no) {
    if (!no) return 0;
    if (no->tipo == NO_FLOAT) return no->valor_float != 0.0f;
    return no->valor_int != 0;
}

static void reportar_divisao_por_zero(int linha, int coluna) {
    if (report_diagnostic()) {
        fprintf(stderr, "Erro semantico na linha %d, coluna %d: divisao por zero\n",
                linha, coluna);
    }
}

static void limpar_filhos(NoAST *no) {
    for (int i = 0; i < AST_MAX_FILHOS; ++i) {
        if (no->filho[i]) {
            liberarAST(no->filho[i]);
            no->filho[i] = NULL;
        }
    }
    no->operador = OP_NENHUM;
}

static void mutar_para_num(NoAST *no, int valor) {
    limpar_filhos(no);
    no->tipo = NO_NUM;
    no->tipo_dado = TIPO_INT;
    no->valor_int = valor;
    no->valor_float = 0.0f;
}

static void mutar_para_float(NoAST *no, float valor) {
    limpar_filhos(no);
    no->tipo = NO_FLOAT;
    no->tipo_dado = TIPO_FLOAT;
    no->valor_float = valor;
    no->valor_int = 0;
}

static void mutar_para_bool(NoAST *no, int valor) {
    limpar_filhos(no);
    no->tipo = NO_BOOL;
    no->tipo_dado = TIPO_BOOL;
    no->valor_int = valor != 0;
    no->valor_float = 0.0f;
}

static void otimizar_op_binario(NoAST *no) {
    NoAST *esq = no->filho[0];
    NoAST *dir = no->filho[1];
    if (!esq || !dir) return;

    switch (no->operador) {
    case OP_SOMA:
    case OP_SUB:
    case OP_MULT:
    case OP_DIV:
        if (!eh_literal_numerico(esq) || !eh_literal_numerico(dir)) return;
        if (esq->tipo == NO_FLOAT || dir->tipo == NO_FLOAT) {
            float ve = valor_como_float(esq);
            float vd = valor_como_float(dir);
            if (no->operador == OP_DIV && vd == 0.0f) {
                reportar_divisao_por_zero(no->linha, no->coluna);
                return;
            }
            float res = 0.0f;
            switch (no->operador) {
            case OP_SOMA: res = ve + vd; break;
            case OP_SUB:  res = ve - vd; break;
            case OP_MULT: res = ve * vd; break;
            case OP_DIV:  res = ve / vd; break;
            default:      break;
            }
            mutar_para_float(no, res);
        } else {
            int ve = esq->valor_int;
            int vd = dir->valor_int;
            if (no->operador == OP_DIV) {
                if (vd == 0) {
                    reportar_divisao_por_zero(no->linha, no->coluna);
                    return;
                }
                if (ve == INT_MIN && vd == -1) return;
            }
            int res = 0;
            switch (no->operador) {
            case OP_SOMA: res = ve + vd; break;
            case OP_SUB:  res = ve - vd; break;
            case OP_MULT: res = ve * vd; break;
            case OP_DIV:  res = ve / vd; break;
            default:      break;
            }
            mutar_para_num(no, res);
        }
        break;

    case OP_MOD:
        if (esq->tipo != NO_NUM || dir->tipo != NO_NUM) return;
        if (dir->valor_int == 0) {
            reportar_divisao_por_zero(no->linha, no->coluna);
            return;
        }
        if (esq->valor_int == INT_MIN && dir->valor_int == -1) {
            mutar_para_num(no, 0);
            return;
        }
        mutar_para_num(no, esq->valor_int % dir->valor_int);
        break;

    case OP_IGUAL:
    case OP_DIFERENTE:
    case OP_MENOR:
    case OP_MAIOR:
    case OP_MENOR_IGUAL:
    case OP_MAIOR_IGUAL: {
        if (!eh_literal_escalar(esq) || !eh_literal_escalar(dir)) return;
        int res = 0;
        if (esq->tipo == NO_FLOAT || dir->tipo == NO_FLOAT) {
            float ve = valor_como_float(esq);
            float vd = valor_como_float(dir);
            switch (no->operador) {
            case OP_IGUAL:       res = (ve == vd); break;
            case OP_DIFERENTE:   res = (ve != vd); break;
            case OP_MENOR:       res = (ve < vd);  break;
            case OP_MAIOR:       res = (ve > vd);  break;
            case OP_MENOR_IGUAL: res = (ve <= vd); break;
            case OP_MAIOR_IGUAL: res = (ve >= vd); break;
            default:             break;
            }
        } else {
            int ve = esq->valor_int;
            int vd = dir->valor_int;
            switch (no->operador) {
            case OP_IGUAL:       res = (ve == vd); break;
            case OP_DIFERENTE:   res = (ve != vd); break;
            case OP_MENOR:       res = (ve < vd);  break;
            case OP_MAIOR:       res = (ve > vd);  break;
            case OP_MENOR_IGUAL: res = (ve <= vd); break;
            case OP_MAIOR_IGUAL: res = (ve >= vd); break;
            default:             break;
            }
        }
        mutar_para_bool(no, res);
        break;
    }

    case OP_E:
    case OP_OU: {
        if (!eh_literal_escalar(esq) || !eh_literal_escalar(dir)) return;
        int be = valor_como_bool(esq);
        int bd = valor_como_bool(dir);
        int res = (no->operador == OP_E) ? (be && bd) : (be || bd);
        mutar_para_bool(no, res);
        break;
    }

    default:
        break;
    }
}

static void otimizar_op_unario(NoAST *no) {
    NoAST *op = no->filho[0];
    if (!op) return;

    switch (no->operador) {
    case OP_NEG:
        if (op->tipo == NO_NUM) {
            if (op->valor_int == INT_MIN) return;
            mutar_para_num(no, -op->valor_int);
        } else if (op->tipo == NO_FLOAT) {
            mutar_para_float(no, -op->valor_float);
        }
        break;

    case OP_NAO:
        if (eh_literal_escalar(op)) {
            mutar_para_bool(no, !valor_como_bool(op));
        }
        break;

    default:
        break;
    }
}

NoAST *otimizar_ast(NoAST *raiz) {
    if (!raiz) return NULL;

    for (int i = 0; i < AST_MAX_FILHOS; ++i) {
        raiz->filho[i] = otimizar_ast(raiz->filho[i]);
    }
    raiz->proximo = otimizar_ast(raiz->proximo);

    if (raiz->tipo == NO_OP_BINARIO) {
        otimizar_op_binario(raiz);
    } else if (raiz->tipo == NO_OP_UNARIO) {
        otimizar_op_unario(raiz);
    }

    return raiz;
}
