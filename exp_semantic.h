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

/**
 *  @brief Function to get the data type of the token
 *  @param parserData - Core parser struct with all the data
 *  @return Semantic data type of the token
 */
sem_data_type_t tok_type(parser_t* parserData);

/**
 *  @brief Function to get the data type of the token
 *  @param token - Token, but used on the stack already
 *  @return Semantic data type of the token
 */
sem_data_type_t tok_term_type(stack_terminal_t* token);

/**
 *  @brief Function to run the semantic analysis for expressions
 *  @param analysis - Analysis struct with all the tokens
 *  @param parserData - Core parser struct with all the data
 *  @return Data type of the token, true if it was successful, false if not
 */
bool sem_analysis(analysis_t* analysis, parser_t* parserData, bool* id_appear);


#endif // EXP_SEMANTIC_H
