
#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

ASTNode* create_program() {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_PROGRAM;
    node->program.functions = NULL;
    node->program.function_count = 0;
    node->program.classes = NULL;
    node->program.class_count = 0;
    node->program.data = NULL;
    node->program.data_count = 0;
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

ASTNode* create_if(ASTNode* condition, ASTNode** then_branch, size_t then_count,ASTNode* elseif_condition,ASTNode** elseif_branch,
                    size_t elseif_count,ASTNode** else_branch, size_t else_count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_IF;
    node->if_stmt.condition = condition;
    node->if_stmt.then_branch = then_branch;
    node->if_stmt.then_count = then_count;
    node->if_stmt.elseif_condition = elseif_condition;
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
ASTNode* create_input(ASTNode* input_type,ASTNode* prompt) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_INPUT;
    node->input_stmt.input_type = input_type;
    node->input_stmt.prompt = prompt;
    //printf("se hizo un input");
    return node;
}
ASTNode* create_class_def(char* name, ASTNode** properties, size_t properties_count,ASTNode** methods,size_t method_count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_CLASS;
    node->class_def.name = strdup(name);
    node->class_def.properties = properties;
    node->class_def.properties_count = properties_count;
    node->class_def.methods = methods;
    node->class_def.method_count = method_count;
    return node;
}
ASTNode* create_class_instance(ASTNode* class_ref, ASTNode** properties, size_t properties_count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_CLASS_INSTANCE;
    node->class_instance.class_ref = class_ref;
    node->class_instance.properties = properties;
    node->class_instance.properties_count = properties_count;
    return node;
}
ASTNode* create_property_access(ASTNode* object,struct ASTNode* property_name,bool is_this,bool assingment) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_PROPERTY_ACCESS;
    node->property_access.object = object;
    node->property_access.property_name = property_name;
    node->property_access.is_this = is_this;
    node->property_access.assingment = assingment;
    printf("se creo u property accses\n");
    return node;
}
ASTNode* create_method_call(ASTNode* object, char* method_name,ASTNode** args,size_t args_count) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_METHOD_CALL;
    node->method_call.object = object;
    node->method_call.method_name = method_name;
    node->method_call.args = args;
    node->method_call.arg_count = args_count;
    return node;
}
ASTNode* create_assigment_stmt(ASTNode* target, ASTNode* value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type=NODE_ASSIGN;
    node->assignment.target = target;
    node->assignment.value = value;
    return node;
}

ASTNode* create_this_node() {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = NODE_THIS;
    return node;
}

