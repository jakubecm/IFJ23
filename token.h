/**
 *  token.h 
 *
 * @authors Jiri Tesar (xtesar44)
 * @authors Jakub Ráček (xracek12)
 */

#include <stdbool.h> 

#ifndef TOKEN_H
#define TOKEN_H

/**
 * @brief All the possible Token types
 */
typedef enum token_type
{
    // TOKENS
    TOK_PLUS,          // '+'
    TOK_MINUS,         // -
    TOK_MUL,           // *
    TOK_DIV,           // ÷
    TOK_EQUAL,         // ==
    TOK_NOTEQUAL,      // !=
    TOK_LESS,          // <
    TOK_GREATER,       // >
    TOK_LESSEQ,        // <=
    TOK_GREATEREQ,     // >=
    TOK_DQUESTMK,      // ??
    TOK_NOT,           // !
    TOK_LBRACKET,      // (
    TOK_RBRACKET,      // )
    TOK_IDENTIFIER,    // identifier
    TOK_INT,           // integer literal (eg. 2,3,4..)
    TOK_DOUBLE,        // double/float (decimal) literal (eg. 2.3, 4.5)
    TOK_STRING,        // string literal (eg. "hello world")
    K_NIL,             // nil (keyword, here due to precedence table)
    TOK_DOLLAR,        // $
    TOK_MLSTRING,      // multiline string 
    TOK_ENDMARKER,     // symbol used for precedence analysis (dont move it from here)
    TOK_COLON,         // :
    TOK_COMMA,         // ,
    TOK_ARROW,         // ->
    TOK_ASSIGNMENT,    // =
    TOK_LCURLYBRACKET, // {
    TOK_RCURLYBRACKET, // }
    TOK_QUESTMK,       // ?
    TOK_EOF,           // end of file
    TOK_UNDERSCORE,    // _
    TOK_BOOL,          // boolean
    TOK_UNDEF,         // undefined

    // KEYWORDS
    K_IF,      // if
    K_ELSE,    // else
    K_VAR,     // var
    K_WHILE,   // while
    K_LET,     // let
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

/**
 * @brief struct representing a Token
 */
typedef struct token
{
    token_type_t type;
    token_attribute_t attribute;
    bool eol;
} token_t;

/**
 *   @brief Allocate memory for token.
 * 
 *   @param token structer for token
 *   @param lenght lenght for allocate memory
 */ 
void tokinit(token_t *token, int lenght);

/**
 *   @brief Allocate destory token
 * 
 *   @param token structer for token
 */ 
void tokdestroy(token_t *token);

/**
 *   @brief Check if token in keyword.
 * 
 *   @param token structer for token
 */ 
bool iskeyw(token_t *token);

bool is_literal(token_type_t token);

bool is_bracket(token_type_t left, token_type_t middle, token_type_t right);

#endif
