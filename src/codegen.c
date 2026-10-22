#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

typedef struct {
    FILE *out;
    int nivel_indent;
} ContextoCodegen;

static void emitir_indent(ContextoCodegen *ctx) {
    for (int i = 0; i < ctx->nivel_indent; ++i) {
        fprintf(ctx->out, "    ");
    }
}

static const char *tipo_para_c(TipoDado tipo) {
    switch (tipo) {
    case TIPO_INT:    return "int";
    case TIPO_FLOAT:  return "float";
    case TIPO_BOOL:   return "int";
    case TIPO_STRING: return "char*";
    case TIPO_VOID:   return "void";
    default:          return "int";
    }
}

static const char *op_simbolo(Operador op) {
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
    case OP_ATRIB:       return "=";
    case OP_ATRIB_SOMA:  return "+=";
    case OP_ATRIB_SUB:   return "-=";
    case OP_ATRIB_MULT:  return "*=";
    case OP_ATRIB_DIV:   return "/=";
    case OP_INC:         return "++";
    case OP_DEC:         return "--";
    default:             return "";
    }
}

static void codegen_expressao(ContextoCodegen *ctx, const NoAST *no);
static void codegen_comando(ContextoCodegen *ctx, const NoAST *no);
static void codegen_bloco(ContextoCodegen *ctx, const NoAST *no);
static void codegen_corpo(ContextoCodegen *ctx, const NoAST *no);

static void codegen_expressao(ContextoCodegen *ctx, const NoAST *no) {
    if (!no) return;

    switch (no->tipo) {
    case NO_NUM:
        fprintf(ctx->out, "%d", no->valor_int);
        break;

    case NO_FLOAT: {
        char buf[64];
        snprintf(buf, sizeof(buf), "%g", no->valor_float);
        if (!strchr(buf, '.') && !strchr(buf, 'e') && !strchr(buf, 'E')) {
            strcat(buf, ".0");
        }
        fprintf(ctx->out, "%s", buf);
        break;
    }

    case NO_BOOL:
        fprintf(ctx->out, "%d", no->valor_int ? 1 : 0);
        break;

    case NO_STRING:
        fprintf(ctx->out, "%s", no->texto ? no->texto : "\"\"");
        break;

    case NO_ID:
        fprintf(ctx->out, "%s", no->texto ? no->texto : "");
        break;

    case NO_INDICE:
        fprintf(ctx->out, "%s[", no->texto ? no->texto : "");
        codegen_expressao(ctx, no->filho[0]);
        fprintf(ctx->out, "]");
        break;

    case NO_CHAMADA:
        fprintf(ctx->out, "%s(", no->texto ? no->texto : "");
        for (const NoAST *arg = no->filho[0]; arg != NULL; arg = arg->proximo) {
            codegen_expressao(ctx, arg);
            if (arg->proximo) fprintf(ctx->out, ", ");
        }
        fprintf(ctx->out, ")");
        break;

    case NO_OP_BINARIO:
        fprintf(ctx->out, "(");
        codegen_expressao(ctx, no->filho[0]);
        fprintf(ctx->out, " %s ", op_simbolo(no->operador));
        codegen_expressao(ctx, no->filho[1]);
        fprintf(ctx->out, ")");
        break;

    case NO_OP_UNARIO:
        fprintf(ctx->out, "%s", op_simbolo(no->operador));
        if (no->filho[0] && (no->filho[0]->tipo == NO_OP_BINARIO || no->filho[0]->tipo == NO_TERNARIO)) {
            fprintf(ctx->out, "(");
            codegen_expressao(ctx, no->filho[0]);
            fprintf(ctx->out, ")");
        } else {
            codegen_expressao(ctx, no->filho[0]);
        }
        break;

    case NO_TERNARIO:
        fprintf(ctx->out, "(");
        codegen_expressao(ctx, no->filho[0]);
        fprintf(ctx->out, " ? ");
        codegen_expressao(ctx, no->filho[1]);
        fprintf(ctx->out, " : ");
        codegen_expressao(ctx, no->filho[2]);
        fprintf(ctx->out, ")");
        break;

    case NO_ATRIBUICAO:
        codegen_expressao(ctx, no->filho[0]);
        fprintf(ctx->out, " %s ", op_simbolo(no->operador));
        codegen_expressao(ctx, no->filho[1]);
        break;

    case NO_INC_DEC:
        codegen_expressao(ctx, no->filho[0]);
        fprintf(ctx->out, "%s", op_simbolo(no->operador));
        break;

    default:
        break;
    }
}

