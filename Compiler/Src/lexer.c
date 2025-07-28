#include  "lexer.h"
#include  <ctype.h>
#include  <string.h>
#include  <stdlib.h>


static Token* tokens = NULL;
static size_t num_tokens = 0;
static const char* current_token=NULL;

static void add_token(TokenType type,const char* start, size_t length) {
    tokens=realloc(tokens,sizeof(Token)*(num_tokens+1));
    tokens[num_tokens].type=type;
    tokens[num_tokens].value=strndup(start,length);
    if(type==Token_Identifier) {current_token--;}
    num_tokens++;
}


Token* tokenize(const char* source) {
    current_token = source;
    num_tokens = 0;
    while (*current_token != '\0') {
        if (isspace(*current_token)) {
            current_token++;
            continue;
        }
        if (strncmp(current_token, "<-", 2) == 0) {
            add_token(Token_LArrow,current_token,2);
            current_token += 2;
            continue;
        }
        if (strncmp(current_token, "->", 2) == 0) {
            add_token(Token_RArrow,current_token,2);
            current_token += 2;
            continue;
        }
        if (strncmp(current_token, "==", 2) == 0) {
            add_token(Token_EQ,current_token,2);
            current_token += 2;
            continue;
        }
        if (strncmp(current_token, "!=", 2) == 0) {
            add_token(Token_Not_EQ,current_token,2);
            current_token += 2;
            continue;
        }
        switch (*current_token) {
            case '[': add_token(Token_LBrace,current_token,1); break;
            case ']': add_token(Token_RBrace,current_token,1); break;
            case '(': add_token(Token_LParen,current_token,1); break;
            case ')': add_token(Token_RParen,current_token,1); break;
            case '.': add_token(Token_Dot,current_token,1); break;
            case ',': add_token(Token_Comma,current_token,1); break;
            case ':': add_token(Token_Double_Dot,current_token,1); break;
            case '{': add_token(Token_LBracket,current_token,1); break;
            case '}': add_token(Token_RBracket,current_token,1); break;
            case '+': add_token(Token_Plus,current_token,1); break;
            case '-': add_token(Token_Minus,current_token,1); break;
            case '*': add_token(Token_Mult,current_token,1); break;
            case '/': add_token(Token_LBrace,current_token,1); break;
            case ';': add_token(Token_SemiColon,current_token,1); break;
            default:
                if (isalpha(*current_token)) {
                    const char* start = current_token;
                    while (isalnum(*current_token)){current_token++;}
                    size_t length =current_token-start;
                    char* word=strndup(start,length);

                    if (strcmp(word,"Number")==0) {add_token(Token_Number,start,length);}
                    else if (strcmp(word,"Letter")==0) {add_token(Token_Letter,start,length);}
                    else if (strcmp(word,"Void")==0) {add_token(Token_Void,start,length);}
                    else if (strcmp(word,"var")==0) {add_token(Token_Var,start,length);}
                    else if (strcmp(word,"if")==0) {add_token(Token_If,start,length);}
                    else if (strcmp(word,"else")==0) {add_token(Token_Else,start,length);}
                    else if (strcmp(word,"return")==0) {add_token(Token_Return,start,length);}
                    else if (strcmp(word,"switch")==0) {add_token(Token_Switch,start,length);}
                    else if (strcmp(word,"case")==0) {add_token(Token_Case,start,length);}
                    else if (strcmp(word,"break")==0) {add_token(Token_Break,start,length);}
                    else if (strcmp(word,"Array")==0) {add_token(Token_Array,start,length);}
                    else {
                        add_token(Token_Identifier,start,length);
                        }
                    free(word);
                    }
                else if (*current_token == '"') {
                    current_token++;
                    const char* start = current_token;
                    while (*current_token != '"') {current_token++;}
                    add_token(Token_Literal_Letter,start,current_token-start);
                    }
                else if (isdigit(*current_token)) {
                    const char* start = current_token;
                    while (isdigit(*current_token)){current_token++;}
                    add_token(Token_Literal_Number,start,start-current_token);
                    }
                break;
        }
        current_token++;
    }
    add_token(Token_EOF,current_token,0);
    return tokens;
}

void free_token(Token* tokens) {
    for (size_t i=0; i<num_tokens; i++) {
        free(tokens[i].value);
    }
    free(tokens);
}
