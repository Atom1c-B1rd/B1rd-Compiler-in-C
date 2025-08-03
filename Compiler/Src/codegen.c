//
// Created by Elbar on 31/7
#include "codegen.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static const char* map_type(ASTNode* type_node) {
    if (!type_node) return "void";

    if (type_node->type==NODE_TYPE_PRIMITIVE) {
        if (strcmp(type_node->primitive_type.type_name,"Number")==0) {return "double";}
        if (strcmp(type_node->primitive_type.type_name,"Letter")==0) {return "char*";}
        if (strcmp(type_node->primitive_type.type_name,"Void")==0) {return "void";}
        return type_node->primitive_type.type_name;
    }
    if (type_node->type==NODE_TYPE_ARRAY) {
        return "double*";
    }
    fprintf(stderr,"Tipo no reconosido");
}

void generate_expression(ASTNode* node, FILE* output) {
    if (!node) {
        fprintf(stderr, "Error:Expresion nula\n");
        return;
    }
    printf("[DEBUG] Nodo en generate_expression: %d\n", node->type);
    switch (node->type) {
        case NODE_LITERAL:
            assert(node->literal.value && "Literal number es nulo");
            fprintf(output, "%s", node->literal.value);
            break;
        case NODE_LITERAL_LETTER:
            assert(node->literal.value && "Literal Letter es nulo");
            fprintf(output, "\"%s\"", node->literal.value);
            break;
        case NODE_IDENTIFIER:
            assert(node->literal.value && "Identifier es nulo");
            fprintf(output, "%s", node->literal.value);
            break;
        case NODE_BINARY_OP: {
            assert(node->binary_op.left && "Binary operation left es nulo");
            assert(node->binary_op.right && "Binary operation right es nulo");
            char* op="";
            switch (node->binary_op.op) {
                case TOKEN_PLUS:op="+";break;
                case TOKEN_MINUS:op="-";break;
                case TOKEN_MUL:op="*";break;
                case TOKEN_DIV:op="/";break;
                case TOKEN_EQ:op="==";break;
                case TOKEN_NEQ:op="!=";break;
                case TOKEN_LT:op="<";break;
                case TOKEN_LEQ:op="<=";break;
                case TOKEN_GT:op=">";break;
                case TOKEN_GEQ:op=">=";break;
                default:
                    fprintf(stderr,"Error: Operador binario desconocido");
                    op="??";
                    break;
            }
            fprintf(output, "(");
            generate_expression(node->binary_op.left, output);
            fprintf(output, "%s", op);
            generate_expression(node->binary_op.right, output);
            fprintf(output, ")");
            break;
        }
        case NODE_UNARY_OP: {
            assert(node->unary_op.expr && "Binary operation left es nulo");
        }
        case NODE_FUNCTION_CALL:
            assert(node->function_call.func &&"Function call nulo");
            fprintf(output, "%s(", node->function_call.func->literal.value);
            for (size_t i=0; i<node->function_call.arg_count;i++) {
                assert(node->function_call.args && "Function call array arguments es nulo");
                assert(node->function_call.args[i] && "Function call arguments es nulo");
                generate_expression(node->function_call.args[i], output);
                if (i<node->function_call.arg_count - 1) {fprintf(output, ",");}
            }
            fprintf(output, ")");
            break;
        case NODE_ARRAY_ACCESS: {
            assert(node->array_access.array && "Array access es nulo");
            assert(node->array_access.index && "Array access index es nulo");
            generate_expression(node->array_access.array, output);
            fprintf(output, "[");
            generate_expression(node->array_access.index, output);
            fprintf(output, "]");
            break;
        }
        case NODE_THIS: {
            fprintf(output,"self");
            break;
        }
        case NODE_PROPERTY_ACCESS: {
            generate_statement(node->property_access.object, output);
            fprintf(output,"->");
            generate_statement(node->property_access.property_name, output);
            break;
        }
        case NODE_METHOD_CALL: {
            fprintf(output,"%s_%s(",node->method_call.object->literal.value,
                node->method_call.method_name);
            for (size_t i=0; i<node->class_def.method_count; i++) {
                generate_expression(node->method_call.args[i],output);
                if (i<node->class_def.method_count-1) {fprintf(output,",");}
            }
            fprintf(output,");");
            break;
        }
            default:
                fprintf(stderr, "Error:Tipo de expresion no soportado\n");
                exit(1);
    }

}

