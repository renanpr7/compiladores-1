#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tac.h"

typedef struct ContextoLacoTAC {
    const char *label_continue;
    const char *label_break;
    struct ContextoLacoTAC *anterior;
} ContextoLacoTAC;

typedef struct {
    ProgramaTAC *prog;
    ContextoLacoTAC *laco_atual;
} ContextoTAC;

static void sem_memoria_tac(void) {
    fprintf(stderr, "Erro: memoria insuficiente ao gerar TAC\n");
    exit(EXIT_FAILURE);
}

static char *copiar_texto_tac(const char *s) {
    if (!s) return NULL;
    size_t tam = strlen(s) + 1;
    char *copia = malloc(tam);
    if (!copia) sem_memoria_tac();
    memcpy(copia, s, tam);
    return copia;
}

static const char *simbolo_op_tac(Operador op) {
    switch (op) {
    case OP_SOMA:        return "+";
    case OP_SUB:         return "-";
    case OP_MULT:        return "*";
    case OP_DIV:         return "/";
    case OP_MOD:         return "%";
    case OP_IGUAL:       return "==";
    case OP_DIFERENTE:   return "!=";
    case OP_MENOR:       return "<";
    case OP_MAIOR:       return ">";
    case OP_MENOR_IGUAL: return "<=";
    case OP_MAIOR_IGUAL: return ">=";
    case OP_E:           return "&&";
    case OP_OU:          return "||";
    case OP_NAO:         return "!";
    case OP_NEG:         return "-";
    case OP_ATRIB_SOMA:  return "+";
    case OP_ATRIB_SUB:   return "-";
    case OP_ATRIB_MULT:  return "*";
    case OP_ATRIB_DIV:   return "/";
    case OP_INC:         return "+";
    case OP_DEC:         return "-";
    default:             return "";
    }
}

ProgramaTAC *criar_programa_tac(void) {
    ProgramaTAC *prog = calloc(1, sizeof(*prog));
    if (!prog) sem_memoria_tac();
    prog->proximo_temp = 1;
    prog->proximo_label = 1;
    return prog;
}

char *novo_temporario_tac(ProgramaTAC *prog) {
    if (!prog) return NULL;
    int id = prog->proximo_temp++;
    int n = snprintf(NULL, 0, "_t%d", id);
    if (n < 0) sem_memoria_tac();
    char *nome = malloc((size_t)n + 1);
    if (!nome) sem_memoria_tac();
    snprintf(nome, (size_t)n + 1, "_t%d", id);
    return nome;
}

char *novo_label_tac(ProgramaTAC *prog) {
    if (!prog) return NULL;
    int id = prog->proximo_label++;
    int n = snprintf(NULL, 0, "L%d", id);
    if (n < 0) sem_memoria_tac();
    char *nome = malloc((size_t)n + 1);
    if (!nome) sem_memoria_tac();
    snprintf(nome, (size_t)n + 1, "L%d", id);
    return nome;
}

int emitir_instrucao_tac(ProgramaTAC *prog, TipoInstrucaoTAC tipo,
                         const char *op, const char *arg1,
                         const char *arg2, const char *result) {
    if (!prog) return -1;

    if (prog->quantidade == prog->capacidade) {
        size_t nova_cap = (prog->capacidade == 0) ? 16 : (prog->capacidade * 2);
        if (nova_cap < prog->capacidade ||
            nova_cap > ((size_t)-1) / sizeof(InstrucaoTAC)) {
            sem_memoria_tac();
        }
        InstrucaoTAC *novo = realloc(prog->instrucoes, nova_cap * sizeof(InstrucaoTAC));
        if (!novo) sem_memoria_tac();
        prog->instrucoes = novo;
        prog->capacidade = nova_cap;
    }

    InstrucaoTAC *inst = &prog->instrucoes[prog->quantidade++];
    inst->tipo = tipo;
    inst->op = copiar_texto_tac(op);
    inst->arg1 = copiar_texto_tac(arg1);
    inst->arg2 = copiar_texto_tac(arg2);
    inst->result = copiar_texto_tac(result);
    return 0;
}

static char *tac_expressao(ContextoTAC *ctx, const NoAST *no);
static void tac_comando(ContextoTAC *ctx, const NoAST *no);
static void tac_lista_comandos(ContextoTAC *ctx, const NoAST *lista);

