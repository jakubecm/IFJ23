/**
 *  @file exp_semantic.h
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 */

#include <stdio.h>

typedef enum sem_data_type {
    SEM_OPERATOR,
    SEM_INT,
    SEM_FLOAT,
    SEM_STRING,
    SEM_BOOL,
    SEM_UNDEF
} sem_data_type_t;

/**
 *  @brief Function to check the semantic of the expression given to expression parser
 *  @param tok1 - Token on the top of stack
 *  @param tok2 - Token under the the token on top of stack
 *  @param tok3 - Token 3rd in row on the top of stack
 *  @param rule - Rule given to sem. analysis by expression parser
 *  @param end_type - the final type of the expression (including generator changes)
 */
bool sem_analysis(stack_terminal_t* tok1, stack_terminal_t* tok2, stack_terminal_t* tok3, rules_enum rule, variable_type_t *end_type);
