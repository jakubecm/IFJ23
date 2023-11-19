/**
 *  @file parser.h
 *
 *
 *  @authors Milan Jakubec (xjakub41)
 *  @authors Jakub Ráček (xracek12)
 *  @brief Semantic and syntax analysis header file
 */

#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include "stack.h"
#include <stdbool.h>

typedef struct parser
{
    scanner_t *scanner;     // Scanner instance
    stack_t *stack;         // Stack instance
    htab_value_t *function; // Current function
    token_t token;          // Current token
    token_t next_token;     // Next token

    // States of parser
    bool in_function;
    bool func_is_void;
    bool returned;
} parser_t;

#endif // PARSER_H

/**
 * @brief Initializes parser
 *
 * @param scanner Pointer to scanner
 * @return Initialized parser structure
 */

void parser_init(parser_t *parser, scanner_t *scanner);

/**
 * @brief Runs parser
 */
void run_parser(parser_t *parser);

/**
 * @brief Destroys parser
 */
void parser_destroy(parser_t *parser);