/**
 *  token.h and shit
 *
 *
 * @authors Jakub Ráček (xracek12)
 */

#include <stdbool.h> // lmao musim importovat knihovnu abych mohl mit bool hodnoty miluju c muj oblibeny jazyk

/**
 * @brief struct representing a Token
 */
typedef struct token
{
    token_type_t type;
    token_attribute_t attribute;
} token_t;

/**
 * @brief All the possible Token types
 */
typedef enum token_type
{
    // TOKENS
    TOK_PLUS,          // '+'
    TOK_COLON,         // :
    TOK_MUL,           // *
    TOK_MINUS,         // -
    TOK_COMMA,         // ,
    TOK_ARROW,         // ->
    TOK_LESS,          // <
    TOK_LESSEQ,        // <=
    TOK_GREATER,       // >
    TOK_GREATEREQ,     // >=
    TOK_EQUAL,         // ==
    TOK_NOTEQUAL,      // !=
    TOK_ASSIGNMENT,    // =
    TOK_LCURLYBRACKET, // {
    TOK_RCURLYBRACKET, // }
    TOK_LBRACKET,      // (
    TOK_RBRACKET,      // )
    TOK_QUESTMK,       // ?
    TOK_DQUESTMK,      // ??
    TOK_NOT,           // !
    TOK_EOF,           // end of file
    TOK_UNDERSCORE,    // _
    TOK_INT,           // integer literal (eg. 2,3,4..)
    TOK_DOUBLE,        // double/float (decimal) literal (eg. 2.3, 4.5)
    TOK_EINT,          // integer literal with exponent (eg. 2e3, 4e5?)
    TOK_EDOUBLE,       // double/float with exponent (eg. 2.3e3, 4.5e5?)
    TOK_STRING,        // string literal (eg. "hello world")
    TOK_MLSTRING,      // multiline string 
    TOK_DIV,           // ÷
    TOK_IDENTIFIER,    // identifier
    TOK_BOOL,          // boolean
    TOK_UNDEF,         // undefined

    // KEYWORDS
    K_IF,      // if
    K_ELSE,    // else
    K_VAR,     // var
    K_WHILE,   // while
    K_LET,     // let
    K_NIL,     // nil
    K_FUNC,    // func
    K_DOUBLE,  // double
    K_DOUBLEQ, // double?
    K_DOUBLEE, // double!
    K_INT,     // Int
    K_INTQ,    // Int?
    K_INTE,    // Int!
    K_STRING,  // String
    K_STRINGQ, // String?
    K_STRINGE, // String!
    K_RETURN,  // return

    // ONLY FOR EXP PARSER
    TOK_DOLLAR,
    TOK_ENDMARKER,
    TOK_NTERM,

} token_type_t;

/**
 * @brief union of Token attributes
 */
typedef union token_attribute
{
    int number;
    double decimal;
    char *string;
    bool boolean;
} token_attribute_t;
