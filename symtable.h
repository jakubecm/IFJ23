/**
 *  @file symtable.h
 * 
 * 
 *  @authors Jakub Ráček (xracek12)
 */

#include <stdio.h>
#include <stdlib.h>
#include "token.h"

// typedef token_type_t variable_type_t;
typedef token_attribute_t htab_attribute_t;
typedef const char * htab_key_t;

typedef enum variable_type {
    EXP_ERR  = -1,  // Error    
    VAL_BOOL,       // Bool
    VAL_DOUBLE,     // double
    VAL_DOUBLEQ,    // double?
    VAL_INT,        // Int
    VAL_INTQ,       // Int?
    VAL_STRING,     // String
    VAL_STRINGQ,    // String?
    VAL_NIL,        // Nil
    VAL_UNKNOWN,    // Type not provided yet
    VAL_VOID,       // Void
    VAL_TERM,       // Term - Used for write function
    VAL_ID          // ID
} variable_type_t;

typedef enum symbol_type {
    VAR,
    LET,
    FUNC,
    NOT_FOUND = -1
} symbol_type_t;

typedef struct htab_var {
    htab_attribute_t        value;
    variable_type_t         type;
    bool                    initialized;
    bool                    if_initialized;
    bool                    else_initialized;
    bool                    func_init;
} htab_var_t;

typedef struct htab_func_param {
    htab_var_t parameter;
    char    *call_name;        // name of the parameter in the function call
    char    *def_name;         // name of the parameter in the function definition
} htab_func_param_t;

typedef struct vector {
    htab_func_param_t *data;
    int size;
    int capacity;
} vector_t;

typedef struct htab_func {
    vector_t        *parameters;
    variable_type_t return_type;
    bool            arguments_defined;
    bool            defined;
} htab_func_t;

typedef union htab_value {
    htab_var_t      var_id;
    htab_func_t     func_id;
} htab_value_t;

typedef struct data {
    char            *name;
    htab_value_t    value;
    symbol_type_t   type;
} data_t; 

/**
 * @brief Symbol table entry
*/
typedef struct symbol {
    htab_key_t  key;
    data_t      data;
} symbol;

/**
 * @brief Symbol table
*/
typedef struct symbol_table_t {
    size_t capacity;
    size_t size;
    struct symbol **table; // symbol array
} symbol_table_t;


/**
 * @brief Hash function
 * @param str String to be hashed
*/
size_t htab_hash_function(htab_key_t str);


/**
 * @brief Creates new symbol table
 * @param size Size of the table
 * @return Pointer to new symbol table
*/
symbol_table_t * symbol_table_init(size_t size);

/**
 * @brief Destroy the symbol table
 * @param table Pointer to the symbol table
*/
void symbol_table_free(symbol_table_t *table);

/**
 * @brief Search for a symbol with the given key.
 * @param table Pointer to the symbol table
 * @param key key of the symbol
*/
data_t *symbol_table_lookup_generic(symbol_table_t *table, htab_key_t key);

/**
 * @brief Search for a function with the given key.
 * @param table Pointer to the symbol table
 * @param key key of the symbol
*/
data_t *symbol_table_lookup_func(symbol_table_t *table, htab_key_t key);

/**
 * @brief Search for a variable with the given key.
 * @param table Pointer to the symbol table
 * @param key key of the symbol
*/
data_t *symbol_table_lookup_var(symbol_table_t *table, htab_key_t key);

/**
 * @brief Inserts new symbol into the table
 * @param table Pointer to the symbol table
 * @param key key of the symbol
 * @param value Value of the symbol
*/
void symbol_table_insert(symbol_table_t *table, htab_key_t key, data_t data);

/**
 * @brief Removes symbol from the table
*/
void symbol_table_remove(symbol_table_t *table, htab_key_t key);

/**
 * @brief Resizes the symbol table
*/
symbol_table_t *symbol_table_resize(symbol_table_t *table, size_t new_size);

/**
 * @brief Creates new parameter vector
*/
vector_t *vector_init(int capacity);

/**
 * @brief Pushes new parameter to the vector
*/
void vector_push(vector_t *arr, htab_func_param_t value);

/**
 * @brief Returns top parameter from the vector
*/
htab_func_param_t *vector_top(vector_t *arr);

/**
 * @brief Destroys the vector
*/
void vector_destroy(vector_t *arr);
