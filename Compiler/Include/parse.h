#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct {
    Token* tokens;
    size_t current_token;
    size_t token_count;
    int current_line;
} Parser;

ASTNode* parse(Token* tokens);

void parser_error(Parser* parser, const char* message);
static ASTNode* parse_type(Parser* parser);
static ASTNode* parse_primary(Parser* parser);
static ASTNode* parse_array_literal(Parser* parser);
static ASTNode* parse_expression(Parser* parser);
static ASTNode* parse_function_call(Parser* parser);
static ASTNode* parse_variable(Parser* parser);
static ASTNode* parse_if(Parser* parser);
static ASTNode* parse_statement(Parser* parser);
static ASTNode* parse_switch(Parser* parser);
static ASTNode* parse_function(Parser* parser);
static ASTNode* parse_print_statement(Parser* parser);
static ASTNode* parse_return_statement(Parser* parser);
static ASTNode* parse_input_statement(Parser* parser);
static ASTNode* parse_class(Parser* parser);
static ASTNode* parse_new_instance(Parser* parser);
static ASTNode* parse_method_call(Parser* parser,ASTNode* object);
static ASTNode* parse_property_accsess(Parser* parser,char* property_name);

#endif