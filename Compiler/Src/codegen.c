#include "codeGen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static const char* map_type(const ASTNode* type_node) {
    if (!type_node) return "void";

    if (type_node->type == NODE_TYPE_PRIMITIVE) {
        if (strcmp(type_node->primitive_type.type_name, "Number") == 0) return "int";
        if (strcmp(type_node->primitive_type.type_name, "Letter") == 0) return "char*";
        if (strcmp(type_node->primitive_type.type_name, "Void") == 0) return "void";
        return type_node->primitive_type.type_name;
    }

    if (type_node->type == NODE_TYPE_ARRAY) {
        return "int*";
    }

    return "int";
}

void generate_expression(ASTNode* node, FILE* output) {
    if (!node) {
        fprintf(stderr, "Error: Expresión nula\n");
        return;
    }

    switch (node->type) {
        case NODE_LITERAL_NUMBER:
            assert(node->literal.value && "Literal number value is NULL");
            fprintf(output, "%s", node->literal.value);
            break;

        case NODE_LITERAL_LETTER:
            assert(node->literal.value && "Literal letter value is NULL");
            fprintf(output, "\"%s\"", node->literal.value);
            break;

        case NODE_IDENTIFIER:
            assert(node->literal.value && "Identifier name is NULL");
            fprintf(output, "%s", node->literal.value);
            break;

        case NODE_BINARY_OP: {
            assert(node->binary_op.left && "Binary op left operand is NULL");
            assert(node->binary_op.right && "Binary op right operand is NULL");

            const char* op = "";
            switch(node->binary_op.op) {
                case TOKEN_PLUS: op = "+"; break;
                case TOKEN_MINUS: op = "-"; break;
                case TOKEN_MUL: op = "*"; break;
                case TOKEN_DIV: op = "/"; break;
                case TOKEN_EQ: op = "=="; break;
                case TOKEN_NEQ: op = "!="; break;
                case TOKEN_LT: op = "<"; break;
                case TOKEN_GT: op = ">"; break;
                case TOKEN_LEQ: op = "<="; break;
                case TOKEN_GEQ: op = ">="; break;
                default:
                    fprintf(stderr, "Error: Operador binario desconocido\n");
                    op = "??";
                    break;
            }
            fprintf(output, "(");
            generate_expression(node->binary_op.left, output);
            fprintf(output, " %s ", op);
            generate_expression(node->binary_op.right, output);
            fprintf(output, ")");
            break;
        }

        case NODE_FUNCTION_CALL:
            assert(node->function_call.func && "Function call func is NULL");
            fprintf(output, "%s(", node->function_call.func->literal.value);
            for (size_t i = 0; i < node->function_call.arg_count; i++) {
                assert(node->function_call.args && "Function call args array is NULL");
                assert(node->function_call.args[i] && "Function call arg is NULL");
                generate_expression(node->function_call.args[i], output);
                if (i < node->function_call.arg_count - 1) fprintf(output, ", ");
            }
            fprintf(output, ")");
            break;

        case NODE_ARRAY_ACCESS:
            assert(node->array_access.array && "Array access array is NULL");
            assert(node->array_access.index && "Array access index is NULL");
            generate_expression(node->array_access.array, output);
            fprintf(output, "[");
            generate_expression(node->array_access.index, output);
            fprintf(output, "]");
            break;

        default:
            fprintf(stderr, "Error: Tipo de expresión no soportado en generación de código\n");
            exit(1);
    }
}

void generate_statement(ASTNode* node, FILE* output) {
    if (!node) {
        fprintf(stderr, "Error: Intento de generar declaración nula\n");
        return;
    }

    switch (node->type) {
        case NODE_VARIABLE: {
            assert(node->variable.name && "Variable name is NULL");
            assert(node->variable.var_type && "Variable type is NULL");

            const char* type = map_type(node->variable.var_type);

            if (!node->variable.is_mutable) {
                fprintf(output, "const ");
            }

            fprintf(output, "%s %s", type, node->variable.name);

            if (node->variable.value) {
                fprintf(output, " = ");
                generate_expression(node->variable.value, output);
            } else {
                if (strcmp(type, "char*") == 0) fprintf(output, " = NULL");
                else if (strcmp(type, "int") == 0) fprintf(output, " = 0");
            }
            fprintf(output, ";\n");
            break;
        }

        case NODE_PRINT:
            assert(node->print_stmt.args && "Print args array is NULL");
            assert(node->print_stmt.arg_count >0  && "Print requires at least one argument");
            assert(node->print_stmt.args[0] && "First print argument is NULL");

            fprintf(output, "printf(");
            if (node->print_stmt.args[0]->type == NODE_LITERAL_LETTER) {
                fprintf(output, "\"%%s\\n\", ");
            } else {
                fprintf(output, "\"%%d\\n\", ");
            }
            generate_expression(node->print_stmt.args[0], output);
            fprintf(output, ");\n");
            break;

        case NODE_RETURN:
            fprintf(output, "return");
            if (node->return_stmt.value) {
                fprintf(output, " ");
                generate_expression(node->return_stmt.value, output);
            }
            fprintf(output, ";\n");
            break;

        case NODE_IF:
            assert(node->if_stmt.condition && "If condition is NULL");

            fprintf(output, "if (");
            generate_expression(node->if_stmt.condition, output);
            fprintf(output, ") {\n");

            for (size_t i = 0; i < node->if_stmt.then_count; i++) {
                assert(node->if_stmt.then_branch && "If then_branch array is NULL");
                assert(node->if_stmt.then_branch[i] && "If then_branch statement is NULL");
                fprintf(output, "    ");
                generate_statement(node->if_stmt.then_branch[i], output);
            }

            if (node->if_stmt.else_count > 0) {
                assert(node->if_stmt.else_branch && "If else_branch array is NULL");
                fprintf(output, "} else {\n");
                for (size_t i = 0; i < node->if_stmt.else_count; i++) {
                    assert(node->if_stmt.else_branch[i] && "If else_branch statement is NULL");
                    fprintf(output, "    ");
                    generate_statement(node->if_stmt.else_branch[i], output);
                }
            }
            fprintf(output, "}\n");
            break;

        default:
            generate_expression(node, output);
            fprintf(output, ";\n");
            break;
    }
}

