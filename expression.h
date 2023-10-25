/**
 *  @file expression.h
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 */

#include "token.h"
#include "stack.h"

/**
 * @brief Parsing rules
 */
/* typedef enum exp_rules {
    PR_UNDEF,
    PR_OP,      // E -> i
    PR_NOT,     // E -> !E
    PR_BRACKET, // E -> (E)
    PR_PLUS,    // E -> E + E
    PR_MINUS,   // E -> E - E
    PR_MUL,     // E -> E * E
    PR_DIV,     // E -> E / E
    PR_EQ,      // E -> E == E
    PR_NEQ,     // E -> E != E
    PR_LEQ,     // E -> E <= E
    PR_MEQ,     // E -> E >= E
    PR_LESS,    // E -> E < E
    PR_MORE,    // E -> E > E
    PR_DQUE     // E -> E ?? E

} rules_enum; */

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