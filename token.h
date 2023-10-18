/**
 *  token.h and shit
 * 
 * 
 * @authors Jakub Ráček (xracek12)
 */

#include <stdbool.h>    // lmao musim importovat knihovnu abych mohl mit bool hodnoty miluju c muj oblibeny jazyk

/**
 * @brief struct representing a Token
*/
typedef struct token {
    token_type_t  type;
    token_attribute_t attribute;
} token_t;

/**
 * @brief All the possible Token types
*/
typedef enum token_type {
    // zde budou vsechny typy kterych muze token dosahovat
} token_type_t;

/**
 * @brief union of Token attributes
*/
typedef union token_attribute {
    int number;
    double decimal;
    char* string;
    bool boolean;
} token_attribute_t;