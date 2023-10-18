/**
 *  @file expression.h
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 */

#include "token.h"

/**
 * @brief Parsing rules
 */
typedef enum exp_rules {
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
    PR_MORE     // E -> E > E

} rules_enum;

/**
 *  @brief Data types used for semantic analysis and exp parsing
 */
typedef enum data_types {
    INT,
    DOUBLE,
    STRING,
    UNDEFINED
} data_enum;

/**
 *  @brief Function for getting the precedence from the table
 *  @param top - Top token from the stack
 *  @param input - Input token from scanner
 */ 
int precedence(token_t top, token_t input);


rules_enum test(int number, stack_terminal_t *tok1, stack_terminal_t *tok2, stack_terminal_t *tok3);

/**
 *  @brief Function for parsing expressions
 *  @param 
 *  @param
 */
void exp_parsing(int a, int b, int c);