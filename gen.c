/**
 *  @file gen.c
 *
 *
 *  @authors Milan Jakubec (xjakub41)
 *  @authors Jakub Ráček (xracek12)
 *  @authors Norman Babiak (xbabia01)
 */

#include "gen.h"

void gen_init(gen_t *gen)
{
    initstr(&gen->header);
    initstr(&gen->global);
    initstr(&gen->temp);
    initstr(&gen->local);
    initstr(&gen->functions);
}

void gen_free(gen_t *gen)
{
    destroy(&gen->header);
    destroy(&gen->global);
    destroy(&gen->temp);
    destroy(&gen->local);
    destroy(&gen->functions);
}

void gen_print(gen_t *gen)
{
    printf("%s", gen->header.string);
    printf("%s", "-----------------global------------------");
    printf("%s", gen->global.string);
    printf("%s", "-----------------functions---------------");
    printf("%s", gen->functions.string);

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
    // funkce predpoklada ze v globalni promenne GF@return_exp je vysledek vyrazu ktery se ma vyhodnocovat (true nebo false)
    mergestr(&gen->global, "JUMPIFNEQ $else$");
    mergestr_int(&gen->global, gen->label_counter);
    mergestr(&gen->global, " GF@return_exp bool@true\n");
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
    mergestr(&gen->global, gen->arg_counter);
    mergestr(&gen->global, "\n");

    mergestr(&gen->global, "MOVE TF@");
    mergestr(&gen->global, gen->arg_counter);
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
            mergestr(&gen->local, "ADDS\n");
            break;
            
        case TOK_MINUS:
            mergestr(&gen->local, "SUBS\n");
            break;

        case TOK_DIV:
            mergestr(&gen->local, "POPS GF@op1\n");
            mergestr(&gen->local, "POPS GF@op2\n");
            //JUMP IF op1 IS 0
            mergestr(&gen->local, "DIV GF@op1 GF@op2 GF@op1\n");
            mergestr(&gen->local, "PUSHS GF@op1\n");
            break;
            
        case TOK_MUL:
            mergestr(&gen->local, "MULS\n");
            break;

        case TOK_EQUAL:
            mergestr(&gen->local, "EQS\n");
            break;

        case TOK_NOTEQUAL:
            mergestr(&gen->local, "EQS\nNOTS\n");
            break;

        case TOK_GREATER:
            mergestr(&gen->local, "GTS\n");
            break;

        case TOK_LESS:
            mergestr(&gen->local, "LTS\n");
            break;

        case TOK_GREATEREQ:
            mergestr(&gen->local, "LTS\nNOTS\n");
            break;

        case TOK_LESSEQ:
            mergestr(&gen->local, "GTS\nNOTS\n");
            break;
        
        case TOK_DQUESTMK:
            mergestr(&gen->local, "POPS GF@op1\n");
            mergestr(&gen->local, "POPS GF@op2\n");

            mergestr(&gen->local, "JUMPIFEQ !op2NIL nil@nil GF@op2\n");
            mergestr(&gen->local, "JUMPIFNEQ !op2NotNIL nil@nil GF@op2\n");
            break;

        default:
            return;
    }
}

//This goes to buildin!
void gen_nil_check(gen_t *gen) {
    mergestr(&gen->global, "LABEL !op2NIL\n");
    mergestr(&gen->local, "PUSHS GF@op1\n");
    mergestr(&gen->local, "RETURN\n");

    mergestr(&gen->global, "LABEL !op2NotNIL\n");
    mergestr(&gen->local, "PUSHS GF@op2\n");
    mergestr(&gen->local, "RETURN\n");
}