/**
 *  @file expression.h
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 */

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "token.h"
#include "parser.h"
#include "stack.h"

/**
 *  @brief Function for getting the precedence from the table
 *  @param top - Top token from the stack
 *  @param input - Input token from scanner
 */ 
int precedence(stack_terminal_t* top, token_t* input);

/**
 *  @brief Function for parsing expressions
 *  @param 
 *  @param
 */
void exp_parsing(parser_t* parserData);

#endif