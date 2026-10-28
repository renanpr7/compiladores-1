#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantica.h"
#include "tabela.h"
#include "diagnostics.h"

typedef struct {
    TabelaSimbolos *tabela;
    TipoDado retorno_funcao_atual;
    int erros;
    int profundidade_laco;
} ContextoSemantico;

static void erro_semantico(ContextoSemantico *ctx, int linha, int coluna, const char *fmt, ...) {
    ctx->erros++;
    if (report_diagnostic()) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "Erro semantico na linha %d, coluna %d: ", linha, coluna);
        vfprintf(stderr, fmt, args);
        fputc('\n', stderr);
        va_end(args);
    }
}

static int eh_numerico(TipoDado t) {
    return t == TIPO_INT || t == TIPO_FLOAT;
}

static int eh_escalar_condicao(TipoDado t) {
    return t == TIPO_INT || t == TIPO_FLOAT || t == TIPO_BOOL;
}

static char *copiar_texto_semantica(const char *texto) {
    size_t tamanho = strlen(texto) + 1;
    char *copia = malloc(tamanho);
    if (!copia) {
        fprintf(stderr, "Erro: memoria insuficiente na analise semantica\n");
        exit(EXIT_FAILURE);
    }
    memcpy(copia, texto, tamanho);
    return copia;
}

static int tipos_compativeis_atribuicao(TipoDado destino, TipoDado origem) {
    if (destino == TIPO_NENHUM || origem == TIPO_NENHUM) return 1;
    if (destino == TIPO_VOID || origem == TIPO_VOID) return 0;
    if (destino == TIPO_STRING || origem == TIPO_STRING) {
        return destino == TIPO_STRING && origem == TIPO_STRING;
    }
    return eh_escalar_condicao(destino) && eh_escalar_condicao(origem);
}

static void validar_condicao(ContextoSemantico *ctx, const NoAST *cmd, const NoAST *cond) {
    if (!cond || cond->tipo_dado == TIPO_NENHUM) return;
    if (!eh_escalar_condicao(cond->tipo_dado)) {
        erro_semantico(ctx, cmd->linha, cmd->coluna,
                       "condicao deve ser do tipo bool ou int");
    }
}

static void visitar_no(ContextoSemantico *ctx, NoAST *no);

static void visitar_lista(ContextoSemantico *ctx, NoAST *lista) {
    for (NoAST *item = lista; item != NULL; item = item->proximo) {
        visitar_no(ctx, item);
    }
}

static void analisar_declaracao(ContextoSemantico *ctx, NoAST *no) {
    for (NoAST *decl = no->filho[0]; decl != NULL; decl = decl->proximo) {
        decl->tipo_dado = no->tipo_dado;
        decl->eh_const = no->eh_const;

        if (decl->filho[1]) {
            visitar_no(ctx, decl->filho[1]);
            if (decl->filho[1]->tipo_dado != TIPO_NENHUM &&
                decl->filho[1]->tipo_dado != TIPO_INT) {
                erro_semantico(ctx, decl->linha, decl->coluna,
                               "indice de array deve ser do tipo int");
            }
        }
        if (decl->filho[0]) {
            visitar_no(ctx, decl->filho[0]);
            if (!tipos_compativeis_atribuicao(no->tipo_dado, decl->filho[0]->tipo_dado)) {
                erro_semantico(ctx, decl->linha, decl->coluna,
                               "tipos incompativeis na atribuicao");
            }
        }

        Simbolo *s = inserir_simbolo(ctx->tabela, decl->texto, no->tipo_dado,
                                     CAT_VARIAVEL, decl->linha, decl->coluna);
        if (!s) {
            erro_semantico(ctx, decl->linha, decl->coluna,
                           "redeclaracao de '%s' no mesmo escopo", decl->texto);
        } else {
            s->eh_const = no->eh_const;
            s->eh_array = (decl->filho[1] != NULL);
            if (decl->filho[1] && decl->filho[1]->tipo == NO_NUM) {
                s->tamanho_array = decl->filho[1]->valor_int;
            }
        }
    }
}

