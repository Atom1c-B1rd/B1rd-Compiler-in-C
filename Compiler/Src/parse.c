#include "parse.h"
#include "ast.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static Token peek(Parser* parser) {
    if (parser->current_token >=parser->token_count) {
        printf("[ERROR] peek fuera de rango: %p >=%p\n",&parser->current_token,&parser->token_count);
        exit(1);
    }
    return parser->tokens[parser->current_token];
}

static Token consume(Parser* parser) {
    return parser->tokens[parser->current_token++];
}

static bool match(Parser* parser, TokenType type) {
    return peek(parser).type == type;
}

static void expect(Parser* parser, TokenType type, const char* error_msg) {
    Token t=peek(parser);
    if (!match(parser, type)) {
        fprintf(stderr,"[EXPECT] esperaba un %d pero encontre esto %d ('%s') en la linea %d\n",type,t.type,t.value,t.line);
        fprintf(stderr, "Error (línea %d): %s\n", peek(parser).line, error_msg);
        exit(1);
    }
    consume(parser);
}
static bool is_class_type(Parser* parser, char* type_name) {
    for (size_t i=0; i<parser->classes_count; i++) {
        if (!strcmp(type_name, parser->classes[i])==0) {
            return true;
        }
    }
    return false;
}
static ASTNode* parse_type(Parser* parser) {
    Token type_token = consume(parser);

    if (type_token.type==TOKEN_VOID) {
        return create_primitive_type(type_token.value);
    }
     if (type_token.type==TOKEN_LETTER) {
        return  create_primitive_type(type_token.value);
    }
     if (type_token.type==TOKEN_NUMBER) {
        return create_primitive_type(type_token.value);
    }
    if(type_token.type == TOKEN_IDENTIFIER) {
        ASTNode* type_node = create_primitive_type(type_token.value);
        if (match(parser, TOKEN_LBRACKET)) {
            consume(parser); // '['
            expect(parser, TOKEN_RBRACKET, "Se esperaba ']'");
            return create_array_type(type_node);
        }
        return type_node;
    }
    fprintf(stderr, "Error: Tipo inválido\n");
    exit(1);
}
static ASTNode* parse_primary(Parser* parser) {
    Token token = peek(parser);
    switch (token.type) {
        case TOKEN_INPUT: {
            consume(parser);
            expect(parser,TOKEN_LARROW,"Se esperaba '<-' despues del input");
            expect(parser,TOKEN_LPAREN,"Se esperaba '(' despues del input");
            ASTNode* input_type = parse_type(parser);
            expect(parser,TOKEN_COMMA,"Se esperaba ',' despues del tipo");
            ASTNode* prompt=NULL;
            if (!match(parser,TOKEN_RPAREN)) {
                prompt=parse_expression(parser);
            }
            expect(parser,TOKEN_RPAREN,"Se esperaba ')' despues del prompt");
            return create_input(input_type,prompt);
        }

        case TOKEN_LITERAL_NUMBER:
        case TOKEN_LITERAL_STRING:
            consume(parser);
            return create_literal(token.value,token.type == TOKEN_LITERAL_NUMBER ? NODE_LITERAL : NODE_LITERAL_LETTER);
        case TOKEN_IDENTIFIER: {
            printf("entre al identifier\n");
            if ((parser->current_token + 1 < parser->token_count) && parser->tokens[parser->current_token + 1].type == TOKEN_LARROW ){
                return parse_function_call(parser);
            }

            consume(parser);
            ASTNode* ident = malloc(sizeof(ASTNode));
            ident->type = NODE_IDENTIFIER;
            ident->literal.value = strdup(token.value);
            return ident;
        }

        case TOKEN_LBRACKET:
            return parse_array_literal(parser);

        case TOKEN_LPAREN: {
            consume(parser);
            ASTNode* expr = parse_expression(parser);
            expect(parser, TOKEN_RPAREN, "Se esperaba ')'");
            return expr;
        }
        case TOKEN_THIS:
            create_this_node();
            break;
        case TOKEN_NEW:
            printf("entre al new primary\n");
            parse_new_instance(parser);
            break;
        default:
            fprintf(stderr, "Error: Expresión inválida en la linea %d\n",token.line);
            exit(1);
    }
}
static ASTNode* parse_this(Parser* parser) {
    expect(parser,TOKEN_THIS,"Se esperaba This");
    consume(parser);
    Token varible_name = consume(parser);
    ASTNode* class_ref=create_literal(varible_name.value,NODE_IDENTIFIER);
    return create_this_node(class_ref);

}
static ASTNode* parse_array_literal(Parser* parser) {
    expect(parser, TOKEN_LBRACKET, "Se esperaba '['");
    ASTNode** elements = NULL;
    size_t count = 0;

    while (!match(parser, TOKEN_RBRACKET)) {
        ASTNode* expr = parse_expression(parser);
        elements = realloc(elements, sizeof(ASTNode*) * (count + 1));
        elements[count++] = expr;

        if (!match(parser, TOKEN_COMMA)) break;
        consume(parser);
    }

    expect(parser, TOKEN_RBRACKET, "Se esperaba ']'");
    return create_array_literal(elements, count);
}

