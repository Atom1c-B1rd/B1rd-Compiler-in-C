//
// Created by Elbar on 31/7/2025.
//

#ifndef CODEGEN_H
#define CODEGEN_H
#include "ast.h"
#include <stdio.h>

void generate_codegen(ASTNode* ast, const char* output_filename);

void generate_program(FILE* output, ASTNode* program);
void generate_class(FILE* output, ASTNode* class_def);
void generate_function(FILE* output, ASTNode* function);
void generate_expression(FILE* output, ASTNode* expr);
void generate_statement(FILE* output, ASTNode* stmt);
void generate_type(FILE* output, ASTNode* type_node);


#endif //CODEGENERATOR_H