void free_ast(ASTNode* node) {
    if (!node) return;
    switch (node->type) {
        case NODE_PROGRAM:
            for (size_t i = 0; i < node->program.function_count; i++) {
                free_ast(node->program.functions[i]);
            }
            if (node->program.functions) {
                free(node->program.functions);
            }

            for (size_t i=0; i < node->program.class_count; i++) {
                free_ast(node->program.classes[i]);
            }
            if (node->program.classes) {
                free(node->program.classes);
            }

            for (size_t i=0; i < node->program.data_count; i++) {
                free_ast(node->program.data[i]);
            }
            if (node->program.data) {
                free(node->program.data);
            }
            break;

        case NODE_FUNCTION:
            if (node->function.name) {
                free(node->function.name);
            }
            for (size_t i = 0; i < node->function.param_count; i++) {
                free_ast(node->function.params[i]);
            }
            if (node->function.params) {
                free(node->function.params);
            }
            if (node->function.return_type) {
                free_ast(node->function.return_type);
            }
            for (size_t i = 0; i < node->function.body_count; i++) {
                free_ast(node->function.body[i]);
            }
            if (node->function.body) {
                free(node->function.body);
            }
            break;

        case NODE_VARIABLE:
            if (node->variable.name) {
                free(node->variable.name);
            }
            if (node->variable.var_type) {
                free_ast(node->variable.var_type);
            }
            if (node->variable.value) {
                free_ast(node->variable.value);
            }
            break;

        case NODE_LITERAL:
        case NODE_LITERAL_NUMBER:
        case NODE_LITERAL_LETTER:
        case NODE_TYPE_PRIMITIVE:
            if (node->literal.value) {
                free(node->literal.value);
            }
            break;
        case NODE_PRINT:
            for (size_t i = 0; i < node->print_stmt.arg_count; i++) {
                free_ast(node->print_stmt.args[i]);
            }
            if (node->print_stmt.args) {
                free(node->print_stmt.args);
            }
            break;

        case NODE_ARRAY_LITERAL:
            for (size_t i = 0; i < node->array_literal.count; i++) {
                free_ast(node->array_literal.elements[i]);
            }
            if (node->array_literal.elements) {
                free(node->array_literal.elements);
            }
            break;

        case NODE_BINARY_OP:
            if (node->binary_op.left) {
                free_ast(node->binary_op.left);
            }
            if (node->binary_op.right) {
                free_ast(node->binary_op.right);
            }
            break;

        case NODE_UNARY_OP:
            if (node->unary_op.expr) {
                free_ast(node->unary_op.expr);
            }
            break;

        case NODE_ARRAY_ACCESS:
            if (node->array_access.array) {
                free_ast(node->array_access.array);
            }
            if (node->array_access.index) {
                free_ast(node->array_access.index);
            }
            break;

        case NODE_FUNCTION_CALL:
            if (node->function_call.func) {
                free_ast(node->function_call.func);
            }
            for (size_t i = 0; i < node->function_call.arg_count; i++) {
                free_ast(node->function_call.args[i]);
            }
            if (node->function_call.args) {
                free(node->function_call.args);
            }
            break;

        case NODE_IF:
            if (node->if_stmt.condition) {
                free_ast(node->if_stmt.condition);
            }
            for (size_t i = 0; i < node->if_stmt.then_count; i++) {
                free_ast(node->if_stmt.then_branch[i]);
            }
            if (node->if_stmt.then_branch) {
                free(node->if_stmt.then_branch);
            }
            for (size_t i = 0; i < node->if_stmt.else_count; i++) {
                free_ast(node->if_stmt.else_branch[i]);
            }
            if (node->if_stmt.else_branch) {
                free(node->if_stmt.else_branch);
            }
            for (size_t i = 0; i < node->if_stmt.elseif_count; i++) {
                free_ast(node->if_stmt.elseif_branch[i]);
            }
            if (node->if_stmt.elseif_branch) {
                free(node->if_stmt.elseif_branch);
            }
            break;

        case NODE_SWITCH:
            if (node->switch_stmt.expr) {
                free_ast(node->switch_stmt.expr);
            }
            for (size_t i = 0; i < node->switch_stmt.case_count; i++) {
                free_ast(node->switch_stmt.cases[i]);
            }
            if (node->switch_stmt.cases) {
                free(node->switch_stmt.cases);
            }
            break;

        case NODE_CASE:
            if (node->case_stmt.value) {
                free_ast(node->case_stmt.value);
            }
            for (size_t i = 0; i < node->case_stmt.body_count; i++) {
                free_ast(node->case_stmt.body[i]);
            }
            if (node->case_stmt.body) {
                free(node->case_stmt.body);
            }
            break;

        case NODE_TYPE_ARRAY:
            if (node->array_type.base_type) {
                free_ast(node->array_type.base_type);
            }
            break;
        case NODE_INPUT:
            if(node->input_stmt.input_type) {
                free_ast(node->input_stmt.input_type);
            }
            if (node->input_stmt.prompt) {
                free_ast(node->input_stmt.prompt);
            }
            break;
        case NODE_CLASS:
            if (node->class_def.name) {
                free(node->class_def.name);
                node->class_def.name = NULL;
            }

            if (node->class_def.properties) {
                for (size_t i = 0; i < node->class_def.properties_count; i++) {
                    free_ast(node->class_def.properties[i]);
                }
                free(node->class_def.properties);
            }
            if (node->class_def.methods) {
                for (size_t i = 0; i < node->class_def.method_count; i++) {
                    free_ast(node->class_def.methods[i]);
                }
                free(node->class_def.methods);
            }
            break;
        case NODE_CLASS_INSTANCE:
            if (node->class_instance.class_ref) {
                free_ast(node->class_instance.class_ref);
            }
            for (size_t i = 0; i < node->class_instance.properties_count; i++) {
                free_ast(node->class_instance.properties[i]);
            }
            if (node->class_instance.properties) {
                free(node->class_instance.properties);
            }
            break;
        case NODE_PROPERTY_ACCESS:
            if (node->property_access.property_name) {
                free_ast(node->property_access.property_name);
            }
            if (node->property_access.object) {
                free_ast(node->property_access.object);
            }
            node->property_access.is_this = NULL;
            node->property_access.assingment = NULL;
            break;
        case NODE_METHOD_CALL:
            free_ast(node->method_call.object);
            free(node->method_call.method_name);
            for (size_t i = 0; i < node->method_call.arg_count; i++) {
                free_ast(node->method_call.args[i]);
            }
            free(node->method_call.args);
            break;
        case NODE_THIS:
            free(node->this_.target);
            break;
        default:
            break;
    }
    free(node);
}