static ASTNode* parse_function_call(Parser* parser) {
    Token func_name = consume(parser);
    expect(parser,TOKEN_LARROW, "Se esperaba '<-'");
    expect(parser, TOKEN_LPAREN, "Se esperaba '('");

    ASTNode** args = NULL;
    size_t arg_count = 0;

    while (!match(parser, TOKEN_RPAREN)) {
        ASTNode* arg = parse_expression(parser);
        args = realloc(args, sizeof(ASTNode*) * (arg_count + 1));
        args[arg_count++] = arg;

        if (!match(parser, TOKEN_COMMA)) break;
        consume(parser);
    }

    expect(parser, TOKEN_RPAREN, "Se esperaba ')'");
    ASTNode* func = create_literal(func_name.value, NODE_IDENTIFIER);
    return create_function_call(func, args, arg_count);
}

static ASTNode* parse_expression(Parser* parser) {
    ASTNode* left = parse_primary(parser);

    if (left->type == NODE_THIS) {
        printf("entre al this de expression\n");
        consume(parser);
        expect(parser, TOKEN_RARROW, "Se esperaba '.' despues de 'this'");
        Token prop=consume(parser);
        if (prop.type != TOKEN_IDENTIFIER) {
            fprintf(stderr,"Se esperaba un identificar de propiedad despues de 'this'");
        }
        bool assigment=false;
        if (match(parser, TOKEN_ASSIGN)) {
            printf("esta asingnando\n");
            assigment = true;
        }
        left= create_property_access(left,create_literal(prop.value,NODE_IDENTIFIER),true,assigment);
        if (assigment) {
            consume(parser);
            consume(parser);
        }

    }

    while (match(parser, TOKEN_PLUS) || match(parser, TOKEN_MINUS) ||
           match(parser, TOKEN_MUL) || match(parser, TOKEN_DIV) ||
           match(parser, TOKEN_EQ) || match(parser, TOKEN_NEQ) ||
           match(parser, TOKEN_LT) || match(parser, TOKEN_GT)) {
        Token op = peek(parser);
        consume(parser);
        ASTNode* right = parse_primary(parser);
        left = create_binary_op(op.type, left, right);

    }
    if (match(parser,TOKEN_DOT)) {
        consume(parser);
        if (match(parser, TOKEN_IDENTIFIER)&&
            (parser->current_token + 1 < parser->token_count)&&
            parser->tokens[parser->current_token + 1].type == TOKEN_LARROW) {
                left=parse_method_call(parser, left);
            }else {
                left=create_property_access(left,create_literal(consume(parser).value,NODE_IDENTIFIER),false,false);
                if (match(parser, TOKEN_ASSIGN)) {
                    consume(parser);
                    ASTNode* value = parse_expression(parser);
                    create_assigment_stmt(left, value);
                }
            }

    }
    if (match(parser, TOKEN_LBRACKET)) {
        consume(parser);
        ASTNode* index = parse_expression(parser);
        expect(parser, TOKEN_RBRACKET, "Se esperaba ']'");
        left = create_array_access(left, index);
    }

    return left;
}
static ASTNode* parse_variable(Parser* parser) {
    expect(parser,TOKEN_VAR,"Se esperaba 'var'");
    Token name = consume(parser);
    expect(parser, TOKEN_COLON , "Se esperaba ':'");

    ASTNode* var_type = parse_type(parser);
    if (var_type->type == NODE_IDENTIFIER && is_class_type(parser,var_type->primitive_type.type_name)) {
        var_type->type=NODE_CLASS;
    }

    ASTNode* value=NULL;
    bool is_mutable = false;
    if (match(parser, TOKEN_ASSIGN)) {
        consume(parser);
        is_mutable = true;
        value = parse_expression(parser);
    }else if (match(parser, TOKEN_LARROW)){
        consume(parser);
        value = parse_expression(parser);
    }
    expect(parser,TOKEN_SEMICOLON,"se esperaba ';");
    return create_variable(name.value, var_type,value,is_mutable);
}
static ASTNode* parse_statement(Parser* parser) {
    Token token=peek(parser);
    switch (token.type) {
        case TOKEN_IF:
            return parse_if(parser);
        case TOKEN_SWITCH:
            return parse_switch(parser);
        case TOKEN_RETURN: {
            return parse_return_statement(parser);
        }
        case TOKEN_VAR:
            return parse_variable(parser);
        case TOKEN_PRINT:
            return parse_print_statement(parser);
        case TOKEN_INPUT:
            return parse_input_statement(parser);
        case TOKEN_CLASS:
            parse_class(parser);
        case TOKEN_NEW:
            parse_new_instance(parser);
        default: {
            ASTNode* expr = parse_expression(parser);
            if (expr->type != TOKEN_ASSIGN) {
                expect(parser, TOKEN_SEMICOLON,"Se esperaba ';'");
            }

            return expr;
        }
    }
}
static ASTNode* parse_class(Parser* parser) {
    expect(parser,TOKEN_CLASS,"Se esperaba 'class'");
    Token name = consume(parser);
    if (name.type!=TOKEN_IDENTIFIER) {
        fprintf(stderr,"Se esperaba un identificador para el nombre de la clase'");
    }
    parser->classes = realloc(parser->classes, sizeof(char*)*(parser->classes_count+1));

    parser->classes[parser->classes_count] = strdup(name.value);
    parser->classes_count++;

    expect(parser,TOKEN_LBRACE,"Se esperaba '{'");

    ASTNode** properties=NULL;
    size_t properties_count = 0;
    ASTNode** methods=NULL;
    size_t methods_count = 0;

    while (!match(parser, TOKEN_RBRACE)) {
        if (match(parser,TOKEN_VAR)) {
            ASTNode* prop = parse_variable(parser);
            properties=realloc(properties,(properties_count+1)*sizeof(ASTNode*));
            properties[properties_count++]=prop;
        }else if (match(parser, TOKEN_IDENTIFIER)) {
            ASTNode* meth=parse_function(parser);
            methods=realloc(methods,(methods_count+1)*sizeof(ASTNode*));
            methods[methods_count++]=meth;
        }else {
            fprintf(stderr,"Error; Declaracion Invalido en clase\n");
            exit(1);
        }
    }
    expect(parser,TOKEN_RBRACE,"Se esperaba '}' al final de la clase");
    return create_class_def(name.value,properties,properties_count,methods,methods_count);
}
static ASTNode* parse_new_instance(Parser* parser) {
    expect(parser,TOKEN_NEW,"Se esperaba new");
    Token class_name = consume(parser);
    ASTNode* class_ref=create_literal(class_name.value,NODE_IDENTIFIER);
    return create_class_instance(class_ref,NULL,0);
}
static ASTNode* parse_method_call(Parser* parser, ASTNode* object) {
    Token method_name=consume(parser);
    expect(parser,TOKEN_LARROW,"Se esperaba '<-'");
    expect(parser,TOKEN_LPAREN,"Se esperaba '('");
    ASTNode** args=NULL;
    size_t args_count = 0;
    while (!match(parser, TOKEN_RPAREN)) {
        ASTNode* arg=parse_expression(parser);
        args=realloc(args,(args_count+1)*sizeof(ASTNode*));
        args[args_count++]=arg;
    }
    expect(parser,TOKEN_RPAREN,"Se esperaba ')'");
    return create_method_call(object,method_name.value,args,args_count);
}
static ASTNode* parse_return_statement(Parser* parser) {
    consume(parser);
    expect(parser, TOKEN_LARROW, "Se esperaba '<-' despues de return");
    ASTNode* expr = parse_expression(parser);
    expect(parser,TOKEN_SEMICOLON,"Se esperaba ';' despues de return");
    ASTNode* ret = malloc(sizeof(ASTNode));
    ret->type = NODE_RETURN;
    ret->return_stmt.value = expr;
    return ret;
}
static ASTNode* parse_print_statement(Parser* parser) {
    consume(parser);
    expect(parser, TOKEN_LARROW, "Se esperaba '<-' despues de Print");
    expect(parser, TOKEN_LPAREN, "Se esperaba '(' despues de Print");
    ASTNode** args = NULL;
    size_t arg_count = 0;

    while (!match(parser, TOKEN_RPAREN)) {
        ASTNode* arg = parse_expression(parser);
        args=realloc(args, sizeof(ASTNode*) * (arg_count + 1));
        args[arg_count++] = arg;
        if (!match(parser, TOKEN_COMMA)) {break;}
        consume(parser);
    }
    expect(parser, TOKEN_RPAREN, "Se esperaba ')' despues de los argumentos");
    expect(parser, TOKEN_SEMICOLON,"Se esperaba ';' despues de Print");
    return create_print(args,arg_count);
}