static void tac_declaracao(ContextoTAC *ctx, const NoAST *no) {
    if (!no) return;
    for (const NoAST *decl = no->filho[0]; decl != NULL; decl = decl->proximo) {
        if (decl->filho[0]) {
            char *val = tac_expressao(ctx, decl->filho[0]);
            emitir_instrucao_tac(ctx->prog, TAC_COPY, NULL, val, NULL, decl->texto);
            free(val);
        }
    }
}

static void tac_atribuicao(ContextoTAC *ctx, const NoAST *no) {
    const NoAST *alvo = no->filho[0];
    const NoAST *expr = no->filho[1];
    if (!alvo || !expr) return;

    if (alvo->tipo == NO_INDICE) {
        char *idx = tac_expressao(ctx, alvo->filho[0]);
        if (no->operador == OP_ATRIB) {
            char *val = tac_expressao(ctx, expr);
            emitir_instrucao_tac(ctx->prog, TAC_ARR_STORE, NULL, idx, val, alvo->texto);
            free(val);
        } else {
            char *t_old = novo_temporario_tac(ctx->prog);
            emitir_instrucao_tac(ctx->prog, TAC_ARR_LOAD, NULL, alvo->texto, idx, t_old);
            char *val = tac_expressao(ctx, expr);
            char *t_res = novo_temporario_tac(ctx->prog);
            emitir_instrucao_tac(ctx->prog, TAC_BINOP, simbolo_op_tac(no->operador),
                                 t_old, val, t_res);
            emitir_instrucao_tac(ctx->prog, TAC_ARR_STORE, NULL, idx, t_res, alvo->texto);
            free(t_old);
            free(val);
            free(t_res);
        }
        free(idx);
    } else {
        const char *nome = alvo->texto ? alvo->texto : "";
        char *val = tac_expressao(ctx, expr);
        if (no->operador == OP_ATRIB) {
            emitir_instrucao_tac(ctx->prog, TAC_COPY, NULL, val, NULL, nome);
        } else {
            char *t_res = novo_temporario_tac(ctx->prog);
            emitir_instrucao_tac(ctx->prog, TAC_BINOP, simbolo_op_tac(no->operador),
                                 nome, val, t_res);
            emitir_instrucao_tac(ctx->prog, TAC_COPY, NULL, t_res, NULL, nome);
            free(t_res);
        }
        free(val);
    }
}

static void tac_inc_dec(ContextoTAC *ctx, const NoAST *no) {
    const NoAST *alvo = no->filho[0];
    if (!alvo) return;
    const char *op_str = simbolo_op_tac(no->operador);

    if (alvo->tipo == NO_INDICE) {
        char *idx = tac_expressao(ctx, alvo->filho[0]);
        char *t_old = novo_temporario_tac(ctx->prog);
        emitir_instrucao_tac(ctx->prog, TAC_ARR_LOAD, NULL, alvo->texto, idx, t_old);
        char *t_res = novo_temporario_tac(ctx->prog);
        emitir_instrucao_tac(ctx->prog, TAC_BINOP, op_str, t_old, "1", t_res);
        emitir_instrucao_tac(ctx->prog, TAC_ARR_STORE, NULL, idx, t_res, alvo->texto);
        free(idx);
        free(t_old);
        free(t_res);
    } else {
        const char *nome = alvo->texto ? alvo->texto : "";
        char *t_res = novo_temporario_tac(ctx->prog);
        emitir_instrucao_tac(ctx->prog, TAC_BINOP, op_str, nome, "1", t_res);
        emitir_instrucao_tac(ctx->prog, TAC_COPY, NULL, t_res, NULL, nome);
        free(t_res);
    }
}

static char *tac_chamada(ContextoTAC *ctx, const NoAST *no, int precisa_resultado) {
    int num_args = 0;
    for (const NoAST *arg = no->filho[0]; arg != NULL; arg = arg->proximo) {
        ++num_args;
    }

    char **vals = NULL;
    if (num_args > 0) {
        vals = calloc((size_t)num_args, sizeof(char *));
        if (!vals) sem_memoria_tac();
        int i = 0;
        for (const NoAST *arg = no->filho[0]; arg != NULL; arg = arg->proximo) {
            vals[i++] = tac_expressao(ctx, arg);
        }
        for (int j = 0; j < num_args; ++j) {
            emitir_instrucao_tac(ctx->prog, TAC_PARAM, NULL, vals[j], NULL, NULL);
            free(vals[j]);
        }
        free(vals);
    }

    char num_buf[32];
    snprintf(num_buf, sizeof(num_buf), "%d", num_args);

    if (precisa_resultado && no->tipo_dado != TIPO_VOID) {
        char *t_res = novo_temporario_tac(ctx->prog);
        emitir_instrucao_tac(ctx->prog, TAC_CALL, NULL, no->texto, num_buf, t_res);
        return t_res;
    }

    emitir_instrucao_tac(ctx->prog, TAC_CALL, NULL, no->texto, num_buf, NULL);
    return NULL;
}

