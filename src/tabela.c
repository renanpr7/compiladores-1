#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tabela.h"

static void erro_memoria_tabela(void) {
    fprintf(stderr, "Erro: memoria insuficiente na tabela de simbolos\n");
    exit(EXIT_FAILURE);
}

static char *copiar_nome(const char *str) {
    char *copia = strdup(str);
    if (!copia) erro_memoria_tabela();
    return copia;
}

static uint32_t hash_fnv1a(const char *str) {
    uint32_t h = 2166136261u;
    for (const unsigned char *p = (const unsigned char *)str; *p != '\0'; ++p) {
        h ^= (uint32_t)(*p);
        h *= 16777619u;
    }
    return h % TABELA_BUCKETS;
}

static void liberar_simbolo(Simbolo *s) {
    if (!s) return;
    free(s->nome);
    free(s->tipos_param);
    free(s->ref_params);
    free(s);
}

TabelaSimbolos *criar_tabela(void) {
    TabelaSimbolos *t = calloc(1, sizeof *t);
    if (!t) erro_memoria_tabela();
    entrar_escopo(t);
    t->global = t->atual;
    return t;
}

void liberar_tabela(TabelaSimbolos *t) {
    if (!t) return;
    while (t->atual) {
        sair_escopo(t);
    }
    free(t);
}

int entrar_escopo(TabelaSimbolos *t) {
    if (!t) return 0;
    Escopo *novo = calloc(1, sizeof *novo);
    if (!novo) erro_memoria_tabela();
    novo->pai = t->atual;
    t->atual = novo;
    return 1;
}

void sair_escopo(TabelaSimbolos *t) {
    if (!t || !t->atual) return;
    Escopo *escopo = t->atual;
    t->atual = escopo->pai;
    if (t->global == escopo) {
        t->global = NULL;
    }
    for (int i = 0; i < TABELA_BUCKETS; ++i) {
        Simbolo *s = escopo->buckets[i];
        while (s) {
            Simbolo *prox = s->proximo;
            liberar_simbolo(s);
            s = prox;
        }
    }
    free(escopo);
}

Simbolo *buscar_simbolo_escopo_atual(TabelaSimbolos *t, const char *nome) {
    if (!t || !t->atual || !nome) return NULL;
    uint32_t idx = hash_fnv1a(nome);
    for (Simbolo *s = t->atual->buckets[idx]; s != NULL; s = s->proximo) {
        if (strcmp(s->nome, nome) == 0) {
            return s;
        }
    }
    return NULL;
}

Simbolo *buscar_simbolo(TabelaSimbolos *t, const char *nome) {
    if (!t || !nome) return NULL;
    uint32_t idx = hash_fnv1a(nome);
    for (Escopo *e = t->atual; e != NULL; e = e->pai) {
        for (Simbolo *s = e->buckets[idx]; s != NULL; s = s->proximo) {
            if (strcmp(s->nome, nome) == 0) {
                return s;
            }
        }
    }
    return NULL;
}

static int param_compativel(TipoDado esperado, TipoDado recebido) {
    if (esperado == TIPO_NENHUM || recebido == TIPO_NENHUM) return 1;
    if (esperado == TIPO_VOID || recebido == TIPO_VOID) return 0;
    if (esperado == TIPO_STRING || recebido == TIPO_STRING) {
        return esperado == TIPO_STRING && recebido == TIPO_STRING;
    }
    return 1;
}

