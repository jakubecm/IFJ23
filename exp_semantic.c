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
            return SEM_INT;
        case TOK_DOUBLE:
            return SEM_FLOAT;
        case TOK_STRING:
        case TOK_MLSTRING:
            return SEM_STRING;
        case K_NIL:
            return SEM_NIL;
        default:
            return SEM_UNDEF;
    }
}

sem_data_type_t tok_term_type(stack_terminal_t* token) { 
    switch(token->type) {
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
            return SEM_INT;
        case TOK_DOUBLE:
            return SEM_FLOAT;
        case TOK_STRING:
        case TOK_MLSTRING:
            return SEM_STRING;
        case K_NIL:
            return SEM_NIL;
        default:
            return SEM_UNDEF;
    }
}

bool is_string(sem_data_type_t data) {
    return (data == SEM_STRING);
}

bool is_int(sem_data_type_t data) {
    return (data == SEM_INT);
}

bool is_float(sem_data_type_t data) {
    return (data == SEM_FLOAT);
}


bool sem_analysis(analysis_t* analysis) {
    bool tok1_float = false, tok3_float = false;

    switch(analysis->tok2->type) {   
        case TOK_PLUS:
            analysis->end_type = SEM_FLOAT;
            if((is_string(analysis->tok1->data)) && (is_string(analysis->tok3->data))) {
                analysis->end_type = TOK_STRING;
                break;
               }
            
            if((is_string(analysis->tok1->data)) || (is_string(analysis->tok3->data))) {
                error = ERR_SEM_TYPE;
                return false;
               }

            if((is_int(analysis->tok1->data)) && (is_int(analysis->tok3->data))) {
                analysis->end_type = SEM_INT;
                break;
               }
            
            tok1_float = true;
            tok3_float = true;
            break;

        case TOK_MINUS:
        case TOK_MUL:
            analysis->end_type = SEM_FLOAT;
            if((is_string(analysis->tok1->data)) && (is_string(analysis->tok3->data))) {
                analysis->end_type = TOK_STRING;
                break;
               }
            
            if((is_string(analysis->tok1->data)) || (is_string(analysis->tok3->data))) {
                error = ERR_SEM_TYPE;
                return false;
               }

            if((is_int(analysis->tok1->data)) && (is_int(analysis->tok3->data))) {
                analysis->end_type = SEM_INT;
                break;
               }
            
            tok1_float = true;
            tok3_float = true;
            break;

        case TOK_DIV:
            analysis->end_type = SEM_FLOAT;
            if((is_string(analysis->tok1->data)) || (is_string(analysis->tok3->data))) {
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
        default:
            error = ERR_SEM_TYPE;
            return false;
    }

    if(tok1_float) {
        //generate code for conversion
    }

    if(tok3_float) {
        //generate code for conversion
    }

    return false;
}
