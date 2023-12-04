/**
 *  @file scanner.h
 * 
 *  @author Jiri Tesar (xtesar44)
 *  @author Jakub Ráček (xracek12)
 *  @author Milan Jakubec (xjakub41)
 */

#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"
#include "str.h"

// Enum for all the states of the FSM
typedef enum scanner_state {
    // Type out all the FSM states
    START,
    COMMENT,
    MCOMMENT,
    NUM,
    DNUM,
    ENUM,
    ID,
    STRING,
    MSTRING,
} scanner_state_t;

// Scanner structure itself
typedef struct{
    scanner_state_t state; // Current state the FSM is in

} scanner_t;

/**
 * @brief Function get_next_token() reads a token from stdin and returns it to be used by parser
 * 
 * @param stuff not sure jestli je potreba parametr? je to na tobe Jirko!
 * @returns a token from stdin
*/
token_t get_next_token(void);

/**
 * @brief Function myungetc return char to stdin
 * 
 * @param backchar the char we want to return to stdin
*/
void myungetc(int backchar);

/**
 * @brief Check if backlash chars is correct.
 * 
 * @param str structur for string
*/
void backslash(mystring_t *str);

/**
 * @brief Aligns multiline string
 * 
 * @param str structur for string
*/
void spaceignor(mystring_t *str);

// Missing functions to initialize and destroy the scanner

#endif

/* end of file scanner.h */