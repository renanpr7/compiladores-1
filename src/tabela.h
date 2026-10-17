#ifndef TABELA_H
#define TABELA_H

#include "tipos.h"

#define TABELA_BUCKETS 64

typedef enum {
    CAT_VARIAVEL,
    CAT_PARAMETRO,
    CAT_FUNCAO
} CategoriaSimbolo;

typedef enum {
    BUSCA_FUNC_OK,
    BUSCA_FUNC_NAO_DECLARADA,
    BUSCA_FUNC_SEM_CANDIDATO,
    BUSCA_FUNC_AMBIGUA
} StatusBuscaFuncao;

typedef struct Simbolo {
    char *nome;
    char *nome_mangled;
    TipoDado tipo;
    CategoriaSimbolo categoria;
    int eh_const;
    int eh_referencia;
    int eh_array;
    int tamanho_array;
    int eh_sobrecarregada;
    int num_params;
    int min_params;
    TipoDado *tipos_param;
    int *ref_params;
    NoAST **valores_padrao;
    NoAST *no_decl;
    int linha;
    int coluna;
    struct Simbolo *proximo;
} Simbolo;

typedef struct Escopo {
    Simbolo *buckets[TABELA_BUCKETS];
    struct Escopo *pai;
} Escopo;

typedef struct TabelaSimbolos {
    Escopo *atual;
    Escopo *global;
} TabelaSimbolos;

TabelaSimbolos *criar_tabela(void);
void liberar_tabela(TabelaSimbolos *t);

int entrar_escopo(TabelaSimbolos *t);
void sair_escopo(TabelaSimbolos *t);

Simbolo *buscar_simbolo_escopo_atual(TabelaSimbolos *t, const char *nome);
Simbolo *buscar_simbolo(TabelaSimbolos *t, const char *nome);
Simbolo *buscar_funcao_compativel(TabelaSimbolos *t, const char *nome,
                                  const TipoDado *tipos_args, int num_args,
                                  StatusBuscaFuncao *status_out);

Simbolo *inserir_simbolo(TabelaSimbolos *t, const char *nome, TipoDado tipo,
                         CategoriaSimbolo categoria, int linha, int coluna);
Simbolo *inserir_funcao(TabelaSimbolos *t, const char *nome, TipoDado retorno,
                        NoAST *params, NoAST *no_decl, int linha, int coluna);

#endif
