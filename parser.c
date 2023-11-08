/**
 *  @file parser.c
 * 
 * 
 *  @authors Milan Jakubec (xjakub41)
 *  @authors Jakub Ráček (xracek12)
 */

#include "parser.h"


void parser_init(parser_t *parser, scanner_t *scanner)
{
    parser->scanner = scanner;
    parser->stack = malloc(sizeof(stack_t));
    stack_init(parser->stack);
    parser->function = NULL;
}

void parser_destroy(parser_t *parser)
{
    stack_free(parser->stack); // TODO: funkci na uvolneni stacku s tabulkama
}