static void codegen_declaracao_inline(ContextoCodegen *ctx, const NoAST *no) {
    if (no->eh_const) {
        fprintf(ctx->out, "const ");
    }
    fprintf(ctx->out, "%s ", tipo_para_c(no->tipo_dado));

    for (const NoAST *decl = no->filho[0]; decl != NULL; decl = decl->proximo) {
        fprintf(ctx->out, "%s", decl->texto ? decl->texto : "");
        if (decl->filho[1]) {
            fprintf(ctx->out, "[");
            codegen_expressao(ctx, decl->filho[1]);
            fprintf(ctx->out, "]");
        }
        if (decl->filho[0]) {
            fprintf(ctx->out, " = ");
            codegen_expressao(ctx, decl->filho[0]);
        }
        if (decl->proximo) {
            fprintf(ctx->out, ", ");
        }
    }
}

static void codegen_declaracao(ContextoCodegen *ctx, const NoAST *no) {
    emitir_indent(ctx);
    codegen_declaracao_inline(ctx, no);
    fprintf(ctx->out, ";\n");
}

static void codegen_funcao(ContextoCodegen *ctx, const NoAST *no) {
    emitir_indent(ctx);
    fprintf(ctx->out, "%s %s(", tipo_para_c(no->tipo_dado), no->texto ? no->texto : "");
    if (!no->filho[0]) {
        fprintf(ctx->out, "void");
    } else {
        for (const NoAST *param = no->filho[0]; param != NULL; param = param->proximo) {
            fprintf(ctx->out, "%s %s", tipo_para_c(param->tipo_dado), param->texto ? param->texto : "");
            if (param->proximo) fprintf(ctx->out, ", ");
        }
    }
    fprintf(ctx->out, ") ");

    if (no->filho[1]) {
        if (no->filho[1]->tipo == NO_BLOCO) {
            codegen_bloco(ctx, no->filho[1]);
        } else {
            fprintf(ctx->out, "{\n");
            ctx->nivel_indent++;
            codegen_comando(ctx, no->filho[1]);
            ctx->nivel_indent--;
            emitir_indent(ctx);
            fprintf(ctx->out, "}\n");
        }
    } else {
        fprintf(ctx->out, "{}\n");
    }
    fprintf(ctx->out, "\n");
}

static const char *formato_io(TipoDado tipo) {
    switch (tipo) {
    case TIPO_FLOAT:  return "%f";
    case TIPO_STRING: return "%s";
    case TIPO_INT:
    case TIPO_BOOL:   return "%d";
    default:          return NULL;
    }
}

static void codegen_cout(ContextoCodegen *ctx, const NoAST *no) {
    emitir_indent(ctx);
    fprintf(ctx->out, "printf(\"");
    for (const NoAST *item = no->filho[0]; item != NULL; item = item->proximo) {
        if (item->tipo == NO_ENDL) {
            fprintf(ctx->out, "\\n");
        } else {
            const char *formato = formato_io(item->tipo_dado);
            fprintf(ctx->out, "%s", formato ? formato : "%d");
        }
    }
    fprintf(ctx->out, "\"");

    for (const NoAST *item = no->filho[0]; item != NULL; item = item->proximo) {
        if (item->tipo == NO_ENDL) continue;
        fprintf(ctx->out, ", ");
        codegen_expressao(ctx, item);
    }
    fprintf(ctx->out, ");\n");
}

static void codegen_cin(ContextoCodegen *ctx, const NoAST *no) {
    emitir_indent(ctx);
    fprintf(ctx->out, "scanf(\"");
    for (const NoAST *alvo = no->filho[0]; alvo != NULL; alvo = alvo->proximo) {
        const char *formato = formato_io(alvo->tipo_dado);
        fprintf(ctx->out, "%s", formato ? formato : "%d");
    }
    fprintf(ctx->out, "\"");

    for (const NoAST *alvo = no->filho[0]; alvo != NULL; alvo = alvo->proximo) {
        fprintf(ctx->out, ", &");
        if (alvo->tipo == NO_INDICE) {
            fprintf(ctx->out, "%s[", alvo->texto ? alvo->texto : "");
            codegen_expressao(ctx, alvo->filho[0]);
            fprintf(ctx->out, "]");
        } else {
            fprintf(ctx->out, "%s", alvo->texto ? alvo->texto : "");
        }
    }
    fprintf(ctx->out, ");\n");
}

static void codegen_bloco(ContextoCodegen *ctx, const NoAST *no) {
    fprintf(ctx->out, "{\n");
    ctx->nivel_indent++;
    for (const NoAST *cmd = no->filho[0]; cmd != NULL; cmd = cmd->proximo) {
        codegen_comando(ctx, cmd);
    }
    ctx->nivel_indent--;
    emitir_indent(ctx);
    fprintf(ctx->out, "}\n");
}

