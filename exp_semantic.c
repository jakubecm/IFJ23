/**
 *  @file exp_semantic.c
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 */

//TODO: ADDING GENERATORS

#include "exp_semantic.h"
#include "error.h"

extern error_t error;

sem_data_type_t tok_type(token_t token) { 
    switch(token.type) {
        case TOK_NOT:
        case TOK_PLUS:
        case TOK_MINUS:
        case TOK_MUL:
        case TOK_DIV:
        case TOK_LBRACKET:
        case TOK_RBRACKET:
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
        case TOK_IDENTIFIER:
            //ADD SEARCH IN SYMTABLE
            return SEM_STRING;
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

bool is_nil(sem_data_type_t data) {
    return (data == SEM_NIL);
}

bool is_operator(sem_data_type_t data) {
    return (data = SEM_OPERATOR);
}

bool is_number(sem_data_type_t data) {
    return (is_float(data) || is_int(data));
}

bool check_operator_compatibility(stack_terminal_t* operator, stack_terminal_t* left, stack_terminal_t* right) {
    switch(operator->type) {
        case TOK_PLUS:
            return (is_string(left->data) && is_string(right->data)) ||
                   (is_number(left->data) && is_number(right->data));

        case TOK_MINUS:
        case TOK_MUL:
        case TOK_DIV:
            return is_number(left->data) && is_number(right->data);
        
        case TOK_EQUAL:
        //TODO string and string or int and int or double or double else error
        case TOK_LESS:
        case TOK_GREATER:
        case TOK_NOTEQUAL:
        case TOK_GREATEREQ:
        case TOK_LESSEQ:
            return is_number(left->data) && is_number(right->data);

        case TOK_DQUESTMK:
            return true;

        default:
            error = ERR_SEM_TYPE;
            break;
    }

    error = ERR_SEM_TYPE;
    return false;
}

int get_result_type(stack_terminal_t* operator, stack_terminal_t* left, stack_terminal_t* right) {
    if(operator->type == TOK_PLUS) {
        if(is_number(left->data) && is_number(right->data)) {
            if(is_int(left->data) && is_int(right->data)) {
                return SEM_INT;
            } else {
                //left, right to float gen
                return SEM_FLOAT;
            }
        } else {
            return SEM_STRING;
        }
    }

    if(operator->type == TOK_MINUS || operator->type == TOK_MUL || operator->type == TOK_DIV) {
        if(is_number(left->data) && is_number(right->data)) {
            if(is_int(left->data) && is_int(right->data)) {
                return SEM_INT;
            } else {
                //left, right to float gen
                return SEM_FLOAT;
            }
        }
    }

    if(operator->type == TOK_LESS || operator->type == TOK_GREATER || operator->type == TOK_EQUAL ||
       operator->type == TOK_NOTEQUAL || operator->type == TOK_GREATEREQ || operator->type == TOK_LESSEQ) {
        if(right->data != left->data) {
            //Separate tok equal since it doesnt change 
            //left, right to float gen
        }
        //printf("to bool\n");
        return SEM_BOOL;
    }

    if(operator->type == TOK_DQUESTMK) {
        if(is_nil(right->data)) {
            return left->data;

        } else if(is_nil(right->data) && is_nil(left->data)) {
            return SEM_NIL;

        } else if(is_nil(left->data)) {
            return right->data;

        } else {
            return left->data;
        }
    }

    return SEM_UNDEF;
}

bool sem_analysis(analysis_t* analysis) {
    if (!check_operator_compatibility(analysis->tok2, analysis->tok1, analysis->tok3)) {
        error = ERR_SEM_TYPE;
        //printf("here\n");
        return false;
    }
    //printf("here 1\n");
    analysis->end_type = get_result_type(analysis->tok2, analysis->tok1, analysis->tok3);
    if(analysis->end_type == SEM_UNDEF) {
        error = ERR_SEM_TYPE;
        return false;
    }

    return true;
}

/**
int main() {
    stack_terminal_t* tok1 = malloc(sizeof(stack_terminal_t)); // Allocate memory for tok1
    stack_terminal_t* tok2 = malloc(sizeof(stack_terminal_t)); // Allocate memory for tok2
    stack_terminal_t* tok3 = malloc(sizeof(stack_terminal_t)); // Allocate memory for tok3

    tok2->type = TOK_DQUESTMK;
    tok1->type = K_NIL;
    tok3->type = K_NIL;
    tok1->data = tok_term_type(tok1);
    tok2->data = tok_term_type(tok2);
    tok3->data = tok_term_type(tok3);

    analysis_t analysis;
    analysis.tok1 = tok1;
    analysis.tok2 = tok2;
    analysis.tok3 = tok3;
    analysis.end_type = SEM_UNDEF;

    printf("%d %d %d\n", analysis.tok1->type, analysis.tok2->type, analysis.tok3->type);

    sem_analysis(&analysis);
    printf("END TYPE: %d\n", analysis.end_type);
    printf("%d\n", error);


    free(tok1); // Free the allocated memory when done
    free(tok2);
    free(tok3);

    return 0;
}
*/