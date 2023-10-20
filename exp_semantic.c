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

bool sem_analysis(stack_terminal_t* tok1, stack_terminal_t* tok2, stack_terminal_t* tok3, rules_enum rule, sem_data_type_t *end_type) {
    switch(rule) {
        case PR_NOT:
            if(tok2->data != SEM_STRING) {
                error = ERR_SEM_INCOMPATIBLE;
                return false;
            }

            *end_type = tok_term_type(tok2);
            break;

        case PR_BRACKET:
            if(tok2->data == TOK_UNDEF) {
                error = ERR_SEM_NDEF;
                return false;
            } else {
                *end_type = tok_term_type(tok2);
                break;
            }
            
        case PR_PLUS:
        case PR_MINUS:
        case PR_MUL:
            *end_type = TOK_DOUBLE;
            if((tok1->data == SEM_STRING) && (tok3->data == SEM_STRING) && (rule = PR_PLUS)) {
                *end_type = TOK_STRING;
                break;
               }
            
            if((tok1->data == SEM_STRING) || (tok3->data == SEM_STRING)) {
                error = ERR_SEM_TYPE;
                return false;
               }

            if((tok1->data == TOK_INT || tok1->data == K_INTE || tok1->data == K_INTQ)
                &&
               (tok3->data == TOK_INT || tok3->data == K_INTE || tok3->data == K_INTQ)) {
                *end_type = TOK_INT;
                break;
               }
            // NEED TO ADD GENERATING TO DIFFERENT TYPES!!!!

        case PR_DIV:
            *end_type = TOK_DOUBLE;
            if((tok1->data == SEM_STRING) || (tok3->data == SEM_STRING)) {
                error = ERR_SEM_TYPE;
                return false;
               }
            
            //GENERATOR FOR INTO TO DOUBLE
            break;

        case PR_LESS:
        case PR_MORE:
        case PR_EQ:
        case PR_NEQ:
        case PR_MEQ:
        case PR_LEQ:
            *end_type = TOK_BOOL;

            //GENERATOR FOR INT TO DOUBLE

            if(tok1->data != tok3->data) {
                error = ERR_SEM_TYPE;
                return false;
            }
            break;

        case PR_DQUE:
        case PR_OP:
            if(tok1->data == TOK_BOOL) {
                error = ERR_SEM_TYPE;
                return false;
            } else if(tok1->data == TOK_UNDEF) {
                error = ERR_SEM_NDEF;
                return false;
            } else {
                *end_type = tok_term_type(tok2);
                break;
            }
    }
}