static void analisar_funcao(ContextoSemantico *ctx, NoAST *no) {
    Simbolo *f = inserir_funcao(ctx->tabela, no->texto, no->tipo_dado,
                                no->filho[0], no, no->linha, no->coluna);
    if (!f) {
        erro_semantico(ctx, no->linha, no->coluna,
                       "redeclaracao de '%s' no mesmo escopo", no->texto);
    }

    TipoDado retorno_anterior = ctx->retorno_funcao_atual;
    ctx->retorno_funcao_atual = no->tipo_dado;

    entrar_escopo(ctx->tabela);

    for (NoAST *param = no->filho[0]; param != NULL; param = param->proximo) {
        if (param->eh_referencia) {
            erro_semantico(ctx, param->linha, param->coluna,
                           "referencias ainda nao suportadas");
        }
        if (param->filho[0]) {
            visitar_no(ctx, param->filho[0]);
            if (!tipos_compativeis_atribuicao(param->tipo_dado, param->filho[0]->tipo_dado)) {
                erro_semantico(ctx, param->linha, param->coluna,
                               "tipos incompativeis na atribuicao");
            }
        }
        Simbolo *p = inserir_simbolo(ctx->tabela, param->texto, param->tipo_dado,
                                     CAT_PARAMETRO, param->linha, param->coluna);
        if (!p) {
            erro_semantico(ctx, param->linha, param->coluna,
                           "redeclaracao de '%s' no mesmo escopo", param->texto);
        } else {
            p->eh_referencia = param->eh_referencia;
        }
    }

    /* Os defaults sao tipados durante a visita acima. A tabela guarda copias
     * independentes, portanto atualize-as depois dessa visita. */
    if (f && f->valores_padrao) {
        int indice = 0;
        for (NoAST *param = no->filho[0]; param != NULL; param = param->proximo, ++indice) {
            liberarAST(f->valores_padrao[indice]);
            f->valores_padrao[indice] = param->filho[0]
                ? clonarExpressaoAST(param->filho[0]) : NULL;
        }
    }

    if (no->filho[1]) {
        if (no->filho[1]->tipo == NO_BLOCO) {
            visitar_lista(ctx, no->filho[1]->filho[0]);
        } else {
            visitar_no(ctx, no->filho[1]);
        }
    }

    sair_escopo(ctx->tabela);
    ctx->retorno_funcao_atual = retorno_anterior;
}

