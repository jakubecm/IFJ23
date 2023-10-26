/**
 *  @file exp_semantic.c
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 */

//TODO: PR_DQUE
//      ADDING GENERATORS

#include "exp_semantic.h"
#include "expression.h"
#include "error.h"
#include "stack.h"

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

bool sem_analysis(analysis_t* analysis) {
    bool tok1_float = false, tok3_float = false;

    switch(analysis->tok2->type) {   
        case TOK_PLUS:
        case TOK_MINUS:
        case TOK_MUL:
            analysis->end_type = SEM_FLOAT;
            if((analysis->tok1->data == SEM_STRING) && (analysis->tok3->data == SEM_STRING) && (analysis->tok2->type = TOK_PLUS)) {
                analysis->end_type = TOK_STRING;
                break;
               }
            
            if((analysis->tok1->data == SEM_STRING) || (analysis->tok3->data == SEM_STRING)) {
                error = ERR_SEM_TYPE;
                return false;
               }

            if((analysis->tok1->data == SEM_INT) && (analysis->tok3->data == SEM_INT)) {
                analysis->end_type = SEM_INT;
                break;
               }
            
            tok1_float = true;
            tok3_float = true;

        case TOK_DIV:
            analysis->end_type = SEM_FLOAT;
            if((analysis->tok1->data == SEM_STRING) || (analysis->tok3->data == SEM_STRING)) {
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
            analysis->end_type = SEM_BOOL;

            if(analysis->tok1->data != analysis->tok3->data) {
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
