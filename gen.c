/**
 *  @file gen.c
 *
 *
 *  @authors Milan Jakubec (xjakub41)
 *  @authors Jakub Ráček (xracek12)
 *  @authors Norman Babiak (xbabia01)
 */

#include "buildin.h"

void gen_init(gen_t *gen);
void gen_free(gen_t *gen);
void gen_header(gen_t *gen);
void gen_main(gen_t *gen);
void gen_var_definition(gen_t *gen, token_t* token, bool in_function, bool in_if);
void gen_func(gen_t *gen, token_t *name);
void gen_func_end(gen_t *gen);
void gen_func_return_to_var(gen_t *gen, char *name, bool in_function);
void gen_arguments(gen_t *gen, vector_t *gen_arguments, bool in_function, bool in_if);
void gen_func_call(gen_t *gen, char *name, bool in_function);
void gen_if(gen_t *gen, bool in_function);
void gen_else(gen_t *gen, bool in_function);
void gen_endif(gen_t *gen, bool in_function);
void gen_while(gen_t *gen, bool in_function);
void gen_while_exit(gen_t *gen, bool in_function);
void gen_while_end(gen_t *gen, bool in_function);
void gen_push_int(gen_t *gen, int value, bool in_function);
void gen_push_float(gen_t *gen, double valu, bool in_function);
void gen_push_string(gen_t *gen, char *value, bool in_function);
void gen_push_nil(gen_t *gen, bool in_function);
void gen_push_var(gen_t *gen, char *name, bool local);
void gen_argdef_var(gen_t *gen, char *name, bool local);
void gen_expression(gen_t *gen, token_type_t operator, bool in_function);
void gen_call_convert(gen_t *gen);
void gen_call_convert2(gen_t *gen);
void gen_print(gen_t *gen);
void gen_pop_value(gen_t* gen, char* name, bool in_function, bool in_if);

void gen_init(gen_t *gen)
{
    initstr(&gen->header);
    initstr(&gen->global);
    initstr(&gen->temp);
    initstr(&gen->footer);
    initstr(&gen->functions);

    gen->label_counter = 0;
    gen->arg_counter = 0;

    gen_header(gen);
    gen_main(gen);
    gen_footer(gen);
}

void gen_free(gen_t *gen)
{
    destroy(&gen->header);
    destroy(&gen->global);
    destroy(&gen->temp);
    destroy(&gen->footer);
    destroy(&gen->functions);
}

void gen_header(gen_t *gen)
{
    mergestr(&gen->header, ".IFJcode23\n");
    mergestr(&gen->header, "JUMP $$main\n");
}

void gen_main(gen_t *gen)
{
    mergestr(&gen->global, "LABEL $$main\n");
    mergestr(&gen->global, "CREATEFRAME\n");
    mergestr(&gen->global, "PUSHFRAME\n");

    mergestr(&gen->global, "DEFVAR GF@tmp\n");
    mergestr(&gen->global, "DEFVAR GF@op1\n");
    mergestr(&gen->global, "DEFVAR GF@op2\n");
    mergestr(&gen->global, "DEFVAR GF@op3\n");
    mergestr(&gen->global, "DEFVAR GF@return_func\n");
    mergestr(&gen->global, "DEFVAR GF@return_exp\n");

    gen_buildin_funcs(gen); // Generates all buildin functions at once (check buildin.c to see)
}

void gen_footer(gen_t *gen)
{
    mergestr(&gen->footer, "POPFRAME\n");
    mergestr(&gen->footer, "EXIT int@0\n");
    mergestr(&gen->footer, "LABEL !ERR_DIV\n");
    mergestr(&gen->footer, "POPFRAME\n");
    mergestr(&gen->footer, "EXIT int@9\n");
}



void gen_var_definition(gen_t *gen, token_t* token, bool in_function, bool in_if)
{
    if (in_function) {
        mergestr(&gen->functions, "DEFVAR LF@");
        mergestr(&gen->functions, token->attribute.string);
        mergestr(&gen->functions, "\n");
    } else if (!in_function && in_if){
        mergestr(&gen->global, "DEFVAR LF@");
        mergestr(&gen->global, token->attribute.string);
        mergestr(&gen->global, "\n");
    } else {
        mergestr(&gen->global, "DEFVAR GF@");
        mergestr(&gen->global, token->attribute.string);
        mergestr(&gen->global, "\n");
    }
}

void gen_func(gen_t *gen, token_t *token){
    mergestr(&gen->functions, "LABEL $");
    mergestr(&gen->functions, token->attribute.string);
    mergestr(&gen->functions, "\n");
    mergestr(&gen->functions, "PUSHFRAME\n");
}

