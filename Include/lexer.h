#ifndef LEXER_H
#define LEXER_H

typedef  enum {
    // Palabras reservadas
    Token_If, Token_Else, Token_Return, Token_Var,
    Token_Number, Token_Void, Token_Letter, Token_Array,
    Token_Switch, Token_Break, Token_Case,
    // Simbolos
    Token_LArrow, Token_RArrow, Token_EQ, Token_Plus, Token_Minus,
    Token_LParen, Token_RParen, Token_LBracket, Token_RBracket,
    Token_LBrace, Token_RBrace,Token_Comma, Token_Dot,Token_Not_EQ,
    Token_Double_Dot,Token_Mult,Token_Div,Token_SemiColon,
    //identificadores
    Token_Identifier, Token_Literal_Number, Token_Literal_Letter,
    Token_EOF, Token_Error

}TokenType;

typedef struct {
    TokenType type;
    char* value;
}Token;

Token* tokenize(const char* source);
void free_token(Token* token);
#endif //LEXER_H
