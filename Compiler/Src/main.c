#include "lexer.h"
#include "ast.h"
#include "parse.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    const char* source = "factorial<-(n:Number)->Number{\nif(n==0){\nPrint<-(n);\nreturn<-(1);\n}else{\nPrint<-(n);\nreturn<-(n * factorial<-(n-1));\n }\n}";
    Token* tokens = tokenize(source);
    for (size_t i = 0; tokens[i].type != TOKEN_EOF; i++) {
        printf("Token: %s\tType: %d\tLine: %d\n",
               tokens[i].value, tokens[i].type, tokens[i].line);
    }
    ASTNode* ast = parse(tokens);
    free_tokens(tokens);
    free_ast(ast);
    return 0;
}