void gen_func_end(gen_t *gen){
    mergestr(&gen->functions, "POPS GF@return_func\n");
    mergestr(&gen->functions, "POPFRAME\n");
    mergestr(&gen->functions, "RETURN\n");
}

void gen_func_return_to_var(gen_t *gen, char *name, bool in_function){
    if(in_function){
        mergestr(&gen->functions, "MOVE LF@");
        mergestr(&gen->functions, name);
        mergestr(&gen->functions, " GF@return_func\n");

    }
    else{
        mergestr(&gen->global, "MOVE GF@");
        mergestr(&gen->global, name);
        mergestr(&gen->global, " GF@return_func\n");
    }
}

void gen_parameters(gen_t *gen, vector_t *parameters) {
    
    for(int i = 0; i < parameters->size; i++) {
        mergestr(&gen->functions, "DEFVAR LF@");
        mergestr(&gen->functions, parameters->data[i].def_name);
        mergestr(&gen->functions, "\n");

        mergestr(&gen->functions, "POPS LF@");
        mergestr(&gen->functions, parameters->data[i].def_name);
        mergestr(&gen->functions, "\n");
    }
}

void gen_arguments(gen_t *gen, vector_t *gen_arguments, bool in_function, bool in_if) {

    if(in_function){
        mergestr(&gen->functions, "CREATEFRAME\n");
        for (int i = gen_arguments->size - 1; i >= 0; i--)
        {
            switch (gen_arguments->data[i].parameter.type)
            {
            case VAL_INT:
                mergestr(&gen->functions, "PUSHS int@");
                mergestr_int(&gen->functions, gen_arguments->data[i].parameter.value.number);
                mergestr(&gen->functions, "\n");
                break;
            case VAL_DOUBLE:
                mergestr(&gen->functions, "PUSHS float@");
                mergestr_float(&gen->functions, gen_arguments->data[i].parameter.value.decimal);
                mergestr(&gen->functions, "\n");
                break;
            case VAL_STRING:
                mergestr(&gen->functions, "PUSHS string@");
                mergestr(&gen->functions, gen_arguments->data[i].parameter.value.string);
                mergestr(&gen->functions, "\n");
                break;
            case VAL_NIL:
                mergestr(&gen->functions, "PUSHS nil@nil\n");
                break;
            case VAL_ID:
                mergestr(&gen->functions, "PUSHS LF@");
                mergestr(&gen->functions, gen_arguments->data[i].parameter.value.string);
                mergestr(&gen->functions, "\n");
                break;
            }
        }
    }
    else{
        mergestr(&gen->global, "CREATEFRAME\n");
        for (int i = gen_arguments->size - 1; i >= 0; i--)
        {
            switch (gen_arguments->data[i].parameter.type)
            {
            case VAL_INT:
                mergestr(&gen->global, "PUSHS int@");
                mergestr_int(&gen->global, gen_arguments->data[i].parameter.value.number);
                mergestr(&gen->global, "\n");
                break;
            case VAL_DOUBLE:
                mergestr(&gen->global, "PUSHS float@");
                mergestr_float(&gen->global, gen_arguments->data[i].parameter.value.decimal);
                mergestr(&gen->global, "\n");
                break;
            case VAL_STRING:
                mergestr(&gen->global, "PUSHS string@");
                mergestr(&gen->global, gen_arguments->data[i].parameter.value.string);
                mergestr(&gen->global, "\n");
                break;
            case VAL_NIL:
                mergestr(&gen->global, "PUSHS nil@nil\n");
                break;
            case VAL_ID:
                if(in_if){
                    mergestr(&gen->global, "PUSHS LF@");
                }
                else{
                    mergestr(&gen->global, "PUSHS GF@");
                }
                mergestr(&gen->global, gen_arguments->data[i].parameter.value.string);
                mergestr(&gen->global, "\n");
                break;
            }
        }
    }

}

void gen_func_call(gen_t *gen, char *name, bool in_function)
{
    if(in_function){
        mergestr(&gen->functions, "CALL $");
        mergestr(&gen->functions, name);
        mergestr(&gen->functions, "\n");
    }
    else{
        mergestr(&gen->global, "CALL $");
        mergestr(&gen->global, name);
        mergestr(&gen->global, "\n");
    }
}

void gen_if(gen_t *gen, bool in_function)
{
    // funkce predpoklada ze v globalni promenne GF@return_exp je vysledek vyrazu ktery se ma vyhodnocovat (true nebo false)
    if(in_function){
        mergestr(&gen->functions, "POPS GF@return_exp\n");
        mergestr(&gen->functions, "JUMPIFEQ $else$");
        mergestr_int(&gen->functions, gen->label_counter);
        mergestr(&gen->functions, " GF@return_exp bool@false\n");
    }
    else{
        mergestr(&gen->global, "POPS GF@return_exp\n");
        mergestr(&gen->global, "JUMPIFEQ $else$");
        mergestr_int(&gen->global, gen->label_counter);
        mergestr(&gen->global, " GF@return_exp bool@false\n");
    }
}