static char *tac_expressao(ContextoTAC *ctx, const NoAST *no) {
    if (!no) return copiar_texto_tac("0");

    switch (no->tipo) {
    case NO_NUM: {
        int n = snprintf(NULL, 0, "%d", no->valor_int);
        if (n < 0) sem_memoria_tac();
        char *s = malloc((size_t)n + 1);
        if (!s) sem_memoria_tac();
        snprintf(s, (size_t)n + 1, "%d", no->valor_int);
        return s;
    }

    case NO_FLOAT: {
        char buf[64];
        snprintf(buf, sizeof(buf), "%g", no->valor_float);
        if (!strchr(buf, '.') && !strchr(buf, 'e') && !strchr(buf, 'E')) {
            size_t len = strlen(buf);
            if (len + 2 < sizeof(buf)) {
                memcpy(buf + len, ".0", 3);
            }
        }
        return copiar_texto_tac(buf);
    }

    case NO_BOOL:
        return copiar_texto_tac(no->valor_int ? "1" : "0");

    case NO_STRING:
        return copiar_texto_tac(no->texto ? no->texto : "\"\"");

    case NO_ID:
        return copiar_texto_tac(no->texto ? no->texto : "");

    case NO_INDICE: {
        char *idx = tac_expressao(ctx, no->filho[0]);
        char *t_res = novo_temporario_tac(ctx->prog);
        emitir_instrucao_tac(ctx->prog, TAC_ARR_LOAD, NULL, no->texto, idx, t_res);
        free(idx);
        return t_res;
    }

    case NO_CHAMADA: {
        char *res = tac_chamada(ctx, no, 1);
        return res ? res : copiar_texto_tac("0");
    }

    case NO_OP_BINARIO: {
        char *esq = tac_expressao(ctx, no->filho[0]);
        char *dir = tac_expressao(ctx, no->filho[1]);
        char *t_res = novo_temporario_tac(ctx->prog);
        emitir_instrucao_tac(ctx->prog, TAC_BINOP, simbolo_op_tac(no->operador),
                             esq, dir, t_res);
        free(esq);
        free(dir);
        return t_res;
    }

    case NO_OP_UNARIO: {
        char *op_val = tac_expressao(ctx, no->filho[0]);
        char *t_res = novo_temporario_tac(ctx->prog);
        emitir_instrucao_tac(ctx->prog, TAC_UNOP, simbolo_op_tac(no->operador),
                             op_val, NULL, t_res);
        free(op_val);
        return t_res;
    }

    case NO_TERNARIO: {
        char *l_falso = novo_label_tac(ctx->prog);
        char *l_fim = novo_label_tac(ctx->prog);
        char *cond = tac_expressao(ctx, no->filho[0]);
        char *t_res = novo_temporario_tac(ctx->prog);
        emitir_instrucao_tac(ctx->prog, TAC_IFZ, NULL, cond, NULL, l_falso);
        char *v = tac_expressao(ctx, no->filho[1]);
        emitir_instrucao_tac(ctx->prog, TAC_COPY, NULL, v, NULL, t_res);
        emitir_instrucao_tac(ctx->prog, TAC_GOTO, NULL, NULL, NULL, l_fim);
        emitir_instrucao_tac(ctx->prog, TAC_LABEL, NULL, NULL, NULL, l_falso);
        char *f = tac_expressao(ctx, no->filho[2]);
        emitir_instrucao_tac(ctx->prog, TAC_COPY, NULL, f, NULL, t_res);
        emitir_instrucao_tac(ctx->prog, TAC_LABEL, NULL, NULL, NULL, l_fim);
        free(l_falso);
        free(l_fim);
        free(cond);
        free(v);
        free(f);
        return t_res;
    }

    case NO_ATRIBUICAO:
        tac_atribuicao(ctx, no);
        if (no->filho[0] && no->filho[0]->texto) {
            return copiar_texto_tac(no->filho[0]->texto);
        }
        return copiar_texto_tac("0");

    case NO_INC_DEC:
        tac_inc_dec(ctx, no);
        if (no->filho[0] && no->filho[0]->texto) {
            return copiar_texto_tac(no->filho[0]->texto);
        }
        return copiar_texto_tac("0");

    default:
        return copiar_texto_tac("0");
    }
}

