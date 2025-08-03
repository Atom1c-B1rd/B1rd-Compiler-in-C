#include "lexer.h"
#include "ast.h"
#include "parse.h"
#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    //const char* source = "main<-()->Void{\n\t Print<-(\"hola mundo\");\n}";
    FILE *input=fopen(argv[1],"rb");
    if (!input) {
        perror("Error opening file");
        return 1;
    }
    fseek(input, 0, SEEK_END);
    long size = ftell(input);
    fseek(input, 0, SEEK_SET);
    char* source=malloc(size+1);
    if (!source) {
        fclose(input);
        return 1;
    }
    fread(source, 1, size, input);
    source[size]='\0';
    fclose(input);


    Token* tokens = tokenize(source);
    for (size_t i = 0; tokens[i].type != TOKEN_EOF; i++) {
        printf("Token: %s\tType: %d\tLine: %d\n",
               tokens[i].value, tokens[i].type, tokens[i].line);
    }
    ASTNode* ast = parse(tokens);
    generate_codegen(ast,"output.c");
    free_tokens(tokens);
    free_ast(ast);
    return 0;
}