static void analisar_chamada(ContextoSemantico *ctx, NoAST *no) {
    visitar_lista(ctx, no->filho[0]);

    int num_args = 0;
    for (NoAST *arg = no->filho[0]; arg != NULL; arg = arg->proximo) {
        ++num_args;
    }

    TipoDado *tipos_args = NULL;
    if (num_args > 0) {
        tipos_args = calloc((size_t)num_args, sizeof(TipoDado));
        if (!tipos_args) {
            fprintf(stderr, "Erro: memoria insuficiente na analise semantica\n");
            exit(EXIT_FAILURE);
        }
        int i = 0;
        for (NoAST *arg = no->filho[0]; arg != NULL; arg = arg->proximo) {
            tipos_args[i++] = arg->tipo_dado;
        }
    }

    StatusBuscaFuncao status = BUSCA_FUNC_NAO_DECLARADA;
    Simbolo *comp = buscar_funcao_compativel(ctx->tabela, no->texto, tipos_args, num_args, &status);

    switch (status) {
    case BUSCA_FUNC_NAO_DECLARADA:
        erro_semantico(ctx, no->linha, no->coluna,
                       "funcao '%s' nao declarada", no->texto);
        no->tipo_dado = TIPO_NENHUM;
        break;
    case BUSCA_FUNC_SEM_CANDIDATO:
        erro_semantico(ctx, no->linha, no->coluna,
                       "nenhum candidato compativel para chamada de '%s'", no->texto);
        no->tipo_dado = TIPO_NENHUM;
        break;
    case BUSCA_FUNC_AMBIGUA:
        erro_semantico(ctx, no->linha, no->coluna,
                       "chamada ambigua para funcao '%s'", no->texto);
        no->tipo_dado = TIPO_NENHUM;
        break;
    case BUSCA_FUNC_OK:
        if (comp) {
            no->tipo_dado = comp->tipo;
            /* Os defaults pertencem a tabela, mas cada chamada recebe donos
             * proprios. Assim liberar a AST da funcao e da chamada nunca
             * percorre o mesmo no duas vezes. */
            for (int i = num_args; i < comp->num_params; ++i) {
                if (!comp->valores_padrao || !comp->valores_padrao[i]) {
                    erro_semantico(ctx, no->linha, no->coluna,
                                   "argumento obrigatorio ausente na chamada de '%s'", no->texto);
                    no->tipo_dado = TIPO_NENHUM;
                    break;
                }
                NoAST *padrao = clonarExpressaoAST(comp->valores_padrao[i]);
                no->filho[0] = anexarIrmao(no->filho[0], padrao);
            }
            if (comp->eh_sobrecarregada && comp->nome_mangled) {
                char *copia = copiar_texto_semantica(comp->nome_mangled);
                free(no->texto);
                no->texto = copia;
            }
        } else {
            no->tipo_dado = TIPO_NENHUM;
        }
        break;
    }

    free(tipos_args);
}

/* A segunda declaracao pode promover uma funcao que ja teve seu corpo
 * analisado. Nesse caso, chamadas visitadas antes da promocao precisam ser
 * atualizadas retroativamente para que a AST inteira use o mesmo nome C. */
static void atualizar_nomes_chamadas(ContextoSemantico *ctx, NoAST *no) {
    for (NoAST *atual = no; atual != NULL; atual = atual->proximo) {
        if (atual->tipo == NO_CHAMADA) {
            int quantidade = 0;
            for (NoAST *arg = atual->filho[0]; arg != NULL; arg = arg->proximo)
                ++quantidade;

            TipoDado *tipos = NULL;
            if (quantidade > 0) {
                tipos = calloc((size_t)quantidade, sizeof *tipos);
                if (!tipos) {
                    fprintf(stderr, "Erro: memoria insuficiente na analise semantica\n");
                    exit(EXIT_FAILURE);
                }
                int i = 0;
                for (NoAST *arg = atual->filho[0]; arg != NULL; arg = arg->proximo)
                    tipos[i++] = arg->tipo_dado;
            }

            StatusBuscaFuncao status = BUSCA_FUNC_NAO_DECLARADA;
            Simbolo *funcao = buscar_funcao_compativel(ctx->tabela, atual->texto,
                                                       tipos, quantidade, &status);
            if (status == BUSCA_FUNC_OK && funcao && funcao->eh_sobrecarregada &&
                funcao->nome_mangled) {
                char *nome = copiar_texto_semantica(funcao->nome_mangled);
                free(atual->texto);
                atual->texto = nome;
            }
            free(tipos);
        }

        for (int i = 0; i < AST_MAX_FILHOS; ++i)
            atualizar_nomes_chamadas(ctx, atual->filho[i]);
    }
}

