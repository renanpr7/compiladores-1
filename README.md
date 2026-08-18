## Introdução
Projeto da disciplina Compiladores 1 (Engenharia de Software, UnB), desenvolvido com Flex e Bison.

## Padrões do Repositório

### 1. Nomenclatura de Branches
Padrão: `[ÁREA] - [descrição-curta]`

* LEX - Análise Léxica
* SIN - Análise Sintática
* SEM - Análise Semântica
* GER - Geração de Código
* DOC - Documentação
* Exemplo: `SIN-precedencia-operadores`

### 2. Mensagens de Commit
Inicie os commits com os seguintes prefixos:

* [ADD]: Criação de arquivos ou funcionalidades.
* [FIX]: Correção de bugs.
* [DOCS]: Atualização do relatório ou README.
* [REF]: Refatoração e organização de código.
* Exemplo: `[ADD] reconhecimento de tokens numericos`

## Guia de Contribuição

### Tutorial: Fluxo Git (Terminal)
Siga este passo a passo para cada nova edição:

1. `git pull origin main` (Atualiza seu local)
2. `git checkout -b [NOME_DA_SUA_BRANCH]` (Cria sua ramificação)
3. Faça suas alterações.
4. `git add .`
5. `git commit -m "[PREFIXO] descrição curta"`
6. `git push origin [NOME_DA_SUA_BRANCH]`
7. Abra o Pull Request no site do GitHub.