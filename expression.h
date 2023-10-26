/**
 *  @file expression.h
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 */

#include "token.h"
#include "stack.h"

typedef struct analysis {
    stack_terminal_t* tok1;
    stack_terminal_t* tok2;
    stack_terminal_t* tok3;
    sem_data_type_t end_type;
} analysis_t;

/**
 *  @brief Function for getting the precedence from the table
 *  @param top - Top token from the stack
 *  @param input - Input token from scanner
 */ 
int precedence(stack_terminal_t* top, token_t input);

/**
 *  @brief Function for parsing expressions
 *  @param 
 *  @param
 */
void exp_parsing(parser_t* parserData);