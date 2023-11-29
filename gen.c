/**
 *  @file gen.c
 *
 *
 *  @authors Milan Jakubec (xjakub41)
 *  @authors Jakub Ráček (xracek12)
 *  @authors Norman Babiak (xbabia01)
 */

#include "buildin.h"
#include "symtable.h"

void gen_init(gen_t *gen);
void gen_free(gen_t *gen);
void gen_header(gen_t *gen);
void gen_main(gen_t *gen);
void gen_var_definition(gen_t *gen, token_t* token, bool in_function);
void gen_func(gen_t *gen, token_t *name);
void gen_arguments(gen_t *gen, vector_t *gen_arguments);
void gen_func_call(gen_t *gen, char *name);
void gen_if(gen_t *gen);
void gen_else(gen_t *gen);
void gen_endif(gen_t *gen);
void gen_while(gen_t *gen);
void gen_while_exit(gen_t *gen);
void gen_while_end(gen_t *gen);
void gen_push_int(gen_t *gen, int value);
void gen_push_float(gen_t *gen, double value);
void gen_push_string(gen_t *gen, char *value);
void gen_push_nil(gen_t *gen);
void gen_push_var(gen_t *gen, char *name, bool local);
void gen_argdef_var(gen_t *gen, char *name, bool local);
void gen_expression(gen_t *gen, token_type_t operator);
void gen_call_convert(gen_t *gen);
void gen_call_convert2(gen_t *gen);
void gen_print(gen_t *gen);

void gen_init(gen_t *gen)
{
    initstr(&gen->header);
    initstr(&gen->global);
    initstr(&gen->temp);
    initstr(&gen->local);
    initstr(&gen->functions);

    gen->label_counter = 0;
    gen->arg_counter = 0;

    gen_header(gen);
    gen_main(gen);
}

