//
// Created by Elbar on 31/7/2025.
//

#ifndef CODEGEN_H
#define CODEGEN_H
#include "ast.h"
#include <stdio.h>

void generate_expression(ASTNode* node, FILE* output);
void generate_statement(ASTNode* node, FILE* output);
void generate_function(ASTNode* node, FILE* output);
void generate_codegen(ASTNode* node, char* output_filename);

#endif //CODEGENERATOR_H