static void visitar_no(ContextoSemantico *ctx, NoAST *no) {
    if (!no) return;

    switch (no->tipo) {
    case NO_PROGRAMA:
        visitar_lista(ctx, no->filho[0]);
        break;

    case NO_FUNCAO:
        analisar_funcao(ctx, no);
        break;

    case NO_PARAMETRO:
        break;

    case NO_DECLARACAO:
        analisar_declaracao(ctx, no);
        break;

    case NO_DECLARADOR:
        break;

    case NO_BLOCO:
        entrar_escopo(ctx->tabela);
        visitar_lista(ctx, no->filho[0]);
        sair_escopo(ctx->tabela);
        break;

    case NO_IF:
        visitar_no(ctx, no->filho[0]);
        validar_condicao(ctx, no, no->filho[0]);
        visitar_no(ctx, no->filho[1]);
        visitar_no(ctx, no->filho[2]);
        break;

    case NO_WHILE:
        visitar_no(ctx, no->filho[0]);
        validar_condicao(ctx, no, no->filho[0]);
        ctx->profundidade_laco++;
        visitar_no(ctx, no->filho[1]);
        ctx->profundidade_laco--;
        break;

    case NO_DO_WHILE:
        ctx->profundidade_laco++;
        visitar_no(ctx, no->filho[0]);
        ctx->profundidade_laco--;
        visitar_no(ctx, no->filho[1]);
        validar_condicao(ctx, no, no->filho[1]);
        break;

    case NO_FOR:
        entrar_escopo(ctx->tabela);
        visitar_no(ctx, no->filho[0]);
        visitar_no(ctx, no->filho[1]);
        validar_condicao(ctx, no, no->filho[1]);
        visitar_no(ctx, no->filho[2]);
        ctx->profundidade_laco++;
        visitar_no(ctx, no->filho[3]);
        ctx->profundidade_laco--;
        sair_escopo(ctx->tabela);
        break;

    case NO_RETURN: {
        NoAST *expr = no->filho[0];
        if (expr) {
            visitar_no(ctx, expr);
        }
        if (ctx->retorno_funcao_atual == TIPO_VOID) {
            if (expr != NULL) {
                erro_semantico(ctx, no->linha, no->coluna,
                               "tipo de retorno incompativel com a funcao");
            }
        } else {
            if (expr == NULL) {
                erro_semantico(ctx, no->linha, no->coluna,
                               "tipo de retorno incompativel com a funcao");
            } else if (expr->tipo_dado != TIPO_NENHUM &&
                       !tipos_compativeis_atribuicao(ctx->retorno_funcao_atual, expr->tipo_dado)) {
                erro_semantico(ctx, no->linha, no->coluna,
                               "tipo de retorno incompativel com a funcao");
            }
        }
        break;
    }

    case NO_INC_DEC: {
        NoAST *alvo = no->filho[0];
        visitar_no(ctx, alvo);
        no->tipo_dado = alvo ? alvo->tipo_dado : TIPO_NENHUM;
        if (alvo && alvo->tipo_dado != TIPO_NENHUM && alvo->eh_const) {
            erro_semantico(ctx, no->linha, no->coluna,
                           "atribuicao para variavel constante '%s'",
                           alvo->texto ? alvo->texto : "");
        }
        break;
    }

    case NO_OP_UNARIO: {
        NoAST *op = no->filho[0];
        visitar_no(ctx, op);
        if (!op || op->tipo_dado == TIPO_NENHUM) {
            no->tipo_dado = TIPO_NENHUM;
            break;
        }
        if (no->operador == OP_NEG) {
            if (eh_numerico(op->tipo_dado)) {
                no->tipo_dado = op->tipo_dado;
            } else {
                erro_semantico(ctx, no->linha, no->coluna,
                               "tipos incompativeis na operacao");
                no->tipo_dado = TIPO_NENHUM;
            }
        } else if (no->operador == OP_NAO) {
            if (eh_escalar_condicao(op->tipo_dado)) {
                no->tipo_dado = TIPO_BOOL;
            } else {
                erro_semantico(ctx, no->linha, no->coluna,
                               "tipos incompativeis na operacao");
                no->tipo_dado = TIPO_NENHUM;
            }
        } else {
            no->tipo_dado = TIPO_NENHUM;
        }
        break;
    }

    case NO_BREAK:
    case NO_CONTINUE:
        if (ctx->profundidade_laco == 0)
            erro_semantico(ctx, no->linha, no->coluna, "'%s' fora de um laco",
                           no->tipo == NO_BREAK ? "break" : "continue");
        break;

    case NO_ENDL:
        break;

    case NO_COUT:
    case NO_CIN:
        visitar_lista(ctx, no->filho[0]);
        break;

    case NO_ATRIBUICAO: {
        NoAST *alvo = no->filho[0];
        NoAST *valor = no->filho[1];
        visitar_no(ctx, alvo);
        visitar_no(ctx, valor);
        no->tipo_dado = alvo ? alvo->tipo_dado : TIPO_NENHUM;
        if (alvo && alvo->tipo_dado != TIPO_NENHUM) {
            if (alvo->eh_const) {
                erro_semantico(ctx, no->linha, no->coluna,
                               "atribuicao para variavel constante '%s'",
                               alvo->texto ? alvo->texto : "");
            } else if (valor && valor->tipo_dado != TIPO_NENHUM) {
                if (no->operador != OP_ATRIB &&
                    (!eh_numerico(alvo->tipo_dado) || !eh_numerico(valor->tipo_dado))) {
                    erro_semantico(ctx, no->linha, no->coluna,
                                    "tipos incompativeis na atribuicao");
                } else if (!tipos_compativeis_atribuicao(alvo->tipo_dado, valor->tipo_dado)) {
                    erro_semantico(ctx, no->linha, no->coluna,
                                   "tipos incompativeis na atribuicao");
                }
            }
        }
        break;
    }

    case NO_OP_BINARIO: {
        NoAST *esq = no->filho[0];
        NoAST *dir = no->filho[1];
        visitar_no(ctx, esq);
        visitar_no(ctx, dir);
        if (!esq || !dir ||
            esq->tipo_dado == TIPO_NENHUM || dir->tipo_dado == TIPO_NENHUM) {
            no->tipo_dado = TIPO_NENHUM;
            break;
        }
        TipoDado te = esq->tipo_dado;
        TipoDado td = dir->tipo_dado;
        switch (no->operador) {
        case OP_SOMA:
        case OP_SUB:
        case OP_MULT:
        case OP_DIV:
            if (eh_numerico(te) && eh_numerico(td)) {
                no->tipo_dado = (te == TIPO_FLOAT || td == TIPO_FLOAT) ? TIPO_FLOAT : TIPO_INT;
            } else {
                erro_semantico(ctx, no->linha, no->coluna,
                               "tipos incompativeis na operacao");
                no->tipo_dado = TIPO_NENHUM;
            }
            break;
        case OP_MOD:
            if (te == TIPO_INT && td == TIPO_INT) {
                no->tipo_dado = TIPO_INT;
            } else {
                erro_semantico(ctx, no->linha, no->coluna,
                               "tipos incompativeis na operacao");
                no->tipo_dado = TIPO_NENHUM;
            }
            break;
        case OP_MENOR:
        case OP_MAIOR:
        case OP_MENOR_IGUAL:
        case OP_MAIOR_IGUAL:
            if (eh_escalar_condicao(te) && eh_escalar_condicao(td)) {
                no->tipo_dado = TIPO_BOOL;
            } else {
                erro_semantico(ctx, no->linha, no->coluna,
                               "tipos incompativeis na operacao");
                no->tipo_dado = TIPO_NENHUM;
            }
            break;
        case OP_IGUAL:
        case OP_DIFERENTE:
            if ((eh_escalar_condicao(te) && eh_escalar_condicao(td)) ||
                (te == TIPO_STRING && td == TIPO_STRING)) {
                no->tipo_dado = TIPO_BOOL;
            } else {
                erro_semantico(ctx, no->linha, no->coluna,
                               "tipos incompativeis na operacao");
                no->tipo_dado = TIPO_NENHUM;
            }
            break;
        case OP_E:
        case OP_OU:
            if (eh_escalar_condicao(te) && eh_escalar_condicao(td)) {
                no->tipo_dado = TIPO_BOOL;
            } else {
                erro_semantico(ctx, no->linha, no->coluna,
                               "tipos incompativeis na operacao");
                no->tipo_dado = TIPO_NENHUM;
            }
            break;
        default:
            no->tipo_dado = TIPO_NENHUM;
            break;
        }
        break;
    }

    case NO_TERNARIO: {
        NoAST *cond = no->filho[0];
        NoAST *verd = no->filho[1];
        NoAST *falso = no->filho[2];
        visitar_no(ctx, cond);
        visitar_no(ctx, verd);
        visitar_no(ctx, falso);
        if (cond && cond->tipo_dado != TIPO_NENHUM && !eh_escalar_condicao(cond->tipo_dado)) {
            erro_semantico(ctx, no->linha, no->coluna,
                           "condicao deve ser do tipo bool ou int");
        }
        if (!verd || !falso ||
            verd->tipo_dado == TIPO_NENHUM || falso->tipo_dado == TIPO_NENHUM) {
            no->tipo_dado = TIPO_NENHUM;
            break;
        }
        TipoDado tv = verd->tipo_dado;
        TipoDado tf = falso->tipo_dado;
        if (tv == tf && tv != TIPO_VOID) {
            no->tipo_dado = tv;
        } else if (eh_escalar_condicao(tv) && eh_escalar_condicao(tf)) {
            no->tipo_dado = (tv == TIPO_FLOAT || tf == TIPO_FLOAT) ? TIPO_FLOAT : TIPO_INT;
        } else {
            erro_semantico(ctx, no->linha, no->coluna,
                           "tipos incompativeis na operacao");
            no->tipo_dado = TIPO_NENHUM;
        }
        break;
    }

    case NO_CHAMADA:
        analisar_chamada(ctx, no);
        break;

    case NO_INDICE: {
        Simbolo *s = buscar_simbolo(ctx->tabela, no->texto);
        if (!s || s->categoria == CAT_FUNCAO) {
            erro_semantico(ctx, no->linha, no->coluna,
                           "identificador '%s' nao declarado", no->texto);
            no->tipo_dado = TIPO_NENHUM;
        } else {
            no->tipo_dado = s->tipo;
            no->eh_const = s->eh_const;
        }
        NoAST *idx = no->filho[0];
        visitar_no(ctx, idx);
        if (idx && idx->tipo_dado != TIPO_NENHUM && idx->tipo_dado != TIPO_INT) {
            erro_semantico(ctx, no->linha, no->coluna,
                           "indice de array deve ser do tipo int");
        }
        break;
    }

    case NO_ID: {
        Simbolo *s = buscar_simbolo(ctx->tabela, no->texto);
        if (!s || s->categoria == CAT_FUNCAO) {
            erro_semantico(ctx, no->linha, no->coluna,
                           "identificador '%s' nao declarado", no->texto);
            no->tipo_dado = TIPO_NENHUM;
        } else {
            no->tipo_dado = s->tipo;
            no->eh_const = s->eh_const;
            no->eh_referencia = s->eh_referencia;
        }
        break;
    }

    case NO_NUM:
        no->tipo_dado = TIPO_INT;
        break;

    case NO_FLOAT:
        no->tipo_dado = TIPO_FLOAT;
        break;

    case NO_BOOL:
        no->tipo_dado = TIPO_BOOL;
        break;

    case NO_STRING:
        no->tipo_dado = TIPO_STRING;
        break;
    }
}

int analisar_semantica(NoAST *raiz) {
    if (!raiz) return 0;
    ContextoSemantico ctx;
    ctx.tabela = criar_tabela();
    ctx.retorno_funcao_atual = TIPO_VOID;
    ctx.erros = 0;
    ctx.profundidade_laco = 0;

    visitar_no(&ctx, raiz);
    atualizar_nomes_chamadas(&ctx, raiz);

    liberar_tabela(ctx.tabela);
    return ctx.erros;
}
