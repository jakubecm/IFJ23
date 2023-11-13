/**
 *  @file error.c
 *
 *
 *  @authors Milan Jakubec (xjakub41)
 */

#include "error.h"
#include <stdlib.h>
#include <stdio.h>

const char *printed_error_messages[] = {
    [ERR_OK] = "",
    [ERR_LEX] = "Lexical analysis error",
    [ERR_SYN] = "Syntax analysis error",
    [ERR_SEM_FUNCTION] = "Semantic error - undefined function, redefinition of a variable,...",
    [ERR_SEM_CALL] = "Semantic error - bad number of arguments, bad type of arguments,...",
    [ERR_SEM_NDEF] = "Semantic error - undefined variable",
    [ERR_SEM_RETURN] = "Semantic error - bad return value (missing expression or should be void)",
    [ERR_SEM_TYPE] = "Semantic error - expression type mismatch",
    [ERR_SEM_INCOMPATIBLE] = "Semantic error - incompatible types in expression",
    [ERR_SEM_OTHER] = "Semantic error - other semantic errors",
    [ERR_INTERNAL] = "Internal parse error"};

void print_error_and_exit(error_t error)
{
    fprintf(stderr, "%s\n", printed_error_messages[error]);
    exit(error);
}