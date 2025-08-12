#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    //op
    TOKEN_EQ,
    TOKEN_NEQ,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LEQ,
    TOKEN_GEQ,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_ASSIGN,
    TOKEN_COLON,

    // kw
    TOKEN_PRINT,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_SWITCH,
    TOKEN_CASE,
    TOKEN_RETURN,
    TOKEN_VAR,
    TOKEN_NUMBER,
    TOKEN_LETTER,
    TOKEN_BOOL,
    TOKEN_VOID,
    TOKEN_ARRAY,
    TOKEN_INPUT,
    TOKEN_CLASS,
    TOKEN_NEW,
    TOKEN_THIS,

    // Sím
    TOKEN_LARROW,
    TOKEN_RARROW,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_SEMICOLON,
    TOKEN_DOWNLINE,

    // Lit
    TOKEN_IDENTIFIER,
    TOKEN_LITERAL_NUMBER,
    TOKEN_LITERAL_STRING,

    // Core
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    char* value;
    int line;
} Token;

static const char* const OPERATORS_AND_KEYWORDS[] = {
    // Op Comp
    "==", "!=", "<=", ">=", "<-", "->",
    // Op sim
    "<", ">", "+", "-", "*", "/", "=", ":", "(", ")", "{", "}", "[", "]", ",", ".", ";","_",
    // Kw
    "if", "else", "return", "var", "Number", "Letter", "Void", "Array","switch","case","Print","Input",
    "class","new","this",NULL
};

static const TokenType TOKEN_TYPES[] = {
    TOKEN_EQ, TOKEN_NEQ, TOKEN_LEQ, TOKEN_GEQ, TOKEN_LARROW, TOKEN_RARROW,
    TOKEN_LT, TOKEN_GT, TOKEN_PLUS, TOKEN_MINUS, TOKEN_MUL, TOKEN_DIV,
    TOKEN_ASSIGN, TOKEN_COLON, TOKEN_LPAREN, TOKEN_RPAREN, TOKEN_LBRACE,
    TOKEN_RBRACE, TOKEN_LBRACKET, TOKEN_RBRACKET, TOKEN_COMMA, TOKEN_DOT, TOKEN_SEMICOLON,TOKEN_DOWNLINE,
    TOKEN_IF, TOKEN_ELSE, TOKEN_RETURN, TOKEN_VAR, TOKEN_NUMBER, TOKEN_LETTER,
    TOKEN_VOID, TOKEN_ARRAY,TOKEN_SWITCH,TOKEN_CASE,TOKEN_PRINT,TOKEN_INPUT,TOKEN_CLASS,TOKEN_NEW,TOKEN_THIS,
};

Token* tokenize(const char* source);
void free_tokens(Token* tokens);

#endif