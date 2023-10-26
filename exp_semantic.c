/**
 *  @file exp_semantic.c
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 */

//TODO: PR_DQUE
//      ADDING GENERATORS

#include "exp_semantic.h"
#include "error.h"
#include "stack.h"
#include "expression.h"

error_t error;

sem_data_type_t tok_type(token_t token) { 
    switch(token.type) {
        case TOK_NOT:
        case TOK_PLUS:
        case TOK_MINUS:
        case TOK_MUL:
        case TOK_DIV:
        case TOK_GREATER:
        case TOK_LESS:
        case TOK_EQUAL:
        case TOK_NOTEQUAL:
        case TOK_GREATEREQ:
        case TOK_LESSEQ:
        case TOK_DQUESTMK:
            return SEM_OPERATOR;
        case TOK_INT:
        case TOK_EINT:
        case K_INT:
        case K_INTE:
        case K_INTQ:
            return SEM_INT;
        case TOK_DOUBLE:
        case TOK_EDOUBLE:
        case K_DOUBLE:
        case K_DOUBLEE:
        case K_DOUBLEQ:
            return SEM_FLOAT;
        case TOK_STRING:
        case TOK_MLSTRING:
        case K_STRING:
        case K_STRINGE:
        case K_STRINGQ:
            return SEM_STRING;
    }
}

sem_data_type_t tok_term_type(stack_terminal_t* token) { 
    switch(token->data) {
        case TOK_NOT:
        case TOK_PLUS:
        case TOK_MINUS:
        case TOK_MUL:
        case TOK_DIV:
        case TOK_GREATER:
        case TOK_LESS:
        case TOK_EQUAL:
        case TOK_NOTEQUAL:
        case TOK_GREATEREQ:
        case TOK_LESSEQ:
        case TOK_DQUESTMK:
            return SEM_OPERATOR;
        case TOK_INT:
        case TOK_EINT:
        case K_INT:
        case K_INTE:
        case K_INTQ:
            return SEM_INT;
        case TOK_DOUBLE:
        case TOK_EDOUBLE:
        case K_DOUBLE:
        case K_DOUBLEE:
        case K_DOUBLEQ:
            return SEM_FLOAT;
        case TOK_STRING:
        case TOK_MLSTRING:
        case K_STRING:
        case K_STRINGE:
        case K_STRINGQ:
            return SEM_STRING;
    }
}

bool sem_analysis(stack_terminal_t* tok1, stack_terminal_t* tok2, stack_terminal_t* tok3, sem_data_type_t *end_type) {
    bool tok1_float = false, tok3_float = false;

    switch(tok2->type) {   
        case TOK_PLUS:
        case TOK_MINUS:
        case TOK_MUL:
            *end_type = SEM_FLOAT;
            if((tok1->data == SEM_STRING) && (tok3->data == SEM_STRING) && (tok2->type = TOK_PLUS)) {
                *end_type = TOK_STRING;
                break;
               }
            
            if((tok1->data == SEM_STRING) || (tok3->data == SEM_STRING)) {
                error = ERR_SEM_TYPE;
                return false;
               }

            if((tok1->data == SEM_INT) && (tok3->data == SEM_INT)) {
                *end_type = SEM_INT;
                break;
               }
            
            tok1_float = true;
            tok3_float = true;

        case TOK_DIV:
            *end_type = SEM_FLOAT;
            if((tok1->data == SEM_STRING) || (tok3->data == SEM_STRING)) {
                error = ERR_SEM_TYPE;
                return false;
               }
            
            tok1_float = true;
            tok3_float = true;
            break;

        case TOK_LESS:
        case TOK_GREATER:
        case TOK_EQUAL:
        case TOK_NOTEQUAL:
        case TOK_GREATEREQ:
        case TOK_LESSEQ:
            *end_type = SEM_BOOL;

            if(tok1->data != tok3->data) {
                tok1_float = true;
                tok3_float = true;
            }
            break;

        case TOK_DQUESTMK:
    }

    if(tok1_float) {
        //generate code for conversion
    }

    if(tok3_float) {
        //generate code for conversion
    }
}