static ASTNode* parse_if(Parser* parser) {
    expect(parser, TOKEN_IF, "Se esperaba 'if'");
    expect(parser, TOKEN_LPAREN, "Se esperaba '('");
    ASTNode* condition = parse_expression(parser);
    expect(parser, TOKEN_RPAREN, "Se esperaba ')'");
    expect(parser, TOKEN_LBRACE,"se esperaba '{");
    ASTNode* condition_elseif=NULL;
    ASTNode** then_brach=NULL;
    size_t then_count = 0;
    while (!match(parser, TOKEN_RBRACE)) {
        ASTNode* stmt = parse_statement(parser);
        then_brach =realloc(then_brach, sizeof(ASTNode*) * (then_count + 1));
        then_brach[then_count++]=stmt;
    }
    expect(parser, TOKEN_RBRACE,"se esperaba '}'");
    ASTNode** else_branch=NULL;
    size_t else_count = 0;
    ASTNode** elseif_branch=NULL;
    size_t elseif_count = 0;
    if (match(parser, TOKEN_ELSE)) {
        consume(parser);
        if (match(parser, TOKEN_IF)) {
            consume(parser);
            expect(parser, TOKEN_LPAREN, "Se esperaba '('");
            condition_elseif = parse_expression(parser);
            expect(parser, TOKEN_RPAREN, "Se esperaba ')'");
            expect(parser, TOKEN_LBRACE,"se esperaba '{");
            while (!match(parser, TOKEN_RBRACE)) {
                ASTNode* stmt = parse_statement(parser);
                elseif_branch =realloc(elseif_branch, sizeof(ASTNode*) * (elseif_count + 1));
                elseif_branch[elseif_count++]=stmt;
            }
            expect(parser, TOKEN_RBRACE,"se esperaba '}'");
        }else {
            expect(parser, TOKEN_LBRACE, "Se esperaba '{'");
            while (!match(parser, TOKEN_RBRACE)) {
                ASTNode* stmt = parse_statement(parser);
                else_branch=realloc(else_branch, sizeof(ASTNode*) * (else_count + 1));
                else_branch[else_count++]=stmt;
            }
            expect(parser, TOKEN_RBRACE,"se esperaba '}'");
        }
    }
    return create_if(condition,then_brach,then_count,condition_elseif,elseif_branch,elseif_count,else_branch,else_count);
}
static ASTNode* parse_switch(Parser* parser) {
    expect(parser, TOKEN_SWITCH, "Se esperaba 'switch'");
    //expect(parser, TOKEN_LARROW, "Se esperaba '<-'");
    expect(parser, TOKEN_LPAREN, "Se esperaba '('");
    ASTNode* expr = parse_expression(parser);
    expect(parser, TOKEN_RPAREN, "Se esperaba ')'");
    //expect(parser, TOKEN_RARROW, "Se esperaba '->'");
    expect(parser, TOKEN_LBRACE,"se esperaba '{");
    ASTNode** cases=NULL;
    size_t cases_count = 0;
    while (!match(parser, TOKEN_RBRACE)) {
            expect(parser, TOKEN_CASE, "Se esperaba 'case'");
            ASTNode* value =parse_expression(parser);
            expect(parser,TOKEN_COLON,"Se esperaba ':'");
            ASTNode** body = NULL;
            size_t body_count = 0;
            while (!match(parser, TOKEN_RBRACE) && !match(parser, TOKEN_CASE)) {
                ASTNode* stmt = parse_statement(parser);
                body = realloc(body, sizeof(ASTNode*) * (body_count + 1));
                body[body_count++]=stmt;
            }
            cases=realloc(cases,sizeof(ASTNode*) *(cases_count + 1));
            cases[cases_count++]=create_case(value,body,body_count);
        }

    expect(parser, TOKEN_RBRACE,"se esperaba '}'");
    return create_switch(expr,cases,cases_count);
}