static void codegen_corpo(ContextoCodegen *ctx, const NoAST *no) {
    if (no && no->tipo == NO_BLOCO) {
        codegen_bloco(ctx, no);
        return;
    }

    fprintf(ctx->out, "{\n");
    ctx->nivel_indent++;
    codegen_comando(ctx, no);
    ctx->nivel_indent--;
    emitir_indent(ctx);
    fprintf(ctx->out, "}\n");
}

static void codegen_for_init(ContextoCodegen *ctx, const NoAST *no) {
    if (!no) return;
    if (no->tipo == NO_DECLARACAO) {
        codegen_declaracao_inline(ctx, no);
    } else {
        codegen_expressao(ctx, no);
    }
}

static void codegen_comando(ContextoCodegen *ctx, const NoAST *no) {
    if (!no) return;

    switch (no->tipo) {
    case NO_BLOCO:
        emitir_indent(ctx);
        codegen_bloco(ctx, no);
        break;

    case NO_DECLARACAO:
        codegen_declaracao(ctx, no);
        break;

    case NO_RETURN:
        emitir_indent(ctx);
        fprintf(ctx->out, "return");
        if (no->filho[0]) {
            fprintf(ctx->out, " ");
            codegen_expressao(ctx, no->filho[0]);
        }
        fprintf(ctx->out, ";\n");
        break;

    case NO_IF:
        emitir_indent(ctx);
        fprintf(ctx->out, "if (");
        codegen_expressao(ctx, no->filho[0]);
        fprintf(ctx->out, ") ");
        codegen_corpo(ctx, no->filho[1]);
        if (no->filho[2]) {
            emitir_indent(ctx);
            fprintf(ctx->out, "else ");
            codegen_corpo(ctx, no->filho[2]);
        }
        break;

    case NO_WHILE:
        emitir_indent(ctx);
        fprintf(ctx->out, "while (");
        codegen_expressao(ctx, no->filho[0]);
        fprintf(ctx->out, ") ");
        codegen_corpo(ctx, no->filho[1]);
        break;

    case NO_DO_WHILE:
        emitir_indent(ctx);
        fprintf(ctx->out, "do ");
        codegen_corpo(ctx, no->filho[0]);
        emitir_indent(ctx);
        fprintf(ctx->out, "while (");
        codegen_expressao(ctx, no->filho[1]);
        fprintf(ctx->out, ");\n");
        break;

    case NO_FOR:
        emitir_indent(ctx);
        fprintf(ctx->out, "for (");
        codegen_for_init(ctx, no->filho[0]);
        fprintf(ctx->out, "; ");
        codegen_expressao(ctx, no->filho[1]);
        fprintf(ctx->out, "; ");
        codegen_expressao(ctx, no->filho[2]);
        fprintf(ctx->out, ") ");
        codegen_corpo(ctx, no->filho[3]);
        break;

    case NO_BREAK:
        emitir_indent(ctx);
        fprintf(ctx->out, "break;\n");
        break;

    case NO_CONTINUE:
        emitir_indent(ctx);
        fprintf(ctx->out, "continue;\n");
        break;

    case NO_COUT:
        codegen_cout(ctx, no);
        break;

    case NO_CIN:
        codegen_cin(ctx, no);
        break;

    case NO_ATRIBUICAO:
    case NO_CHAMADA:
    case NO_INC_DEC:
    case NO_OP_BINARIO:
    case NO_OP_UNARIO:
    case NO_TERNARIO:
    case NO_ID:
    case NO_INDICE:
    case NO_NUM:
    case NO_FLOAT:
    case NO_BOOL:
    case NO_STRING:
        emitir_indent(ctx);
        codegen_expressao(ctx, no);
        fprintf(ctx->out, ";\n");
        break;

    default:
        /* Outros comandos (if, while, etc.) serao tratados em fases futuras */
        break;
    }
}

int gerar_codigo(const NoAST *raiz, FILE *saida) {
    if (!raiz || !saida) return 1;

    ContextoCodegen ctx;
    ctx.out = saida;
    ctx.nivel_indent = 0;

    if (raiz->tipo == NO_PROGRAMA) {
        fprintf(ctx.out, "#include <stdio.h>\n\n");
        for (const NoAST *item = raiz->filho[0]; item != NULL; item = item->proximo) {
            if (item->tipo == NO_DECLARACAO) {
                codegen_declaracao(&ctx, item);
            } else if (item->tipo == NO_FUNCAO) {
                codegen_funcao(&ctx, item);
            }
        }
    }

    return 0;
}
