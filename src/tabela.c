#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
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

static char sufixo_tipo(TipoDado t) {
    switch (t) {
    case TIPO_INT:    return 'i';
    case TIPO_FLOAT:  return 'f';
    case TIPO_BOOL:   return 'b';
    case TIPO_STRING: return 's';
    case TIPO_VOID:   return 'v';
    case TIPO_NENHUM: return 'u';
    }
    return 'u';
}

static char *gerar_nome_mangled(const char *nome, const TipoDado *tipos_param, int num_params) {
    size_t len_nome = strlen(nome);
    size_t sufixos = (num_params > 0) ? (size_t)num_params : 1u;
    size_t total = len_nome + 1u + sufixos + 1u;
    char *res = calloc(total, sizeof(char));
    if (!res) erro_memoria_tabela();

    memcpy(res, nome, len_nome);
    res[len_nome] = '_';
    if (num_params <= 0) {
        res[len_nome + 1u] = 'v';
        res[len_nome + 2u] = '\0';
    } else {
        for (int i = 0; i < num_params; ++i) {
            res[len_nome + 1u + (size_t)i] = sufixo_tipo(tipos_param[i]);
        }
        res[len_nome + 1u + (size_t)num_params] = '\0';
    }
    return res;
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
    free(s->nome_mangled);
    free(s->tipos_param);
    free(s->ref_params);
    if (s->valores_padrao) {
        for (int i = 0; i < s->num_params; ++i)
            liberarAST(s->valores_padrao[i]);
    }
    free(s->valores_padrao);
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

Simbolo *buscar_funcao_compativel(TabelaSimbolos *t, const char *nome,
                                  const TipoDado *tipos_args, int num_args,
                                  StatusBuscaFuncao *status_out) {
    if (status_out) *status_out = BUSCA_FUNC_NAO_DECLARADA;
    if (!t || !nome) return NULL;

    uint32_t idx = hash_fnv1a(nome);
    Escopo *escopo_func = NULL;
    Simbolo *primeira_func = NULL;

    for (Escopo *e = t->atual; e != NULL; e = e->pai) {
        for (Simbolo *s = e->buckets[idx]; s != NULL; s = s->proximo) {
            if (strcmp(s->nome, nome) == 0) {
                if (s->categoria != CAT_FUNCAO) {
                    if (status_out) *status_out = BUSCA_FUNC_NAO_DECLARADA;
                    return NULL;
                }
                escopo_func = e;
                primeira_func = s;
                break;
            }
        }
        if (escopo_func) break;
    }

    if (!escopo_func || !primeira_func) {
        if (status_out) *status_out = BUSCA_FUNC_NAO_DECLARADA;
        return NULL;
    }

    /* Anti-cascata: se algum argumento teve erro anterior (TIPO_NENHUM), nao reporta novo erro */
    for (int i = 0; i < num_args; ++i) {
        if (tipos_args && tipos_args[i] == TIPO_NENHUM) {
            if (status_out) *status_out = BUSCA_FUNC_OK;
            return primeira_func;
        }
    }

    /* Passada 1: casamento exato de tipos */
    Simbolo *match_exato = NULL;
    int count_exato = 0;
    for (Simbolo *s = escopo_func->buckets[idx]; s != NULL; s = s->proximo) {
        if (s->categoria == CAT_FUNCAO && strcmp(s->nome, nome) == 0 &&
            num_args >= s->min_params && num_args <= s->num_params) {
            int exato = 1;
            for (int i = 0; i < num_args; ++i) {
                if (tipos_args && s->tipos_param[i] != tipos_args[i]) {
                    exato = 0;
                    break;
                }
            }
            if (exato) {
                match_exato = s;
                ++count_exato;
            }
        }
    }
    if (count_exato == 1) {
        if (status_out) *status_out = BUSCA_FUNC_OK;
        return match_exato;
    }
    if (count_exato > 1) {
        if (status_out) *status_out = BUSCA_FUNC_AMBIGUA;
        return NULL;
    }

    if (status_out) *status_out = BUSCA_FUNC_SEM_CANDIDATO;
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
    int ja_existe_sobrecarga = 0;
    for (Simbolo *s = t->atual->buckets[idx]; s != NULL; s = s->proximo) {
        if (strcmp(s->nome, nome) == 0) {
            if (s->categoria != CAT_FUNCAO || mesma_assinatura(s, params, num_params)) {
                return NULL;
            }
            ja_existe_sobrecarga = 1;
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
        novo->valores_padrao = calloc((size_t)num_params, sizeof(NoAST *));
        if (!novo->tipos_param || !novo->ref_params || !novo->valores_padrao)
            erro_memoria_tabela();
        NoAST *p = params;
        for (int i = 0; i < num_params; ++i) {
            novo->tipos_param[i] = p->tipo_dado;
            novo->ref_params[i] = p->eh_referencia;
            if (p->filho[0])
                novo->valores_padrao[i] = clonarExpressaoAST(p->filho[0]);
            p = p->proximo;
        }
    }

    novo->nome_mangled = gerar_nome_mangled(novo->nome, novo->tipos_param, num_params);

    if (ja_existe_sobrecarga) {
        novo->eh_sobrecarregada = 1;
        for (Simbolo *s = t->atual->buckets[idx]; s != NULL; s = s->proximo) {
            if (s->categoria == CAT_FUNCAO && strcmp(s->nome, novo->nome) == 0) {
                if (!s->eh_sobrecarregada) {
                    s->eh_sobrecarregada = 1;
                    if (s->no_decl && s->nome_mangled) {
                        free(s->no_decl->texto);
                        s->no_decl->texto = copiar_nome(s->nome_mangled);
                    }
                }
            }
        }
        if (novo->no_decl && novo->nome_mangled) {
            free(novo->no_decl->texto);
            novo->no_decl->texto = copiar_nome(novo->nome_mangled);
        }
    }

    novo->proximo = t->atual->buckets[idx];
    t->atual->buckets[idx] = novo;
    return novo;
}