static ASTNode* parse_input_statement(Parser* parser) {
    ASTNode* expr=parse_primary(parser);
    expect(parser,TOKEN_SEMICOLON,"Se esperaba ';' despues del input");
    return expr;

}
/*
            expect(parser,TOKEN_COLON,"Se esperaba ':'");
            ASTNode** default_ =NULL;
            size_t default_count = 0;
            while (!match(parser, TOKEN_RBRACE)) {
                ASTNode* stmt = parse_statement(parser);
                default_ = realloc(default_, sizeof(ASTNode*) * (default_count + 1));
                default_[default_count++]=stmt;
            }
 */
static ASTNode* parse_function(Parser* parser) {
    Token name=consume(parser);
    expect(parser, TOKEN_LARROW,"se esperaba '<-'");
    expect(parser, TOKEN_LPAREN,"se esperaba '('");

    ASTNode** params=NULL;
    size_t params_count = 0;
    while (!match(parser, TOKEN_RPAREN)) {
        Token param_name=consume(parser);
        expect(parser,TOKEN_COLON,"se esperaba ':'");
        ASTNode* param_type = parse_type(parser);
        params=realloc(params,sizeof(ASTNode*) * (params_count + 1));
        params[params_count++]=create_variable(param_name.value,param_type,NULL,true);
        if (!match(parser, TOKEN_COMMA)) {break;}
        consume(parser);
    }
    expect(parser, TOKEN_RPAREN,"se esperaba ')'");
    expect(parser, TOKEN_RARROW,"se esperaba '->'");
    ASTNode* return_type=parse_type(parser);
    expect(parser, TOKEN_LBRACE,"se esperaba '{'");
    ASTNode** body=NULL;
    size_t body_count = 0;
    while (!match(parser, TOKEN_RBRACE)) {
        ASTNode* stmt = parse_statement(parser);
        body = realloc(body, sizeof(ASTNode*) * (body_count + 1));
        body[body_count++]=stmt;
    }
    expect(parser,TOKEN_RBRACE,"se esperaba '}'");
    return create_function(name.value,params,params_count,return_type,body,body_count);
}
ASTNode* parse(Token* tokens) {
    Parser parser={
        .tokens=tokens,
        .current_line=0,
        .token_count=0,
    };
    while (tokens[parser.token_count].type != TOKEN_EOF) {
        parser.token_count++;
    }
    ASTNode* program=create_program();

    while (parser.current_token < parser.token_count) {
        if (match(&parser,TOKEN_CLASS)) {
            ASTNode* class_def=parse_class(&parser);
            program->program.classes=realloc(program->program.classes,
                sizeof(ASTNode*) * (program->program.class_count + 1));
            program->program.classes[program->program.class_count++]=class_def;
        }else {
            ASTNode* function=parse_function(&parser);
            program->program.functions=realloc(
                program->program.functions,
                sizeof(ASTNode*)*(program->program.function_count+1)
                );
            program->program.functions[program->program.function_count++]=function;
        }

    }
    return program;
}