void generate_statement(ASTNode* node, FILE* output) {
    if (!node) {
        fprintf(stderr, "Error:Intento de generar declarasion nula\n");
        return;
    }
    switch (node->type) {
        case NODE_VARIABLE: {
            assert(node->variable.name && "Nombre Variable de nulo");
            assert(node->variable.var_type && "Tipo Variable de nulo");

            const char* type=map_type(node->variable.var_type);
            if (!node->variable.is_mutable) {
                fprintf(output, "const ");
            }
            fprintf(output, "%s %s", type, node->variable.name);
            if (node->variable.value) {
                fprintf(output, "=");
                generate_statement(node->variable.value, output);
            }else {
                if
                (strcmp(type, "char*")==0){fprintf(output,"= NULL");}
                else if (strcmp(type, "double")==0){fprintf(output,"= 0");}
            }
            fprintf(output, ";\n");
            break;
        }
        case NODE_PRINT:
            assert(node->print_stmt.args && "Print args array es nulo");
            assert(node->print_stmt.arg_count && "Print args es nulo");
            assert(node->print_stmt.args[0] && "Print args array es nulo");
            fprintf(output, "printf( ");
            if (node->print_stmt.arg_count > 0) {
                fprintf(output, "\"");
                for (size_t i=0; i<node->print_stmt.arg_count; i++) {
                    if (node->print_stmt.args[0]->type == NODE_LITERAL_LETTER ) {
                        fprintf(output, "%%s, ");
                    }else{
                        fprintf(output, "%%f, ");
                    }
                }
                fprintf(output, "\";\n");
                for (size_t i=0; i<node->print_stmt.arg_count; i++) {
                    if (node->print_stmt.args[i]->type != NODE_LITERAL_LETTER ) {
                        fprintf(output, ", ");
                        generate_statement(node->print_stmt.args[i], output);
                    }
                }
            }
            fprintf(output, ");\n");
            break;
        case NODE_RETURN:
            fprintf(output, "return");
            if (node->return_stmt.value) {
                fprintf(output, " ");
                generate_expression(node->return_stmt.value, output);
            }
            fprintf(output, ";\n");
            break;
        case NODE_IF: {
            assert(node->if_stmt.condition && " IF Condition es nulo");
            fprintf(output, "if (");
            generate_expression(node->if_stmt.condition, output);
            fprintf(output, ") {\n");
            for (size_t i=0; i<node->if_stmt.then_count; i++) {
                assert(node->if_stmt.then_branch && "IF array es nulo");
                assert(node->if_stmt.then_branch[i] && "IF es nulo");
                fprintf(output, "   ");
                generate_statement(node->if_stmt.then_branch[i], output);
            }
            if (node->if_stmt.elseif_count>0) {
                assert(node->if_stmt.elseif_branch && "Else array es nulo");
                assert(node->if_stmt.elseif_condition && " IF Condition es nulo");
                fprintf(output, "} else if(");
                generate_expression(node->if_stmt.elseif_condition, output);
                fprintf(output, ") {\n");
                for (size_t i=0; i<node->if_stmt.elseif_count; i++) {
                    assert(node->if_stmt.elseif_branch[i]&&"Else es Nulo");
                    fprintf(output, "   ");
                    generate_statement(node->if_stmt.elseif_branch[i], output);
                }
                fprintf(output, "}\n");
            }
            if (node->if_stmt.else_count>0) {
                assert(node->if_stmt.else_branch && "Else array es nulo");
                fprintf(output, "} else {");
                for (size_t i=0; i<node->if_stmt.else_count; i++) {
                    assert(node->if_stmt.else_branch[i]&&"Else es Nulo");
                    fprintf(output, "   ");
                    generate_statement(node->if_stmt.else_branch[i], output);
                }

            }
            fprintf(output, "}\n");
            break;
        }
            case NODE_SWITCH: {
                assert(node->switch_stmt.expr && "Switch Expresion nula");
                fprintf(output, "switch (");
                if (node->switch_stmt.expr->type != NODE_LITERAL_LETTER) {
                    fprintf(output, "(int)");
                }
                generate_expression(node->switch_stmt.expr, output);
                fprintf(output, "){\n");
                for (size_t i=0; i<node->switch_stmt.case_count; i++) {
                    assert(node->switch_stmt.cases && "Switch array es nulo");
                    assert(node->switch_stmt.cases[i] && "Switch es nulo");
                    fprintf(output, "   ");
                    generate_statement(node->switch_stmt.cases[i], output);
                }
                fprintf(output,"}\n ");
                break;
            }
            case NODE_CASE: {
                assert(node->case_stmt.value && "Case Value nulo");
                fprintf(output, "case ");
                generate_expression(node->case_stmt.value, output);
                fprintf(output, ":\n");
                for (size_t i=0; i<node->case_stmt.body_count; i++) {
                    assert(node->case_stmt.body && "Case Array es nulo");
                    assert(node->case_stmt.body[i] && "Case es nulo");
                    fprintf(output, "   ");
                    generate_statement(node->case_stmt.body[i], output);
                }
                fprintf(output, "break;\n");
                break;
            }
        case NODE_INPUT: {
            assert(node->input_stmt.input_type && "input type nulo");
            assert(node->input_stmt.prompt && "prompt nulo");
            if (strcmp(node->input_stmt.input_type->literal.value, "Number") == 0) {
                fprintf(output,"InputNumber(");
                generate_expression(node->input_stmt.prompt, output);
                fprintf(output, ")");
            }else if (strcmp(node->input_stmt.input_type->literal.value, "Letter")) {
                fprintf(output,"InputLetter(");
                generate_expression(node->input_stmt.prompt,output);
                fprintf(output, ")");
            }

            break;
        }
        case NODE_CLASS: {
            assert(node->class_def.name && "Class name nulo");
            fprintf(output, "typedef struct {\n");
            for (size_t i=0; i < node->class_def.properties_count; i++) {
                ASTNode* prop=node->class_def.properties[i];
                const char* type=map_type(prop->variable.var_type);
                fprintf(output, "%s %s;",type,prop->variable.name);
            }
            fprintf(output, "}%s;\n",node->class_def.name);

            for (size_t i=0;i<node->class_def.method_count;i++) {
                ASTNode* method=node->class_def.methods[i];
                method->function.name=malloc(strlen(node->class_def.name)+strlen(method->function.name)+2);
                sprintf(method->function.name,"%s_%s",node->class_def.name,method->function.name);
                generate_function(method,output);
            }
            break;
        }
        case NODE_CLASS_INSTANCE: {
            fprintf(output,"%s* instance=malloc(sizeof(%s));\n",
            node->class_instance.class_ref->literal.value,
            node->class_instance.class_ref->literal.value);
            break;
        }

        default:
            generate_expression(node,output);
            fprintf(output, ";\n");
            break;
        }
    }

