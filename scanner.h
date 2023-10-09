/**
 *  scanner.h and shit
 * 
 * 
 * @authors Jakub Ráček (xracek12)
 */

#include "token.h"

/**
 * @brief Function get_next_token() reads a token from stdin and returns it to be used by parser
 * 
 * @param stuff not sure jestli je potreba parametr? je to na tobe Jirko!
 * @returns a token from stdin
*/
token_t* get_next_token(stuff idk);

typedef enum scanner_state {
    // all the FSM states
} scanner_state_t;