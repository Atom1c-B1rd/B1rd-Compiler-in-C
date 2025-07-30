
#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include "lexer.h"

typedef enum {
    NODE_PROGRAM,
    NODE_FUNCTION,
    NODE_VARIABLE,
    NODE_LITERAL,
    NODE_LITERAL_LETTER,
    NODE_LITERAL_NUMBER,
    NODE_ARRAY_LITERAL,
    NODE_BINARY_OP,
    NODE_UNARY_OP,
    NODE_ARRAY_ACCESS,
    NODE_FUNCTION_CALL,
    NODE_IDENTIFIER,
    NODE_IF,
    NODE_SWITCH,
    NODE_CASE,
    NODE_RETURN,
    NODE_TYPE_PRIMITIVE,
    NODE_TYPE_ARRAY,
    NODE_PRINT
} NodeType;

typedef struct ASTNode {
    NodeType type;
    union {
        struct {
            struct ASTNode** functions;
            size_t function_count;
        } program;
        struct {
            char* name;
            struct ASTNode** params;
            size_t param_count;
            struct ASTNode* return_type;
            struct ASTNode** body;
            size_t body_count;
        } function;
        struct {
            char* name;
            struct ASTNode* var_type;
            struct ASTNode* value;
            bool is_mutable;
        } variable;
        struct {
            char* value;
        } literal;
        struct {
            TokenType op;
            struct ASTNode* left;
            struct ASTNode* right;
        } binary_op;

        struct {
            TokenType op;
            struct ASTNode* expr;
        } unary_op;
        struct {
            struct ASTNode* array;
            struct ASTNode* index;
        } array_access;

        struct {
            struct ASTNode** elements;
            size_t count;
        } array_literal;
        struct {
            struct ASTNode* func;
            struct ASTNode** args;
            size_t arg_count;
        } function_call;
        struct {
            struct ASTNode* base_type;
        }array_type;
        struct {
            char* type_name;
        }primitive_type;
        struct {
            struct ASTNode* condition;
            struct ASTNode** then_branch;
            size_t then_count;
            struct ASTNode** elseif_branch;
            size_t elseif_count;
            struct ASTNode** else_branch;
            size_t else_count;
        } if_stmt;
        struct {
            struct ASTNode* expr;
            struct ASTNode** cases;
            size_t case_count;
        } switch_stmt;
        struct {
            struct ASTNode* value;
            struct ASTNode** body;
            size_t body_count;
        } case_stmt;
        struct {
            struct ASTNode* value;
        }return_stmt;
        struct {
            struct ASTNode** args;
            size_t arg_count;
        }print_stmt;
    };
} ASTNode;

ASTNode* create_program();
ASTNode* create_function(char* name, ASTNode** params, size_t param_count,
                         ASTNode* return_type, ASTNode** body, size_t body_count);
ASTNode* create_variable(char* name, ASTNode* var_type, ASTNode* value, bool is_mutable);
ASTNode* create_literal(char* value, NodeType literal_type);
ASTNode* create_binary_op(TokenType op, ASTNode* left, ASTNode* right);
ASTNode* create_unary_op(TokenType op, ASTNode* expr);
ASTNode* create_array_access(ASTNode* array, ASTNode* index);
ASTNode* create_array_literal(ASTNode** elements, size_t count);
ASTNode* create_function_call(ASTNode* func, ASTNode** args, size_t arg_count);
ASTNode* create_if(ASTNode* condition, ASTNode** then_branch, size_t then_count,ASTNode* elseif_branch,
                    size_t elseif_count,ASTNode** else_branch, size_t else_count);
ASTNode* create_switch(ASTNode* expr, ASTNode** cases, size_t case_count);
ASTNode* create_case(ASTNode* value, ASTNode** body, size_t body_count);
ASTNode* create_primitive_type(char* type_name);
ASTNode* create_array_type(ASTNode* base_type);
ASTNode* create_print(ASTNode** args, size_t args_count);


void free_ast(ASTNode* node);

#endif