static void tac_lista_comandos(ContextoTAC *ctx, const NoAST *lista) {
    for (const NoAST *cmd = lista; cmd != NULL; cmd = cmd->proximo) {
        tac_comando(ctx, cmd);
    }
}

static void tac_comando(ContextoTAC *ctx, const NoAST *no) {
    if (!no) return;

    switch (no->tipo) {
    case NO_BLOCO:
        tac_lista_comandos(ctx, no->filho[0]);
        break;

    case NO_DECLARACAO:
        tac_declaracao(ctx, no);
        break;

    case NO_ATRIBUICAO:
        tac_atribuicao(ctx, no);
        break;

    case NO_INC_DEC:
        tac_inc_dec(ctx, no);
        break;

    case NO_CHAMADA: {
        char *res = tac_chamada(ctx, no, 0);
        free(res);
        break;
    }

    case NO_RETURN:
        if (no->filho[0]) {
            char *val = tac_expressao(ctx, no->filho[0]);
            emitir_instrucao_tac(ctx->prog, TAC_RETURN, NULL, val, NULL, NULL);
            free(val);
        } else {
            emitir_instrucao_tac(ctx->prog, TAC_RETURN, NULL, NULL, NULL, NULL);
        }
        break;

    case NO_IF:
        if (!no->filho[2]) {
            char *l_fim = novo_label_tac(ctx->prog);
            char *cond = tac_expressao(ctx, no->filho[0]);
            emitir_instrucao_tac(ctx->prog, TAC_IFZ, NULL, cond, NULL, l_fim);
            tac_comando(ctx, no->filho[1]);
            emitir_instrucao_tac(ctx->prog, TAC_LABEL, NULL, NULL, NULL, l_fim);
            free(cond);
            free(l_fim);
        } else {
            char *l_else = novo_label_tac(ctx->prog);
            char *l_fim = novo_label_tac(ctx->prog);
            char *cond = tac_expressao(ctx, no->filho[0]);
            emitir_instrucao_tac(ctx->prog, TAC_IFZ, NULL, cond, NULL, l_else);
            tac_comando(ctx, no->filho[1]);
            emitir_instrucao_tac(ctx->prog, TAC_GOTO, NULL, NULL, NULL, l_fim);
            emitir_instrucao_tac(ctx->prog, TAC_LABEL, NULL, NULL, NULL, l_else);
            tac_comando(ctx, no->filho[2]);
            emitir_instrucao_tac(ctx->prog, TAC_LABEL, NULL, NULL, NULL, l_fim);
            free(cond);
            free(l_else);
            free(l_fim);
        }
        break;

    case NO_WHILE: {
        char *l_inicio = novo_label_tac(ctx->prog);
        char *l_fim = novo_label_tac(ctx->prog);
        ContextoLacoTAC laco = { l_inicio, l_fim, ctx->laco_atual };
        ctx->laco_atual = &laco;

        emitir_instrucao_tac(ctx->prog, TAC_LABEL, NULL, NULL, NULL, l_inicio);
        char *cond = tac_expressao(ctx, no->filho[0]);
        emitir_instrucao_tac(ctx->prog, TAC_IFZ, NULL, cond, NULL, l_fim);
        free(cond);
        tac_comando(ctx, no->filho[1]);
        emitir_instrucao_tac(ctx->prog, TAC_GOTO, NULL, NULL, NULL, l_inicio);
        emitir_instrucao_tac(ctx->prog, TAC_LABEL, NULL, NULL, NULL, l_fim);

        ctx->laco_atual = laco.anterior;
        free(l_inicio);
        free(l_fim);
        break;
    }

    case NO_DO_WHILE: {
        char *l_inicio = novo_label_tac(ctx->prog);
        char *l_cond = novo_label_tac(ctx->prog);
        char *l_fim = novo_label_tac(ctx->prog);
        ContextoLacoTAC laco = { l_cond, l_fim, ctx->laco_atual };
        ctx->laco_atual = &laco;

        emitir_instrucao_tac(ctx->prog, TAC_LABEL, NULL, NULL, NULL, l_inicio);
        tac_comando(ctx, no->filho[0]);
        emitir_instrucao_tac(ctx->prog, TAC_LABEL, NULL, NULL, NULL, l_cond);
        char *cond = tac_expressao(ctx, no->filho[1]);
        emitir_instrucao_tac(ctx->prog, TAC_IFZ, NULL, cond, NULL, l_fim);
        free(cond);
        emitir_instrucao_tac(ctx->prog, TAC_GOTO, NULL, NULL, NULL, l_inicio);
        emitir_instrucao_tac(ctx->prog, TAC_LABEL, NULL, NULL, NULL, l_fim);

        ctx->laco_atual = laco.anterior;
        free(l_inicio);
        free(l_cond);
        free(l_fim);
        break;
    }

    case NO_FOR: {
        char *l_inicio = novo_label_tac(ctx->prog);
        char *l_passo = novo_label_tac(ctx->prog);
        char *l_fim = novo_label_tac(ctx->prog);

        if (no->filho[0]) {
            tac_comando(ctx, no->filho[0]);
        }

        ContextoLacoTAC laco = { l_passo, l_fim, ctx->laco_atual };
        ctx->laco_atual = &laco;

        emitir_instrucao_tac(ctx->prog, TAC_LABEL, NULL, NULL, NULL, l_inicio);
        if (no->filho[1]) {
            char *cond = tac_expressao(ctx, no->filho[1]);
            emitir_instrucao_tac(ctx->prog, TAC_IFZ, NULL, cond, NULL, l_fim);
            free(cond);
        }
        tac_comando(ctx, no->filho[3]);
        emitir_instrucao_tac(ctx->prog, TAC_LABEL, NULL, NULL, NULL, l_passo);
        if (no->filho[2]) {
            tac_comando(ctx, no->filho[2]);
        }
        emitir_instrucao_tac(ctx->prog, TAC_GOTO, NULL, NULL, NULL, l_inicio);
        emitir_instrucao_tac(ctx->prog, TAC_LABEL, NULL, NULL, NULL, l_fim);

        ctx->laco_atual = laco.anterior;
        free(l_inicio);
        free(l_passo);
        free(l_fim);
        break;
    }

    case NO_BREAK:
        if (ctx->laco_atual && ctx->laco_atual->label_break) {
            emitir_instrucao_tac(ctx->prog, TAC_GOTO, NULL, NULL, NULL,
                                 ctx->laco_atual->label_break);
        }
        break;

    case NO_CONTINUE:
        if (ctx->laco_atual && ctx->laco_atual->label_continue) {
            emitir_instrucao_tac(ctx->prog, TAC_GOTO, NULL, NULL, NULL,
                                 ctx->laco_atual->label_continue);
        }
        break;

    case NO_COUT:
        for (const NoAST *item = no->filho[0]; item != NULL; item = item->proximo) {
            if (item->tipo == NO_ENDL) {
                emitir_instrucao_tac(ctx->prog, TAC_PRINT, NULL, "\"\\n\"", NULL, NULL);
            } else {
                char *val = tac_expressao(ctx, item);
                emitir_instrucao_tac(ctx->prog, TAC_PRINT, NULL, val, NULL, NULL);
                free(val);
            }
        }
        break;

    case NO_CIN:
        for (const NoAST *alvo = no->filho[0]; alvo != NULL; alvo = alvo->proximo) {
            if (alvo->tipo == NO_INDICE) {
                char *idx = tac_expressao(ctx, alvo->filho[0]);
                const char *nome = alvo->texto ? alvo->texto : "";
                int n = snprintf(NULL, 0, "%s[%s]", nome, idx);
                if (n < 0) sem_memoria_tac();
                char *alvo_str = malloc((size_t)n + 1);
                if (!alvo_str) sem_memoria_tac();
                snprintf(alvo_str, (size_t)n + 1, "%s[%s]", nome, idx);
                emitir_instrucao_tac(ctx->prog, TAC_READ, NULL, alvo_str, NULL, NULL);
                free(idx);
                free(alvo_str);
            } else if (alvo->tipo == NO_ID) {
                emitir_instrucao_tac(ctx->prog, TAC_READ, NULL, alvo->texto, NULL, NULL);
            }
        }
        break;

    default: {
        char *val = tac_expressao(ctx, no);
        free(val);
        break;
    }
    }
}