void gen_free(gen_t *gen)
{
    destroy(&gen->header);
    destroy(&gen->global);
    destroy(&gen->temp);
    destroy(&gen->local);
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

void gen_var_definition(gen_t *gen, token_t* token, bool in_function)
{
    if (in_function) {
        mergestr(&gen->local, "DEFVAR LF@");
        mergestr(&gen->local, token->attribute.string);
        mergestr(&gen->local, "\n");
    } else {
        mergestr(&gen->global, "DEFVAR GF@");
        mergestr(&gen->global, token->attribute.string);
        mergestr(&gen->global, "\n");
    }
}

// Paremeter return type??
void gen_func(gen_t *gen, token_t *name){
    mergestr(&gen->functions, "LABEL $");
    mergestr(&gen->functions, name->attribute.string);
    mergestr(&gen->functions, "\n");
}

void gen_arguments(gen_t *gen, vector_t *gen_arguments) {    
    mergestr(&gen->global, "CREATEFRAME\n");
}

void gen_func_call(gen_t *gen, char *name)
{
    mergestr(&gen->global, "CALL $");
    mergestr(&gen->global, name);
    mergestr(&gen->global, "\n");
}

void gen_if(gen_t *gen)
{
    mergestr(&gen->global, "POPS GF@return_exp\n");
    // funkce predpoklada ze v globalni promenne GF@return_exp je vysledek vyrazu ktery se ma vyhodnocovat (true nebo false)
    mergestr(&gen->global, "JUMPIFNEQ $else$");
    mergestr_int(&gen->global, gen->label_counter);
    mergestr(&gen->global, " GF@return_exp bool@false\n");
}

void gen_else(gen_t *gen)
{
    mergestr(&gen->global, "JUMP $endif$");
    mergestr_int(&gen->global, gen->label_counter);
    mergestr(&gen->global, "\n");

    mergestr(&gen->global, "LABEL $else$");
    mergestr_int(&gen->global, gen->label_counter);
    mergestr(&gen->global, "\n");
}

void gen_endif(gen_t *gen)
{
    mergestr(&gen->global, "LABEL $endif$");
    mergestr_int(&gen->global, gen->label_counter);
    mergestr(&gen->global, "\n");
    gen->label_counter++;
}

void gen_while(gen_t *gen)
{
    mergestr(&gen->global, "LABEL $while$");
    mergestr_int(&gen->global, gen->label_counter);
    mergestr(&gen->global, "\n");
}

void gen_while_exit(gen_t *gen)
{
    mergestr(&gen->global, "POPS GF@return_exp\n"); // beru ze zasobniku vysledek vyrazu
    mergestr(&gen->global, "JUMPIFEQ $whileEnd$");
    mergestr_int(&gen->global, gen->label_counter);
    mergestr(&gen->global, " GF@return_exp bool@false\n");
}

void gen_while_end(gen_t *gen)
{
    mergestr(&gen->global, "JUMP $while$");
    mergestr_int(&gen->global, gen->label_counter);
    mergestr(&gen->global, "\n");

    mergestr(&gen->global, "LABEL $whileEnd$");
    mergestr_int(&gen->global, gen->label_counter);
    mergestr(&gen->global, "\n");
    gen->label_counter++;
}

void gen_push_int(gen_t *gen, int value)
{
    mergestr(&gen->global, "PUSHS int@");
    mergestr_int(&gen->global, value);
    mergestr(&gen->global, "\n");
}

void gen_push_float(gen_t *gen, double value)
{
    mergestr(&gen->global, "PUSHS float@");
    mergestr_float(&gen->global, value);
    mergestr(&gen->global, "\n");
}

void gen_push_string(gen_t *gen, char *value)
{
    mergestr(&gen->global, "PUSHS string@");
    mergestr(&gen->global, value);
    mergestr(&gen->global, "\n");
}

void gen_push_nil(gen_t *gen)
{
    mergestr(&gen->global, "PUSHS nil@nil\n");
}

void gen_push_var(gen_t *gen, char *name, bool local) {
    if (local) {
        mergestr(&gen->global, "PUSHS LF@");
    } else {
        mergestr(&gen->global, "PUSHS GF@");
    }
    mergestr(&gen->global, name);
    mergestr(&gen->global, "\n");
}

void gen_argdef_var(gen_t *gen, char *name, bool local)
{
    mergestr(&gen->global, "DEFVAR TF@");
    mergestr_int(&gen->global, gen->arg_counter);
    mergestr(&gen->global, "\n");

    mergestr(&gen->global, "MOVE TF@");
    mergestr_int(&gen->global, gen->arg_counter);
    mergestr(&gen->global, " ");
    if (local) {
        mergestr(&gen->global, "LF@");
    } else {
        mergestr(&gen->global, "GF@");
    }
    mergestr(&gen->global, name);
}

void gen_expression(gen_t *gen, token_type_t operator) {
    switch(operator) {
        case TOK_PLUS:
            mergestr(&gen->global, "ADDS\n");
            break;
            
        case TOK_MINUS:
            mergestr(&gen->global, "SUBS\n");
            break;

        case TOK_DIV:
            mergestr(&gen->global, "POPS GF@op1\n");
            mergestr(&gen->global, "POPS GF@op2\n");
            //JUMP IF op1 IS 0
            mergestr(&gen->global, "DIV GF@op1 GF@op2 GF@op1\n");
            mergestr(&gen->global, "PUSHS GF@op1\n");
            break;
            
        case TOK_MUL:
            mergestr(&gen->global, "MULS\n");
            break;

        case TOK_EQUAL:
            mergestr(&gen->global, "EQS\n");
            break;

        case TOK_NOTEQUAL:
            mergestr(&gen->global, "EQS\nNOTS\n");
            break;

        case TOK_GREATER:
            mergestr(&gen->global, "GTS\n");
            break;

        case TOK_LESS:
            mergestr(&gen->global, "LTS\n");
            break;

        case TOK_GREATEREQ:
            mergestr(&gen->global, "LTS\nNOTS\n");
            break;

        case TOK_LESSEQ:
            mergestr(&gen->global, "GTS\nNOTS\n");
            break;
        
        case TOK_DQUESTMK:
            mergestr(&gen->global, "CALL $nilCheck\n");
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

void gen_print(gen_t *gen) {
    printstr(&gen->header);
    printstr(&gen->global);
    printstr(&gen->functions);
}