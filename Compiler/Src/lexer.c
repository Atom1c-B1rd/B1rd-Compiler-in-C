#include "lexer.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static Token* tokens = NULL;
static size_t tokens_capacity = 0;
static size_t tokens_count = 0;
static const char* current = NULL;
static int current_line = 1;

static void add_token(TokenType type, const char* start, size_t length) {
    if (tokens_count >= tokens_capacity) {
        tokens_capacity = tokens_capacity == 0 ? 16 : tokens_capacity * 2;
        tokens = realloc(tokens, sizeof(Token) * tokens_capacity);
    }
    tokens[tokens_count].type = type;
    tokens[tokens_count].value = strndup(start, length);
    tokens[tokens_count].line = current_line;
    tokens_count++;
}

static bool is_operator_char(char c) {
    return strchr("=!<>+-*/:.(),{}[];,", c) != NULL;
}


Token* tokenize(const char* source) {
    current = source;
    tokens_count = 0;
    current_line = 1;

    while (*current != '\0') {

        if (isspace(*current)) {
            if (*current == '\n') current_line++;
            current++;
            continue;
        }


        if (*current == '/' && *(current + 1) == '/') {
            while (*current != '\0' && *current != '\n') current++;
            continue;
        }


        if (*current == '"') {
            const char* start = current++;
            while (*current != '\0' && *current != '"') {
                if (*current == '\n') current_line++;
                current++;
            }
            if (*current == '\0') {
                add_token(TOKEN_ERROR, start, current - start);
                break;
            }
            add_token(TOKEN_LITERAL_STRING, start + 1, current - start - 1);
            current++;
            continue;
        }

        if (isdigit(*current)) {
            const char* start = current;
            while (isdigit(*current)) current++;
            add_token(TOKEN_LITERAL_NUMBER, start, current - start);
            continue;
        }
        if (*current == '.'&& !isdigit(*current)) {
            add_token(TOKEN_DOT,current, 1);
            current++;
            continue;
        }

        if (isalpha(*current) || *current == '_') {
            const char* start = current;
            while (isalnum(*current) || *current == '_') current++;
            size_t length = current - start;
            char* lexeme = strndup(start, length);

            bool is_keyword = false;
            for (size_t i = 0; OPERATORS_AND_KEYWORDS[i] != NULL; i++) {
                if (strcmp(lexeme, OPERATORS_AND_KEYWORDS[i]) == 0) {
                    add_token(TOKEN_TYPES[i], start, length);
                    is_keyword = true;
                    break;
                }
            }
            if (!is_keyword) add_token(TOKEN_IDENTIFIER, start, length);

            free(lexeme);
            continue;
        }

        bool found_operator = false;
        for (size_t i = 0; OPERATORS_AND_KEYWORDS[i] != NULL; i++) {
            size_t op_len = strlen(OPERATORS_AND_KEYWORDS[i]);
            if (strncmp(current, OPERATORS_AND_KEYWORDS[i], op_len) == 0) {
                add_token(TOKEN_TYPES[i], current, op_len);
                current += op_len;
                found_operator = true;
                break;
            }
        }
        if (found_operator) continue;
        add_token(TOKEN_ERROR, current, 1);
        current++;
    }

    add_token(TOKEN_EOF, current, 0);
    return tokens;
}

void free_tokens(Token* tokens) {
    for (size_t i = 0; i < tokens_count; i++) {
        free(tokens[i].value);
    }
    free(tokens);
}