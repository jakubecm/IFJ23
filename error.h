/**
 *  @file error.h
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 */

/* Usage: Put "error_t error" on the beginning of your file and assign error type when it happens
   > error = ERR_LEX;
*/

#include <stdint.h>

typedef enum Error {
    ERR_OK               = 0,
    ERR_LEX              = 1,
    ERR_SYN              = 2,
    ERR_SEM_FUNCTION     = 3,
    ERR_SEM_CALL         = 4,
    ERR_SEM_NDEF         = 5,
    ERR_SEM_RETURN       = 6,
    ERR_SEM_TYPE         = 7,
    ERR_SEM_INCOMPATIBLE = 8,
    ERR_SEM_OTHER        = 9,
    ERR_INTERNAL         = 99

} error_t;

/**
 * @brief For calling in other files
 */
extern error_t error;

/**
 * @brief Prints error message to stderr
 * 
 * @param error Error type
 */
void print_error_and_exit(error_t error);