Simbolo *buscar_funcao_compativel(TabelaSimbolos *t, const char *nome,
                                  const TipoDado *tipos_args, int num_args) {
    if (!t || !nome) return NULL;
    uint32_t idx = hash_fnv1a(nome);

    for (Escopo *e = t->atual; e != NULL; e = e->pai) {
        /* Passada 1: casamento exato de tipos */
        for (Simbolo *s = e->buckets[idx]; s != NULL; s = s->proximo) {
            if (s->categoria == CAT_FUNCAO && strcmp(s->nome, nome) == 0 &&
                num_args >= s->min_params && num_args <= s->num_params) {
                int exato = 1;
                for (int i = 0; i < num_args; ++i) {
                    if (tipos_args && s->tipos_param[i] != tipos_args[i]) {
                        exato = 0;
                        break;
                    }
                }
                if (exato) return s;
            }
        }
        /* Passada 2: casamento compativel por coercao */
        for (Simbolo *s = e->buckets[idx]; s != NULL; s = s->proximo) {
            if (s->categoria == CAT_FUNCAO && strcmp(s->nome, nome) == 0 &&
                num_args >= s->min_params && num_args <= s->num_params) {
                int compativel = 1;
                for (int i = 0; i < num_args; ++i) {
                    if (tipos_args && !param_compativel(s->tipos_param[i], tipos_args[i])) {
                        compativel = 0;
                        break;
                    }
                }
                if (compativel) return s;
            }
        }
    }
    return NULL;
}

Simbolo *inserir_simbolo(TabelaSimbolos *t, const char *nome, TipoDado tipo,
                         CategoriaSimbolo categoria, int linha, int coluna) {
    if (!t || !t->atual || !nome) return NULL;
    if (buscar_simbolo_escopo_atual(t, nome) != NULL) {
        return NULL;
    }
    uint32_t idx = hash_fnv1a(nome);
    Simbolo *novo = calloc(1, sizeof *novo);
    if (!novo) erro_memoria_tabela();
    novo->nome = copiar_nome(nome);
    novo->tipo = tipo;
    novo->categoria = categoria;
    novo->linha = linha;
    novo->coluna = coluna;
    novo->proximo = t->atual->buckets[idx];
    t->atual->buckets[idx] = novo;
    return novo;
}

static int mesma_assinatura(const Simbolo *s, NoAST *params, int num_params) {
    if (s->num_params != num_params) return 0;
    NoAST *p = params;
    for (int i = 0; i < num_params; ++i) {
        if (!p || s->tipos_param[i] != p->tipo_dado) {
            return 0;
        }
        p = p->proximo;
    }
    return 1;
}

Simbolo *inserir_funcao(TabelaSimbolos *t, const char *nome, TipoDado retorno,
                        NoAST *params, NoAST *no_decl, int linha, int coluna) {
    if (!t || !t->atual || !nome) return NULL;

    int num_params = 0;
    int min_params = 0;
    for (NoAST *p = params; p != NULL; p = p->proximo) {
        ++num_params;
        if (p->filho[0] == NULL) {
            ++min_params;
        }
    }

    uint32_t idx = hash_fnv1a(nome);
    for (Simbolo *s = t->atual->buckets[idx]; s != NULL; s = s->proximo) {
        if (strcmp(s->nome, nome) == 0) {
            if (s->categoria != CAT_FUNCAO || mesma_assinatura(s, params, num_params)) {
                return NULL;
            }
        }
    }

    Simbolo *novo = calloc(1, sizeof *novo);
    if (!novo) erro_memoria_tabela();
    novo->nome = copiar_nome(nome);
    novo->tipo = retorno;
    novo->categoria = CAT_FUNCAO;
    novo->num_params = num_params;
    novo->min_params = min_params;
    novo->no_decl = no_decl;
    novo->linha = linha;
    novo->coluna = coluna;

    if (num_params > 0) {
        novo->tipos_param = calloc((size_t)num_params, sizeof(TipoDado));
        novo->ref_params = calloc((size_t)num_params, sizeof(int));
        if (!novo->tipos_param || !novo->ref_params) erro_memoria_tabela();
        NoAST *p = params;
        for (int i = 0; i < num_params; ++i) {
            novo->tipos_param[i] = p->tipo_dado;
            novo->ref_params[i] = p->eh_referencia;
            p = p->proximo;
        }
    }

    novo->proximo = t->atual->buckets[idx];
    t->atual->buckets[idx] = novo;
    return novo;
}
