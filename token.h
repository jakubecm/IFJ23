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
    TOK_PLUS, //+
    TOK_COLON, //:
    TOK_MUL, //*
    TOK_MINUS, //-
    TOK_COMMA, //,
    TOK_ARROW, //->
    TOK_LESS, //<
    TOK_LESSEQ, //<=
    TOK_GREATER, //>
    TOK_GREATEREQ, //>=
    TOK_EQUAL, //==
    TOK_NOTEQUAL, //!=
    TOK_ASSIGMENT, //=
    TOK_LCURLYBRACKET, //{
    TOK_RCURLYBRACKET, //}
    TOK_LBRACKET, //(
    TOK_RBRACKET, //)
    TOK_QUEST, //?
    TOK_DQUEST, //??
    TOK_NOT, //!
    TOK_EOF, //end of file
    TOK_UNDERSCORE, //_
    TOK_WORD, //word
    TOK_INT, //integers
    TOK_DNUM, //decimal number
    TOK_EINT, //integers with exponent
    TOK_EDNUM, //decimal number with exponent
    TOK_EMSTRING, //empty string
    TOK_STRING, //string
    TOK_MULSTRING, //multiline string
    TOK_EMMULSTRING, //empty multiline string
    TOK_DIV, // /

    // KEYWORDS
    K_IF,
    K_ELSE,
    K_VAR,
    K_LET,
    K_NIL,
    K_FUNC,
    K_DOUBLE,
    K_DOUBLEQ,
    K_DOUBLEE,
    K_INT,
    K_INTQ,
    K_INTE,
    K_STRING,
    K_STRINGQ,
    K_STRINGE,
    K_RETURN,

    //ONLY FOR EXP PARSER
    TOK_DOLLAR,
    TOK_ENDMARKER,
    TOK_NTERM,

    //VARIABLES
    T_IDENTIFIER,
    T_INT,
    T_INTQ, // INT?
    T_INTE, // INT!
    T_DOUBLE,
    T_DOUBLEQ, // DOUBLE?
    T_DOUBLEE, // DOUBLE!
    T_STRING,
    T_STRINGQ, //STRING?
    T_STRINGE, //STRING!
    T_UNDEF

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