void gen_else(gen_t *gen, bool in_function)
{
    if(in_function){
        mergestr(&gen->functions, "JUMP $endif$");
        mergestr_int(&gen->functions, gen->label_counter);
        mergestr(&gen->functions, "\n");

        mergestr(&gen->functions, "LABEL $else$");
        mergestr_int(&gen->functions, gen->label_counter);
        mergestr(&gen->functions, "\n");
    }
    else{
        mergestr(&gen->global, "JUMP $endif$");
        mergestr_int(&gen->global, gen->label_counter);
        mergestr(&gen->global, "\n");

        mergestr(&gen->global, "LABEL $else$");
        mergestr_int(&gen->global, gen->label_counter);
        mergestr(&gen->global, "\n");
    }
}

void gen_endif(gen_t *gen, bool in_function)
{
    if(in_function){
        mergestr(&gen->functions, "LABEL $endif$");
        mergestr_int(&gen->functions, gen->label_counter);
        mergestr(&gen->functions, "\n");
        gen->label_counter++;
    }
    else{
        mergestr(&gen->global, "LABEL $endif$");
        mergestr_int(&gen->global, gen->label_counter);
        mergestr(&gen->global, "\n");
        gen->label_counter++;
    }
}

void gen_while(gen_t *gen, bool in_function)
{
    if(in_function){
        mergestr(&gen->functions, "LABEL $while$");
        mergestr_int(&gen->functions, gen->label_counter);
        mergestr(&gen->functions, "\n");
    }
    else{
        mergestr(&gen->global, "LABEL $while$");
        mergestr_int(&gen->global, gen->label_counter);
        mergestr(&gen->global, "\n");
    }
}

void gen_while_exit(gen_t *gen, bool in_function)
{
    if(in_function){
        mergestr(&gen->functions, "POPS GF@return_exp\n"); // beru ze zasobniku vysledek vyrazu
        mergestr(&gen->functions, "JUMPIFEQ $whileEnd$");
        mergestr_int(&gen->functions, gen->label_counter);
        mergestr(&gen->functions, " GF@return_exp bool@false\n");
    }
    else{
        mergestr(&gen->global, "POPS GF@return_exp\n"); // beru ze zasobniku vysledek vyrazu
        mergestr(&gen->global, "JUMPIFEQ $whileEnd$");
        mergestr_int(&gen->global, gen->label_counter);
        mergestr(&gen->global, " GF@return_exp bool@false\n");
    }
}

void gen_while_end(gen_t *gen, bool in_function)
{
    if(in_function){
        mergestr(&gen->functions, "JUMP $while$");
        mergestr_int(&gen->functions, gen->label_counter);
        mergestr(&gen->functions, "\n");

        mergestr(&gen->functions, "LABEL $whileEnd$");
        mergestr_int(&gen->functions, gen->label_counter);
        mergestr(&gen->functions, "\n");
        gen->label_counter++;
    }
    else{
        mergestr(&gen->global, "JUMP $while$");
        mergestr_int(&gen->global, gen->label_counter);
        mergestr(&gen->global, "\n");

        mergestr(&gen->global, "LABEL $whileEnd$");
        mergestr_int(&gen->global, gen->label_counter);
        mergestr(&gen->global, "\n");
        gen->label_counter++;
    }
}

void gen_push_int(gen_t *gen, int value, bool in_function)
{
    if(in_function){
        mergestr(&gen->functions, "PUSHS int@");
        mergestr_int(&gen->functions, value);
        mergestr(&gen->functions, "\n");
    }
    else{
        mergestr(&gen->global, "PUSHS int@");
        mergestr_int(&gen->global, value);
        mergestr(&gen->global, "\n");
    }
}

void gen_push_float(gen_t *gen, double value, bool in_function)
{
    if(in_function){
        mergestr(&gen->functions, "PUSHS float@");
        mergestr_float(&gen->functions, value);
        mergestr(&gen->functions, "\n");
    }
    else{
        mergestr(&gen->global, "PUSHS float@");
        mergestr_float(&gen->global, value);
        mergestr(&gen->global, "\n");
    }
}

void gen_push_string(gen_t *gen, char *value, bool in_function)
{
    if(in_function){
        mergestr(&gen->functions, "PUSHS string@");
        mergestr(&gen->functions, value);
        mergestr(&gen->functions, "\n");
    }
    else{
        mergestr(&gen->global, "PUSHS string@");
        mergestr(&gen->global, value);
        mergestr(&gen->global, "\n");
    }
}

