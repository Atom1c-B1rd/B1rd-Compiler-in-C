
#include "Ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

ASTNode* create_program() {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_PROGRAM;
    node->program.functions = NULL;
    node->program.function_count = 0;
    return node;
}

ASTNode* create_function(char* name, ASTNode** params, size_t param_count,
                         ASTNode* return_type, ASTNode** body, size_t body_count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_FUNCTION;
    node->function.name = strdup(name);
    node->function.params = params;
    node->function.param_count = param_count;
    node->function.return_type = return_type;
    node->function.body = body;
    node->function.body_count = body_count;
    return node;
}

ASTNode* create_variable(char* name, ASTNode* var_type, ASTNode* value, bool is_mutable) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_VARIABLE;
    node->variable.name = strdup(name);
    node->variable.var_type = var_type;
    node->variable.value = value;
    node->variable.is_mutable = is_mutable;
    return node;
}

ASTNode* create_literal(char* value, NodeType literal_type) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = literal_type;
    node->literal.value = strdup(value);
    return node;
}

ASTNode* create_binary_op(TokenType op, ASTNode* left, ASTNode* right) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_BINARY_OP;
    node->binary_op.op = op;
    node->binary_op.left = left;
    node->binary_op.right = right;
    return node;
}

ASTNode* create_unary_op(TokenType op, ASTNode* expr) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_UNARY_OP;
    node->unary_op.op = op;
    node->unary_op.expr = expr;
    return node;
}

ASTNode* create_array_access(ASTNode* array, ASTNode* index) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ARRAY_ACCESS;
    node->array_access.array = array;
    node->array_access.index = index;
    return node;
}

ASTNode* create_array_literal(ASTNode** elements, size_t count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_ARRAY_LITERAL;
    node->array_literal.elements = elements;
    node->array_literal.count = count;
    return node;
}

ASTNode* create_function_call(ASTNode* func, ASTNode** args, size_t arg_count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_FUNCTION_CALL;
    node->function_call.func = func;
    node->function_call.args = args;
    node->function_call.arg_count = arg_count;
    return node;
}

ASTNode* create_if(ASTNode* condition, ASTNode** then_branch, size_t then_count,ASTNode* elseif_branch,
                    size_t elseif_count,ASTNode** else_branch, size_t else_count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_IF;
    node->if_stmt.condition = condition;
    node->if_stmt.then_branch = then_branch;
    node->if_stmt.then_count = then_count;
    node->if_stmt.elseif_branch = elseif_branch;
    node->if_stmt.elseif_count = elseif_count;
    node->if_stmt.else_branch = else_branch;
    node->if_stmt.else_count = else_count;
    return node;
}

ASTNode* create_switch(ASTNode* expr, ASTNode** cases, size_t case_count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_SWITCH;
    node->switch_stmt.expr = expr;
    node->switch_stmt.cases = cases;
    node->switch_stmt.case_count = case_count;
    return node;
}

ASTNode* create_case(ASTNode* value, ASTNode** body, size_t body_count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_CASE;
    node->case_stmt.value = value;
    node->case_stmt.body = body;
    node->case_stmt.body_count = body_count;
    return node;
}

ASTNode* create_primitive_type(char* type_name) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_TYPE_PRIMITIVE;
    node->primitive_type.type_name = strdup(type_name);
    return node;
}

ASTNode* create_array_type(ASTNode* base_type) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_TYPE_ARRAY;
    node->array_type.base_type = base_type;
    return node;
}
ASTNode* create_print(ASTNode** args, size_t args_count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_PRINT;
    node->print_stmt.args = args;
    node->print_stmt.arg_count = args_count;
    return node;
}

void free_ast(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NODE_PROGRAM:
            for (size_t i = 0; i < node->program.function_count; i++) {
                free_ast(node->program.functions[i]);
            }
            free(node->program.functions);
            break;

        case NODE_FUNCTION:
            free(node->function.name);
            for (size_t i = 0; i < node->function.param_count; i++) {
                free_ast(node->function.params[i]);
            }
            free(node->function.params);
            free_ast(node->function.return_type);
            for (size_t i = 0; i < node->function.body_count; i++) {
                free_ast(node->function.body[i]);
            }
            free(node->function.body);
            break;

        case NODE_VARIABLE:
            free(node->variable.name);
            free_ast(node->variable.var_type);
            free_ast(node->variable.value);
            break;

        case NODE_LITERAL:
        case NODE_LITERAL_NUMBER:
        case NODE_LITERAL_LETTER:
        case NODE_TYPE_PRIMITIVE:
            free(node->literal.value);
            break;
        case NODE_PRINT:
            for (size_t i = 0; i < node->print_stmt.arg_count; i++) {
                free_ast(node->print_stmt.args[i]);
            }
            free(node->print_stmt.args);
            break;

        case NODE_ARRAY_LITERAL:
            for (size_t i = 0; i < node->array_literal.count; i++) {
                free_ast(node->array_literal.elements[i]);
            }
            free(node->array_literal.elements);
            break;

        case NODE_BINARY_OP:
            free_ast(node->binary_op.left);
            free_ast(node->binary_op.right);
            break;

        case NODE_UNARY_OP:
            free_ast(node->unary_op.expr);
            break;

        case NODE_ARRAY_ACCESS:
            free_ast(node->array_access.array);
            free_ast(node->array_access.index);
            break;

        case NODE_FUNCTION_CALL:
            free_ast(node->function_call.func);
            for (size_t i = 0; i < node->function_call.arg_count; i++) {
                free_ast(node->function_call.args[i]);
            }
            free(node->function_call.args);
            break;

        case NODE_IF:
            free_ast(node->if_stmt.condition);
            for (size_t i = 0; i < node->if_stmt.then_count; i++) {
                free_ast(node->if_stmt.then_branch[i]);
            }
            free(node->if_stmt.then_branch);
            for (size_t i = 0; i < node->if_stmt.else_count; i++) {
                free_ast(node->if_stmt.else_branch[i]);
            }
            free(node->if_stmt.else_branch);
            for (size_t i = 0; i < node->if_stmt.elseif_count; i++) {
                free_ast(node->if_stmt.elseif_branch[i]);
            }
            free(node->if_stmt.elseif_branch);
            break;

        case NODE_SWITCH:
            free_ast(node->switch_stmt.expr);
            for (size_t i = 0; i < node->switch_stmt.case_count; i++) {
                free_ast(node->switch_stmt.cases[i]);
            }
            free(node->switch_stmt.cases);
            break;

        case NODE_CASE:
            free_ast(node->case_stmt.value);
            for (size_t i = 0; i < node->case_stmt.body_count; i++) {
                free_ast(node->case_stmt.body[i]);
            }
            free(node->case_stmt.body);
            break;

        case NODE_TYPE_ARRAY:
            free_ast(node->array_type.base_type);
            break;

        default:
            break;
    }

    free(node);
}