void generate_function(ASTNode* node, FILE* output) {
    if (!node || node->type != NODE_FUNCTION) {
        fprintf(stderr, "Error: Nodo de función inválido\n");
        return;
    }

    assert(node->function.name && "Function name is NULL");
    assert(node->function.return_type && "Function return type is NULL");

    const char* ret_type = map_type(node->function.return_type);
    fprintf(output, "%s %s(", ret_type, node->function.name);

    for (size_t i = 0; i < node->function.param_count; i++) {
        assert(node->function.params && "Function params array is NULL");
        assert(node->function.params[i] && "Function param is NULL");
        assert(node->function.params[i]->variable.var_type && "Function param type is NULL");
        assert(node->function.params[i]->variable.name && "Function param name is NULL");

        const char* param_type = map_type(node->function.params[i]->variable.var_type);
        fprintf(output, "%s %s", param_type, node->function.params[i]->variable.name);
        if (i < node->function.param_count - 1) fprintf(output, ", ");
    }

    fprintf(output, ") {\n");

    for (size_t i = 0; i < node->function.body_count; i++) {
        assert(node->function.body && "Function body array is NULL");
        assert(node->function.body[i] && "Function body statement is NULL");
        fprintf(output, "    ");
        generate_statement(node->function.body[i], output);
    }

    if (strcmp(ret_type, "void") == 0 &&
        (node->function.body_count == 0 ||
         node->function.body[node->function.body_count-1]->type != NODE_RETURN)) {
        fprintf(output, "    return;\n");
    }

    fprintf(output, "}\n\n");
}

void generate_code(ASTNode* node, const char* output_filename) {
    if (!node || node->type != NODE_PROGRAM) {
        fprintf(stderr, "Error: Nodo de programa inválido\n");
        return;
    }

    FILE* output = fopen(output_filename, "w");
    if (!output) {
        perror("Error al abrir archivo de salida");
        return;
    }

    fprintf(output, "#include <stdio.h>\n");
    fprintf(output, "#include <stdlib.h>\n\n");

    for (size_t i = 0; i < node->program.function_count; i++) {
        assert(node->program.functions && "Program functions array is NULL");
        assert(node->program.functions[i] && "Program function is NULL");

        ASTNode* func = node->program.functions[i];
        const char* ret_type = map_type(func->function.return_type);
        const char* func_name = func->function.name;
        if (strcmp(func_name, "main") == 0) {
            func_name = "Main";
        }

        fprintf(output, "%s %s(", ret_type, func_name);

        for (size_t j = 0; j < func->function.param_count; j++) {
            assert(func->function.params && "Function params array is NULL");
            assert(func->function.params[j] && "Function param is NULL");

            const char* param_type = map_type(func->function.params[j]->variable.var_type);
            fprintf(output, "%s %s", param_type, func->function.params[j]->variable.name);
            if (j < func->function.param_count - 1) fprintf(output, ", ");
        }
        fprintf(output, ");\n");
    }
    fprintf(output, "\n");

    for (size_t i = 0; i < node->program.function_count; i++) {
        generate_function(node->program.functions[i], output);
    }

    fprintf(output, "int main(){\n");
    bool has_main = false;
    for (size_t i = 0; i < node->program.function_count; i++) {
        if (strcmp(node->program.functions[i]->function.name, "main") == 0) {
            has_main = true;
            const char* ret_type = map_type(node->program.functions[i]->function.return_type);
            if (strcmp(ret_type, "Void") == 0) {
                fprintf(output,"Main();\n");
                fprintf(output, "return 0;\n");
            }
            {
                fprintf(output, "return Main();\n");
            }
            break;
        }
    }

    if (!has_main) {
        fprintf(output, "puts(\"error no se encontro un main\");\n");
        fprintf(output, "return 0;\n");
    }
    fprintf(output, "}\n\n");

    fclose(output);
}
