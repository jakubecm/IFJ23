/**
 *  @file scanner.h
 * 
 * 
 *  @author Jakub Ráček (xracek12)
 *  @author Milan Jakubec (xjakub41)
 */

#include "token.h"

// Enum for all the states of the FSM
typedef enum scanner_state {
    // Type out all the FSM states
    START,
} scanner_state_t;

// Scanner structure itself
typedef struct{
    enum fsm_curr_state state; // Current state the FSM is in

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
int myungetc(int backchar);

// Missing functions to initialize and destroy the scanner
