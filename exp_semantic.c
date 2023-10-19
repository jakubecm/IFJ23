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
#include "token.h"

error_t error;

bool sem_analysis(stack_terminal_t* tok1, stack_terminal_t* tok2, stack_terminal_t* tok3, rules_enum rule, variable_type_t *end_type) {
    switch(rule) {
        case PR_NOT:
            if(tok2->data != TOK_STRING || tok2->data != TOK_EMSTRING || tok2->data != K_STRINGQ ) {
                error = ERR_SEM_INCOMPATIBLE;
                return false;
            }

            *end_type = tok2->data;
            break;

        case PR_BRACKET:
            if(tok2->data == TOK_UNDEF) {
                error = ERR_SEM_NDEF;
                return false;
            } else {
                *end_type = tok2->data;
                break;
            }
            
        case PR_PLUS:
        case PR_MINUS:
        case PR_MUL:
            *end_type = TOK_DOUBLE;
            if((tok1->data == TOK_STRING || tok1->data == TOK_EMSTRING || tok1->data == K_STRINGQ)
                &&
               (tok3->data == TOK_STRING || tok3->data == TOK_EMSTRING || tok3->data == K_STRINGQ)
                &&
               (rule = PR_PLUS)) {
                *end_type = TOK_STRING;
                break;
               }
            
            if((tok1->data == TOK_STRING || tok1->data == TOK_EMSTRING || tok1->data == K_STRINGQ)
                ||
               (tok3->data == TOK_STRING || tok3->data == TOK_EMSTRING || tok3->data == K_STRINGQ)) {
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
            if((tok1->data == TOK_STRING || tok1->data == TOK_EMSTRING || tok1->data == K_STRINGQ)
                ||
               (tok3->data == TOK_STRING || tok3->data == TOK_EMSTRING || tok3->data == K_STRINGQ)) {
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
                *end_type = tok1->data;
                break;
            }
    }
}