void generate_function(ASTNode* node, FILE* output) {
    if (!node||node->type != NODE_FUNCTION) {
        fprintf(stderr, "Error: Nodo de funcion invalido");
        return;
    }
    assert(node->function.name && "Nombre Function es nulo");
    assert(node->function.return_type && "Tipo Function es nulo");
    char* ret_type=map_type(node->function.return_type);
    char* func_name=node->function.name;
    if (strcmp(func_name,"main")==0) {
        func_name="Main";
    }
    fprintf(output, "%s %s(", ret_type,func_name);
    for (size_t i=0;i<node->function.param_count;i++) {
        assert(node->function.params && "Param array es nulo");
        assert(node->function.params[i] && "Param es nulo");
        assert(node->function.params[i]->variable.name && "Name Param es nulo");
        assert(node->function.params[i]->variable.var_type && "Type Param es nulo");
        char* param_type =map_type(node->function.params[i]->variable.var_type);
        fprintf(output, " %s %s", param_type,node->function.params[i]->variable.name);
        if (i<node->function.param_count-1) {fprintf(output,", ");}
    }
    fprintf(output, "){\n");
    for (size_t i=0;i<node->function.body_count;i++) {
        assert(node->function.body && "body array es nulo");
        assert(node->function.body[i] && "body es nulo");
        fprintf(output, "   ");
        generate_statement(node->function.body[i], output);
    }
    if (strcmp(ret_type,"void")==0 ||
        (node->function.body==0 || node->function.body[node->function.body_count-1]->type !=NODE_RETURN)){
        fprintf(output, "return;\n");
        }
    fprintf(output, "}\n\n");
}
void generate_codegen(ASTNode* node, char* output_filename) {
    if (!node||node->type != NODE_PROGRAM) {
        fprintf(stderr, "Error: Nodo de Programa invalido invalido");
        return;
    }
    FILE* output=fopen(output_filename,"w");
    if (!output) {
        perror("error al abrir archivo de salida");
        return;
    }
    fprintf(output,"#include <stdio.h>\n");
    fprintf(output,"#include <stdlib.h>\n\n");
    fprintf(output,"double InputNumber(char* prompt){char input_buffer[256];printf(\"%%s\",prompt);fgets(input_buffer,sizeof(input_buffer),stdin);return atoi(input_buffer);}\n");
    fprintf(output,"char* InputLetter(char* prompt){char input_buffer[256];printf(\"%%s\",prompt);fgets(input_buffer,sizeof(input_buffer),stdin);char*res=input_buffer[strcspn(input_buffer,\"\\n\")]=0;free(input_buffer);return res;}\n");

    for (size_t i=0;i<node->program.function_count;i++) {
        assert(node->program.functions && "Function array es nulo");
        assert(node->program.functions[i] && "Function es nulo");
        ASTNode* func=node->program.functions[i];
        char* ret_type=map_type(func->function.return_type);
        char* func_name=func->function.name;
        if (strcmp(func_name,"main")==0) {
            func_name="Main";
        }
        fprintf(output,"%s %s(",ret_type,func_name);
        for (size_t j=0; j<func->function.param_count;j++) {
            assert(func->function.params && "Param array es nulo");
            assert(func->function.params[j] && "Param es nulo");
            fprintf(output," %s %s",map_type(func->function.params[j]->variable.var_type) ,func->function.params[j]->variable.name);
            if (j<func->function.param_count-1) {fprintf(output,", ");}
        }
        fprintf(output,");\n");
    }
    fprintf(output,"\n");
    for (size_t i=0;i<node->program.function_count;i++) {
        generate_function(node->program.functions[i], output);
    }
    fprintf(output,"int main(){\n");
    bool has_main;
    for (size_t i = 0; i < node->program.function_count; i++) {
        if (strcmp(node->program.functions[i]->function.name,"main")==0) {
            has_main=true;
            fprintf(output, "Main();\n");
            fprintf(output, "return 0;\n");
            break;
        }

    }
    if (!has_main) {
        fprintf(stderr, "main function is not defined\n");
    }
    fprintf(output, "}\n");
    fclose(output);
}