void gen_push_nil(gen_t *gen, bool in_function)
{
    if(in_function){
        mergestr(&gen->functions, "PUSHS nil@nil\n");
    }
    else{
        mergestr(&gen->global, "PUSHS nil@nil\n");
    }
}

void gen_push_var(gen_t *gen, char *name, bool local) {
    if(local) {
        mergestr(&gen->functions, "PUSHS LF@");
        mergestr(&gen->functions, name);
        mergestr(&gen->functions, "\n");
    }
    else {
        mergestr(&gen->global, "PUSHS GF@");
        mergestr(&gen->global, name);
        mergestr(&gen->global, "\n");
    }
}

void gen_expression(gen_t *gen, token_type_t operator, bool in_function) {
    switch(operator) {
        case TOK_PLUS:
            if(in_function){
                mergestr(&gen->functions, "ADDS\n");
            }
            else{
                mergestr(&gen->global, "ADDS\n");
            }
            break;
            
        case TOK_MINUS:
            if(in_function){
                mergestr(&gen->functions, "SUBS\n");
            }
            else{
                mergestr(&gen->global, "SUBS\n");
            }
            break;

        case TOK_DIV:
            if(in_function){
                mergestr(&gen->functions, "POPS GF@op1\n");
                mergestr(&gen->functions, "POPS GF@op2\n");
                //JUMP IF op1 IS 0
                mergestr(&gen->functions, "DIV GF@op1 GF@op2 GF@op1\n");
                mergestr(&gen->functions, "PUSHS GF@op1\n");
            }
            else{
                mergestr(&gen->global, "POPS GF@op1\n");
                mergestr(&gen->global, "POPS GF@op2\n");
                //JUMP IF op1 IS 0
                mergestr(&gen->global, "DIV GF@op1 GF@op2 GF@op1\n");
                mergestr(&gen->global, "PUSHS GF@op1\n");
            }
            break;
            
        case TOK_MUL:
            if(in_function){
                mergestr(&gen->functions, "MULS\n");
            }
            else{
                mergestr(&gen->global, "MULS\n");
            }
            break;

        case TOK_EQUAL:
            if(in_function){
                mergestr(&gen->functions, "EQS\n");
            }
            else{
                mergestr(&gen->global, "EQS\n");
            }
            break;

        case TOK_NOTEQUAL:
            if(in_function){
                mergestr(&gen->functions, "EQS\nNOTS\n");
            }
            else{
                mergestr(&gen->global, "EQS\nNOTS\n");
            }
            break;

        case TOK_GREATER:
            if(in_function){
                mergestr(&gen->functions, "GTS\n");
            }
            else{
                mergestr(&gen->global, "GTS\n");
            }
            break;

        case TOK_LESS:
            if(in_function){
                mergestr(&gen->functions, "LTS\n");
            }
            else{
                mergestr(&gen->global, "LTS\n");
            }
            break;

        case TOK_GREATEREQ:
            if(in_function){
                mergestr(&gen->functions, "LTS\nNOTS\n");
            }
            else{
                mergestr(&gen->global, "LTS\nNOTS\n");
            }
            break;

        case TOK_LESSEQ:
            if(in_function){
                mergestr(&gen->functions, "GTS\nNOTS\n");
            }
            else{
                mergestr(&gen->global, "GTS\nNOTS\n");
            }
            break;
        
        case TOK_DQUESTMK:
            if(in_function) {
                mergestr(&gen->functions, "CALL $nilCheck\n");
            } else {
                mergestr(&gen->global, "CALL $nilCheck\n");
            }
            break;

        default:
            return;
    }
}

void gen_call_convert(gen_t *gen) {
    mergestr(&gen->global, "CALL $int2float\n");
}

void gen_call_convert2(gen_t *gen) {
    mergestr(&gen->global, "CALL $int2float2\n");
}

void gen_pop_value(gen_t* gen, char* name, bool in_function, bool in_if) {
    if (in_function) {
        mergestr(&gen->functions, "POPS LF@");
        mergestr(&gen->functions, name);
        mergestr(&gen->functions, "\n");
    } else if (!in_function && in_if){
        mergestr(&gen->global, "POPS LF@");
        mergestr(&gen->global, name);
        mergestr(&gen->global, "\n");
    }else {
        mergestr(&gen->global, "POPS GF@");
        mergestr(&gen->global, name);
        mergestr(&gen->global, "\n");
    }
}

void gen_print(gen_t *gen) {
    printstr(&gen->header);
    printstr(&gen->global);
    printstr(&gen->footer);
    printstr(&gen->functions);
}