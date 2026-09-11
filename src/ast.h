#ifndef AST_H
#define AST_H

#include "tipos.h"

/*
 * Filhos de cada no, pela posicao em filho[]. Filho NULL e parte opcional
 * ausente. Listas (declaracoes, parametros, comandos, argumentos, itens de
 * cout e cin) sao encadeadas por 'proximo'.
 *
 *   NO_PROGRAMA     [0] declaracoes e funcoes
 *   NO_FUNCAO       [0] parametros  [1] corpo
 *   NO_PARAMETRO    [0] valor padrao
 *   NO_DECLARACAO   [0] declaradores
 *   NO_DECLARADOR   [0] valor inicial  [1] tamanho do array
 *   NO_BLOCO        [0] comandos
 *   NO_IF           [0] condicao  [1] entao  [2] senao
 *   NO_WHILE        [0] condicao  [1] corpo
 *   NO_DO_WHILE     [0] corpo  [1] condicao
 *   NO_FOR          [0] inicio  [1] condicao  [2] passo  [3] corpo
 *   NO_RETURN       [0] expressao
 *   NO_COUT         [0] itens (expressao ou NO_ENDL)
 *   NO_CIN          [0] alvos (NO_ID ou NO_INDICE)
 *   NO_INC_DEC      [0] alvo
 *   NO_ATRIBUICAO   [0] alvo  [1] valor
 *   NO_OP_BINARIO   [0] esquerda  [1] direita
 *   NO_OP_UNARIO    [0] operando
 *   NO_TERNARIO     [0] condicao  [1] se verdadeiro  [2] se falso
 *   NO_CHAMADA      [0] argumentos
 *   NO_INDICE       [0] indice
 *
 * Os construtores copiam as strings recebidas; quem chama continua dono do
 * original. NO_STRING guarda o literal como o lexer entrega, com as aspas.
 */

NoAST *criarNoPrograma(NoAST *declaracoes);
NoAST *criarNoFuncao(TipoDado retorno, const char *nome, NoAST *parametros, NoAST *corpo,
                     int linha, int coluna);
NoAST *criarNoParam(TipoDado tipo, const char *nome, int linha, int coluna);
NoAST *criarNoParamRef(TipoDado tipo, const char *nome, int linha, int coluna);
NoAST *criarNoParamPadrao(TipoDado tipo, const char *nome, NoAST *padrao, int linha, int coluna);
NoAST *criarNoDeclaracao(TipoDado tipo, int eh_const, NoAST *declaradores, int linha, int coluna);
NoAST *criarNoDeclarador(const char *nome, NoAST *inicial, NoAST *tamanho, int linha, int coluna);

NoAST *criarNoBloco(NoAST *comandos, int linha, int coluna);
NoAST *criarNoIf(NoAST *condicao, NoAST *entao, NoAST *senao, int linha, int coluna);
NoAST *criarNoWhile(NoAST *condicao, NoAST *corpo, int linha, int coluna);
NoAST *criarNoDoWhile(NoAST *corpo, NoAST *condicao, int linha, int coluna);
NoAST *criarNoFor(NoAST *inicio, NoAST *condicao, NoAST *passo, NoAST *corpo,
                  int linha, int coluna);
NoAST *criarNoReturn(NoAST *expressao, int linha, int coluna);
NoAST *criarNoBreak(int linha, int coluna);
NoAST *criarNoContinue(int linha, int coluna);
NoAST *criarNoCout(NoAST *itens, int linha, int coluna);
NoAST *criarNoCin(NoAST *alvos, int linha, int coluna);
NoAST *criarNoEndl(int linha, int coluna);
NoAST *criarNoIncDec(Operador operador, NoAST *alvo, int linha, int coluna);

NoAST *criarNoAtribuicao(Operador operador, NoAST *alvo, NoAST *valor, int linha, int coluna);
NoAST *criarNoOp(Operador operador, NoAST *esq, NoAST *dir, int linha, int coluna);
NoAST *criarNoOpUnario(Operador operador, NoAST *operando, int linha, int coluna);
NoAST *criarNoTernario(NoAST *condicao, NoAST *se_verdadeiro, NoAST *se_falso,
                       int linha, int coluna);
NoAST *criarNoChamada(const char *nome, NoAST *argumentos, int linha, int coluna);
NoAST *criarNoIndice(const char *nome, NoAST *indice, int linha, int coluna);
NoAST *criarNoId(const char *nome, int linha, int coluna);
NoAST *criarNoNum(int valor, int linha, int coluna);
NoAST *criarNoFloat(float valor, int linha, int coluna);
NoAST *criarNoBool(int valor, int linha, int coluna);
NoAST *criarNoString(const char *literal, int linha, int coluna);

/* Anexa 'novo' ao fim da lista e devolve o inicio. Aceita lista ou novo NULL. */
NoAST *anexarIrmao(NoAST *lista, NoAST *novo);

void imprimirAST(NoAST *raiz);
void liberarAST(NoAST *raiz);

#endif
