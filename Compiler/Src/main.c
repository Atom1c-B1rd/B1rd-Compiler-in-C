#include <stdio.h>
#include "lexer.h"

int main(void) {
    const char* source="main<-()->Void{ Print(\"Hello World\");}";
    Token* tokens = tokenize(source);
    for (size_t i=0; tokens[i].type!=Token_EOF; i++) {
        printf("tokens[%s].type=%d\n", tokens[i].value, tokens[i].type);
    }
    free_token(tokens);
    return 0;
}
