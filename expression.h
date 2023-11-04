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

#ifdef DEBUG
#define DEBUG_PRINT printf
#else
#define DEBUG_PRINT(...)
#endif

#define CLEANUP_RESOURCES(stack, analysis) do { \
            /*stack_free_token(&(stack));*/     \
            analysis_free((analysis));          \
            free((analysis));                   \
        } while (0)


/**
 *  @brief Main function for parsing expressions
 *  @param parserData - core parser structure for giving scanner and first token to exp parser.
 */
void exp_parsing(parser_t* parserData);

#endif