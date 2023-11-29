/**
 *  @file exp_semantic.h
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 */

#ifndef EXP_SEMANTIC_H
#define EXP_SEMANTIC_H

#include <stdio.h>
#include "expression.h"

struct analysis;

sem_data_type_t tok_type(parser_t* parserData);
sem_data_type_t tok_term_type(stack_terminal_t* token);

/**
 *  @brief Function to check the semantic of the expression given to expression parser
 *  @param tok1 - Token on the top of stack
 *  @param tok2 - Token under the the token on top of stack
 *  @param tok3 - Token 3rd in row on the top of stack
 *  @param rule - Rule given to sem. analysis by expression parser
 *  @param end_type - the final type of the expression (including generator changes)
 */
bool sem_analysis(analysis_t* analysis, parser_t* parserData);


#endif // EXP_SEMANTIC_H