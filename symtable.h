/**
 *  @file symtable.h
 * 
 * 
 *  @authors Jakub Ráček (xracek12)
 */

#include <stdio.h>
#include <stdlib.h>
#include "token.h"

typedef token_type_t variable_type_t;
typedef token_attribute_t htab_attribute_t;
typedef const char * htab_key_t;

typedef enum symbol_type {
    VAR,
    FUNC,
    NOT_FOUND = -1
} symbol_type_t;

typedef struct htab_var {
    htab_attribute_t        value;
    variable_type_t         type;
} htab_var_t;

typedef struct htab_func {
    htab_var_t *parameters;
    htab_var_t *return_values;
} htab_func_t;

typedef union htab_value {
    htab_var_t var_id;
    htab_func_t func_id;
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
    htab_key_t key;
    data_t data;
    struct symbol *next;
} symbol;

/**
 * @brief Symbol table
*/
typedef struct symbol_table_t {
    size_t size;
    symbol **table;
} symbol_table_t;


/**
 * @brief Creates new symbol table
 * @return Pointer to new symbol table
*/
size_t htab_hash_function(htab_key_t str);


/**
 * @brief Creates new symbol table
 * @param size Size of the table
 * @return Pointer to new symbol table
*/
symbol_table_t *symbol_table_init(size_t size);

/**
 * @brief Inserts new symbol into the table
 * @param table Pointer to the symbol table
*/
void symbol_table_free(symbol_table_t *table);

/**
 * @brief Inserts new symbol into the table
 * @param table Pointer to the symbol table
 * @param key key of the symbol
*/
symbol *symbol_table_lookup(symbol_table_t *table, htab_key_t key);

/**
 * @brief Search for a function with the given key.
 * Check for type property of returned struct to determine if the function was found
 * @param table Pointer to the symbol table
 * @param key key of the symbol
*/
data_t symbol_table_lookup_func(symbol_table_t *table, htab_key_t key);

/**
 * @brief Search for a variable with the given key.
 * Check for type property of returned struct to determine if the variable was found
 * @param table Pointer to the symbol table
 * @param key key of the symbol
*/
data_t symbol_table_lookup_var(symbol_table_t *table, htab_key_t key);

/**
 * @brief Inserts new symbol into the table
 * @param table Pointer to the symbol table
 * @param key key of the symbol
 * @param value Value of the symbol
*/
int symbol_table_insert(symbol_table_t *table, htab_key_t key, data_t data);

/**
 * @brief Removes symbol from the table
*/
void symbol_table_remove(symbol_table_t *table, htab_key_t key);

/**
 * @brief Resizes the symbol table
*/
void symbol_table_resize(symbol_table_t *table, size_t new_size);