ProgramaTAC *gerar_tac(const NoAST *raiz) {
    if (!raiz) return NULL;

    ProgramaTAC *prog = criar_programa_tac();
    ContextoTAC ctx = { prog, NULL };

    if (raiz->tipo == NO_PROGRAMA) {
        for (const NoAST *item = raiz->filho[0]; item != NULL; item = item->proximo) {
            if (item->tipo == NO_DECLARACAO) {
                tac_declaracao(&ctx, item);
            } else if (item->tipo == NO_FUNCAO) {
                emitir_instrucao_tac(prog, TAC_FUNC_BEGIN, NULL, NULL, NULL, item->texto);
                if (item->filho[1]) {
                    tac_comando(&ctx, item->filho[1]);
                }
                emitir_instrucao_tac(prog, TAC_FUNC_END, NULL, NULL, NULL, item->texto);
            }
        }
    } else {
        tac_comando(&ctx, raiz);
    }

    return prog;
}

void imprimir_tac(const ProgramaTAC *prog, FILE *saida) {
    if (!prog || !saida) return;

    for (size_t i = 0; i < prog->quantidade; ++i) {
        const InstrucaoTAC *inst = &prog->instrucoes[i];
        switch (inst->tipo) {
        case TAC_FUNC_BEGIN:
            fprintf(saida, "func %s:\n", inst->result ? inst->result : "");
            break;
        case TAC_FUNC_END:
            fprintf(saida, "endfunc %s\n", inst->result ? inst->result : "");
            break;
        case TAC_LABEL:
            fprintf(saida, "%s:\n", inst->result ? inst->result : "");
            break;
        case TAC_GOTO:
            fprintf(saida, "    goto %s\n", inst->result ? inst->result : "");
            break;
        case TAC_IFZ:
            fprintf(saida, "    ifZ %s goto %s\n",
                    inst->arg1 ? inst->arg1 : "",
                    inst->result ? inst->result : "");
            break;
        case TAC_COPY:
            fprintf(saida, "    %s = %s\n",
                    inst->result ? inst->result : "",
                    inst->arg1 ? inst->arg1 : "");
            break;
        case TAC_BINOP:
            fprintf(saida, "    %s = %s %s %s\n",
                    inst->result ? inst->result : "",
                    inst->arg1 ? inst->arg1 : "",
                    inst->op ? inst->op : "",
                    inst->arg2 ? inst->arg2 : "");
            break;
        case TAC_UNOP:
            fprintf(saida, "    %s = %s%s\n",
                    inst->result ? inst->result : "",
                    inst->op ? inst->op : "",
                    inst->arg1 ? inst->arg1 : "");
            break;
        case TAC_ARR_LOAD:
            fprintf(saida, "    %s = %s[%s]\n",
                    inst->result ? inst->result : "",
                    inst->arg1 ? inst->arg1 : "",
                    inst->arg2 ? inst->arg2 : "");
            break;
        case TAC_ARR_STORE:
            fprintf(saida, "    %s[%s] = %s\n",
                    inst->result ? inst->result : "",
                    inst->arg1 ? inst->arg1 : "",
                    inst->arg2 ? inst->arg2 : "");
            break;
        case TAC_PARAM:
            fprintf(saida, "    param %s\n", inst->arg1 ? inst->arg1 : "");
            break;
        case TAC_CALL:
            if (inst->result) {
                fprintf(saida, "    %s = call %s, %s\n",
                        inst->result,
                        inst->arg1 ? inst->arg1 : "",
                        inst->arg2 ? inst->arg2 : "0");
            } else {
                fprintf(saida, "    call %s, %s\n",
                        inst->arg1 ? inst->arg1 : "",
                        inst->arg2 ? inst->arg2 : "0");
            }
            break;
        case TAC_RETURN:
            if (inst->arg1) {
                fprintf(saida, "    return %s\n", inst->arg1);
            } else {
                fprintf(saida, "    return\n");
            }
            break;
        case TAC_PRINT:
            fprintf(saida, "    print %s\n", inst->arg1 ? inst->arg1 : "");
            break;
        case TAC_READ:
            fprintf(saida, "    read %s\n", inst->arg1 ? inst->arg1 : "");
            break;
        }
    }
}

void liberar_tac(ProgramaTAC *prog) {
    if (!prog) return;
    for (size_t i = 0; i < prog->quantidade; ++i) {
        free(prog->instrucoes[i].op);
        free(prog->instrucoes[i].arg1);
        free(prog->instrucoes[i].arg2);
        free(prog->instrucoes[i].result);
    }
    free(prog->instrucoes);
    free(prog);
}
