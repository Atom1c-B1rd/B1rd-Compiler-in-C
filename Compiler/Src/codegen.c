#include "codegen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void generate_type(FILE* output, ASTNode* type_node) {
    if (!type_node) {
        fprintf(output, "void");
        return;
    }
    switch (type_node->type) {
        case NODE_TYPE_PRIMITIVE:
            if (strcmp(type_node->primitive_type.type_name, "Number") == 0) {
                fprintf(output, "double");
            }
            else if (strcmp(type_node->primitive_type.type_name, "Letter") == 0) {
                fprintf(output, "std::string");
            }
            else if (strcmp(type_node->primitive_type.type_name, "Void") == 0) {
                fprintf(output, "void");
            }
            else {
                fprintf(output, "%s",type_node->primitive_type.type_name);
            }
            break;
        case NODE_TYPE_ARRAY:
            fprintf(output, "\tvector<");
            generate_type(output, type_node->array_type.base_type);
            fprintf(output, ">");
            break;
        case NODE_CLASS:
            fprintf(output, "\t%s",type_node->class_def.name);
            break;
        default:
            fprintf(output, "\tauto");
            break;
    }
}

void generate_codegen(ASTNode* ast, const char* output_filename) {
    FILE *output = fopen(output_filename, "w");
    if (!output) {
        printf("Error opening output file\n");
        exit(1);
    }

    fprintf(output, "#include <iostream>\n");
    fprintf(output, "#include <string>\n");
    fprintf(output, "#include <vector>\n");
    fprintf(output, "using namespace std;\n\n");
    generate_program(output, ast);
    fclose(output);
}

void generate_program(FILE* output, ASTNode* program) {
    for (size_t i=0; i<program->program.class_count;i++) {
        generate_class(output,program->program.classes[i]);
    }
    for (size_t i=0;i<program->program.function_count;i++) {
        generate_function(output, program->program.functions[i]);
    }

    fprintf(output, "int main() {\n");
    fprintf(output, "\t_main();\n");
    fprintf(output, "\treturn 0;\n");
    fprintf(output, "}\n");
}
void generate_class(FILE* output, ASTNode* class_def) {
    if (!class_def) return;
    fprintf(output,"class %s{\npublic:\n",class_def->class_def.name);

    for (size_t i=0; i<class_def->class_def.properties_count;i++) {
        ASTNode* prop = class_def->class_def.properties[i];
        generate_type(output, prop->variable.var_type);
        fprintf(output,"\t%s",prop->variable.name);
        if (prop->variable.value) {
            fprintf(output,"=");
            generate_expression(output, prop->variable.value);
        }
        fprintf(output,";\n");
    }
    for (size_t i=0;i<class_def->class_def.method_count;i++) {
        generate_function(output,class_def->class_def.methods[i]);
    }
    fprintf(output,"};\n");

}
void generate_function(FILE* output, ASTNode* function) {
    if (!function) {
        printf("Error: no function defined\n");
        return;
    }
    bool is_main=strcmp(function->function.name,"main")==0;
    if (is_main) {
        fprintf(output,"void _main(");
    }else {
        generate_type(output, function->function.return_type);
        fprintf(output," %s(",function->function.name);
    }
    for (size_t i=0; i<function->function.param_count;i++) {
        ASTNode* param = function->function.params[i];
        generate_type(output,param->variable.var_type);
        fprintf(output," %s",param->variable.name);
        if (i<function->function.param_count-1) {
            fprintf(output,", ");
        }
    }
    fprintf(output,"){\n");
    for (size_t i=0; i<function->function.body_count;i++) {
        generate_statement(output,function->function.body[i]);
    }
    fprintf(output,"}\n\n");

}
void generate_statement(FILE* output, ASTNode* stmt) {
    switch (stmt->type) {
        case NODE_VARIABLE: {
            generate_type(output, stmt->variable.var_type);
            fprintf(output,"\t%s",stmt->variable.name);
            if (stmt->variable.value) {
                fprintf(output,"=");
                generate_expression(output, stmt->variable.value);
            }
            fprintf(output,";\n");
            break;
        }
        case NODE_PRINT: {
            fprintf(output,"\tcout << ");
            for (size_t i =0; i<stmt->print_stmt.arg_count;i++) {
                generate_expression(output, stmt->print_stmt.args[i]);
                if (i<stmt->print_stmt.arg_count-1) {
                    fprintf(output," << ");
                }
            }
            fprintf(output,"<< endl;\n");
            break;
        }
        case NODE_RETURN: {
            fprintf(output,"\treturn ");
            generate_expression(output, stmt->return_stmt.value);
            fprintf(output,";\n");
            break;
        }
        case NODE_IF:{
            fprintf(output,"\tif(");
            generate_expression(output, stmt->if_stmt.condition);
            fprintf(output,"){\n");
            for (size_t i=0; i<stmt->if_stmt.then_count;i++) {
                generate_statement(output,stmt->if_stmt.then_branch[i]);
            }
            fprintf(output,"}\n");

            if (stmt->if_stmt.else_branch) {
                fprintf(output,"\telse{\n");
                for (size_t i=0;i<stmt->if_stmt.else_count;i++) {
                    generate_statement(output,stmt->if_stmt.else_branch[i]);
                }
                fprintf(output,"}\n");
            }
            break;
        }
            case NODE_ASSIGN: {
            generate_expression(output,stmt->assignment.target);
            fprintf(output," = ");
            generate_expression(output,stmt->assignment.value);
            fprintf(output,";\n");
        }
            default: {
            fprintf(output,"\t ");
            generate_expression(output,stmt);
            fprintf(output,";\n");
            break;
        }
    }
}

