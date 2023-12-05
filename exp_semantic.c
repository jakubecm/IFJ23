/**
 *  @file exp_semantic.c
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 */

#include "exp_semantic.h"
#include "error.h"

extern error_t error;

/**
 * @brief Function to check the semantic of the token given to expression parser
 * @param parserData - Core parser struct with all the data
 * @return Data type of the token
 */
sem_data_type_t tok_type(parser_t* parserData) { 
    data_t *tmpData;

    switch(parserData->token.type) {
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

            //Check if the variable is initialized
            tmpData = stack_lookup_var(parserData->stack, parserData->token.attribute.string);
            if (tmpData == NULL || (!tmpData->value.var_id.initialized && !tmpData->value.var_id.if_initialized)) {
                return SEM_UNDEF;
            }
            
            //According to symtable types, return the correct semantic type
            if(tmpData->type == VAR || tmpData->type == LET) {
                switch(tmpData->value.var_id.type) {
                    case VAL_INT:
                    case VAL_INTQ:
                        return SEM_INT;
                    case VAL_STRING:
                    case VAL_STRINGQ:
                        return SEM_STRING;
                    case VAL_DOUBLE:
                    case VAL_DOUBLEQ:
                        return SEM_FLOAT;
                    case VAL_BOOL:
                        return SEM_BOOL;
                    case VAL_NIL:
                        return SEM_NIL;
                    default:
                        return SEM_UNDEF;
                }
            }
            return SEM_UNDEF;

        default:
            return SEM_UNDEF;
    }
}

/**
 * @brief Function to check if the token is a string
 * @param data - Data type of the token
 * @return True if the token is a string, false otherwise
 */
bool is_string(sem_data_type_t data) {
    return (data == SEM_STRING);
}

/**
 * @brief Function to check if the token is an int
 * @param data - Data type of the token
 * @return True if the token is an int, false otherwise
 */
bool is_int(sem_data_type_t data) {
    return (data == SEM_INT);
}

/**
 * @brief Function to check if the token is a float
 * @param data - Data type of the token
 * @return True if the token is a float, false otherwise
 */
bool is_float(sem_data_type_t data) {
    return (data == SEM_FLOAT);
}

/**
 * @brief Function to check if the token is a nil
 * @param data - Data type of the token
 * @return True if the token is a nil, false otherwise
 */
bool is_nil(sem_data_type_t data) {
    return (data == SEM_NIL);
}

/**
 * @brief Function to check if the token is an operator
 * @param data - Data type of the token
 * @return True if the token is an operator, false otherwise
 */
bool is_operator(sem_data_type_t data) {
    return (data = SEM_OPERATOR);
}

/**
 * @brief Function to check if the token is an int or float
 * @param data - Data type of the token
 * @return True if the token is an int or float, false otherwise
 */
bool is_number(sem_data_type_t data) {
    return (is_float(data) || is_int(data));
}

/**
 * @brief Function to check the compatibility of the operator and operands
 * @param operator - Operator token
 * @param left - Left operand token
 * @param right - Right operand token
 * @return True if the operator and operands are compatible, false otherwise
 */
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
        case TOK_NOTEQUAL:
            return ((is_string(left->data) || is_nil(left->data)) && (is_string(right->data) || is_nil(left->data))) ||
                   ((is_number(left->data) || is_nil(left->data)) && (is_number(right->data) || is_nil(right->data)));
                   
        case TOK_LESS:
        case TOK_GREATER:
        case TOK_GREATEREQ:
        case TOK_LESSEQ:
            return (is_number(left->data) && is_number(right->data)) ||
                   (is_string(left->data) && is_string(right->data));

        case TOK_DQUESTMK:
            return ((is_string(left->data) || is_nil(left->data)) && (is_string(right->data) || is_nil(right->data))) ||
                   ((is_int(left->data) || is_nil(left->data)) && (is_int(right->data) || is_nil(right->data))) ||
                   ((is_float(left->data) || is_nil(left->data)) && (is_float(right->data) || is_nil(right->data)));

        default:
            error = ERR_SEM_TYPE;
            break;
    }

    error = ERR_SEM_TYPE;
    return false;
}

/**
 * @brief Function to get the result type of the operator and operands
 * @param operator - Operator token
 * @param left - Left operand token
 * @param right - Right operand token
 * @param parserData - Parser data
 * @return Result type of the operator and operands
 */
int get_result_type(stack_terminal_t* operator, stack_terminal_t* left, stack_terminal_t* right, parser_t* parserData, bool* id_appear) {
    if(operator->type == TOK_PLUS) {
        if(is_number(left->data) && is_number(right->data)) {
            if(is_int(left->data) && is_int(right->data)) {
                return SEM_INT;

            } else if(*id_appear && (left->data != right->data)) {
                error = ERR_SEM_TYPE;
                return SEM_UNDEF;

            //If the sem types are not the same, convert the int to float
            } else {
                if(is_int(left->data)) {
                     gen_call_exp_convert(parserData->gen);
                } else if(is_int(right->data)) {
                    gen_call_convert2(parserData->gen);
                }
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

            } else if(*id_appear && (left->data != right->data)) {
                error = ERR_SEM_TYPE;
                return SEM_UNDEF;

            //If the sem types are not the same, convert the int to float
            } else {
                if(is_int(left->data)) {
                    gen_call_exp_convert(parserData->gen);
                } else if(is_int(right->data)) {
                    gen_call_convert2(parserData->gen);
                }
                return SEM_FLOAT;
            }
        }
    }

    if(operator->type == TOK_EQUAL || operator->type == TOK_NOTEQUAL) {
        if(is_string(left->data) && (!is_string(right->data) || !is_nil(right->data))) {
            return SEM_UNDEF;
        }
        
        if(left->type == TOK_IDENTIFIER && right->type == TOK_IDENTIFIER) {
            if(left->data == right->data) {
                return SEM_BOOL;

            } else {
                return SEM_UNDEF;
            }
        }

        if((is_number(left->data) || is_nil(left->data)) && (is_number(right->data) || is_nil(left->data))) {
            if(is_int(left->data) && is_int(right->data)) {
                return SEM_BOOL;
            
            //If the sem types are not the same, convert the int to float
            } else {
                if(is_int(left->data) && !is_nil(right->data)) {
                    if(*id_appear) {
                        return SEM_UNDEF;
                    }
                    gen_call_exp_convert(parserData->gen);
                } else if(is_int(right->data) && !is_nil(left->data)) {
                    if(*id_appear) {
                        return SEM_UNDEF;
                    }
                    gen_call_convert2(parserData->gen);
                }
                return SEM_BOOL;
            }
        }
        return SEM_BOOL;
    }

    if(operator->type == TOK_LESS || operator->type == TOK_GREATER || operator->type == TOK_GREATEREQ || 
       operator->type == TOK_LESSEQ) {

        //If the sem types are not the same, convert the int to float
        if(right->data != left->data) {
            if(is_int(left->data)) {
                gen_call_exp_convert(parserData->gen);
            } else if(is_int(right->data)) {
                gen_call_convert2(parserData->gen);
            }
        }

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

bool sem_analysis(analysis_t* analysis, parser_t* parserData, bool* id_appear) {
    if (!check_operator_compatibility(analysis->tok2, analysis->tok1, analysis->tok3)) {
        error = ERR_SEM_TYPE;
        return false;
    }

    analysis->end_type = get_result_type(analysis->tok2, analysis->tok1, analysis->tok3, parserData, id_appear);
    if(analysis->end_type == SEM_UNDEF) {
        error = ERR_SEM_TYPE;
        return false;
    }

    return true;
}