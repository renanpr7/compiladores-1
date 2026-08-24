## Introdução
Projeto da disciplina Compiladores 1 (Engenharia de Software, UnB), desenvolvido com Flex e Bison.

**Nome oficial da linguagem:** [SimC](docs/linguagem-simc.md)

## Pré-requisitos e Setup (WSL/Ubuntu)

### Versões Testadas

| Ferramenta | Versão |
|------------|--------|
| Flex | 2.6.4 |
| Bison | 3.8.2 |
| GCC | 13.3.0 |
| Make | 4.3 |

### Instalação

Para compilar e executar o projeto no ambiente canônico (WSL/Ubuntu), instale as dependências necessárias executando os seguintes comandos:

```bash
sudo apt update
sudo apt install flex bison build-essential
```

### Build e Teste

```bash
# Compilar o compilador
make

# Rodar os golden tests
make test

# Limpar artefatos gerados
make clean
```

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

## Nota

- O professor `sergioaafreitas` precisa ser adicionado como colaborador no repositório (ação pelo dono `renanpr7`).
