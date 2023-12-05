/**
 *  @file main.c
 * 
 *  @author
 *  @author Norman Babiak (xbabia01)
 *  @author Jakub Ráček (xracek12)
 *  @author
 */

// BASIC MAIN WITH CORRECT RETURN ON THE END OF PROGRAM
#include "parser.h"

int main() {
    gen_t gen;
    parser_t parser;
    parser_init(&parser, &gen);
    run_parser(&parser);
    parser_destroy(&parser);
    gen_print(&gen);
    gen_free(&gen);

    return 0;
}