void generate_expression(FILE* output, ASTNode* expr) {
    switch (expr->type) {
        case NODE_LITERAL:
        case NODE_LITERAL_NUMBER:
            fprintf(output,"%s", expr->literal.value);
            break;
        case NODE_LITERAL_LETTER:
            fprintf(output,"\"%s\"", expr->literal.value);
            break;
        case NODE_IDENTIFIER:
            fprintf(output,"%s", expr->literal.value);
            break;
        case NODE_BINARY_OP: {
            generate_expression(output, expr->binary_op.left);
            switch (expr->binary_op.op) {
                case TOKEN_PLUS:fprintf(output," + ");break;
                case TOKEN_MINUS:fprintf(output," - ");break;
                case TOKEN_MUL:fprintf(output," * ");break;
                case TOKEN_DIV:fprintf(output," / ");break;
                case TOKEN_NEQ:fprintf(output," != ");break;
                case TOKEN_EQ:fprintf(output," == ");break;
                case TOKEN_GT:fprintf(output," > ");break;
                case TOKEN_LT:fprintf(output," < ");break;
                case TOKEN_GEQ:fprintf(output," >= ");break;
                case TOKEN_LEQ:fprintf(output," <= ");break;
                default: printf("operacion binary not detected");
            }
            generate_expression(output, expr->binary_op.right);
            break;
        }
        case NODE_THIS: {
            fprintf(output,"this->");
            break;
        }
        case NODE_FUNCTION_CALL:{
                generate_expression(output,expr->function_call.func);
                fprintf(output,"(");
                for (size_t i=0; i<expr->function_call.arg_count;i++) {
                    generate_expression(output,expr->function_call.args[i]);
                    if (i<expr->function_call.arg_count-1) {
                        fprintf(output,",");
                    }
                }
                fprintf(output,")");
                break;
                }
        case NODE_PROPERTY_ACCESS: {
            if (!expr->property_access.property_name || !expr->property_access.object) {
                printf("Error PropertyNull");
                return;
            }
            generate_expression(output,expr->property_access.object);
            if (!expr->property_access.is_this) {
                fprintf(output,".");
            }
            generate_expression(output,expr->property_access.property_name);
            break;
        }
        case NODE_METHOD_CALL: {
            generate_expression(output,expr->method_call.object);
            fprintf(output,".%s(",expr->method_call.method_name);
            for (size_t i=0; i<expr->method_call.arg_count;i++) {
                generate_expression(output,expr->method_call.args[i]);
                if (i<expr->method_call.arg_count-1) {
                    fprintf(output,",");
                }
            }
            fprintf(output,")");
            break;
        }
        case NODE_CLASS_INSTANCE: {
            fprintf(output,"new %s()",expr->class_instance.class_ref->literal.value);
            break;
        }
            case NODE_INPUT: {
            fprintf(output, "([]()->");
            generate_type(output,expr->input_stmt.input_type);
            fprintf(output,"{");
            if(expr->input_stmt.prompt) {
               fprintf(output, "cout <<");
                generate_expression(output, expr->input_stmt.prompt);
                fprintf(output,";\n");
            }
            fprintf(output,"string tmp;cin>>tmp;");
            if (expr->input_stmt.input_type->primitive_type.type_name &&
                strcmp(expr->input_stmt.input_type->primitive_type.type_name,"Number")==0) {
                fprintf(output,"return stod(tmp);}())");
            }else {
                fprintf(output,"}())");
            }
            break;
        }
            default: {
            printf("operacion binary not detected");
            break;
        }
    }
}