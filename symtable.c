/**
 *  @file symtable.c
 * 
 * 
 *  @authors Jakub Ráček (xracek12)
 */

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "symtable.h"
#include "error.h"

extern error_t error;

//-----------------------HASHTABLE WITH OPEN ADDRESSING-----------------------

size_t htab_hash_function(htab_key_t str) {
    size_t h = 0; // musí mít 32 bitů
    const unsigned char *p;
    for (p = (const unsigned char *)str; *p != '\0'; p++)
        h = (65599 * h + *p ) % 7193;
    return h;
}

symbol_table_t * symbol_table_init(size_t size) {
    symbol_table_t *table = malloc(sizeof(symbol_table_t));
    if (table == NULL) {
        return NULL;
    }
    table->size = 0;
    table->capacity = size;
    table->table = malloc(sizeof(struct symbol *) * size);
    if (table->table == NULL) {
        free(table);
        return NULL;
    }
    for (size_t i = 0; i < size; i++) {
        table->table[i] = NULL;
    }
    return table;
}

void symbol_table_free(symbol_table_t *table) {
    for (size_t i = 0; i < table->capacity; i++) {
        if (table->table[i] != NULL) {
            free((char *)table->table[i]->key);
            table->table[i]->key = NULL;
            free(table->table[i]);
            table->table[i] = NULL;

        }
    }
    free(table->table);
    table->table = NULL;
    free(table);
    table = NULL;
}

void symbol_table_insert(symbol_table_t *table, htab_key_t key, data_t data) {
    size_t hash = htab_hash_function(key) % table->capacity;
    while (table->table[hash] != NULL) {
        if (strcmp(table->table[hash]->key, key) == 0 && table->table[hash]->data.type == data.type) {
            // symbol already exists
            // update the data of the symbol
            table->table[hash]->data = data;
            return;
        }
        hash = (hash + 1) % table->capacity;
    }

    // symbol does not exist
    // create new symbol
    symbol *new_symbol = malloc(sizeof(symbol));
    if (new_symbol == NULL) {
        return;
    }
    new_symbol->key = malloc(sizeof(char) * (strlen(key) + 1));
    if (new_symbol->key == NULL) {
        free(new_symbol);
        return;
    }
    strcpy((char *)new_symbol->key, (char *)key);
    new_symbol->data = data;
    table->table[hash] = new_symbol;

    table->size++;
    if (table->size == table->capacity) {
        table = symbol_table_resize(table, table->capacity * 2);
    }
}

void symbol_table_remove_generic(symbol_table_t *table, htab_key_t key) {
    size_t hash = htab_hash_function(key) % table->capacity;
    while (table->table[hash] != NULL && strcmp(table->table[hash]->key, key) != 0) {
        hash = (hash + 1) % table->capacity;
    }

    if (table->table[hash] == NULL) {
        return;
    }

    free(table->table[hash]);
}

symbol_table_t *symbol_table_resize(symbol_table_t *table, size_t new_size) {
    symbol **new_table = malloc(new_size * sizeof(symbol *));
    if (new_table == NULL) {
        error = ERR_INTERNAL;
        print_error_and_exit(error);
        return NULL;
    }

    // initialize new table
    for (size_t i = 0; i < new_size; i++) {
        new_table[i] = NULL;
    }

    // rehash all symbols
    for (size_t i = 0; i < table->capacity; i++) {
        if (table->table[i] != NULL) {
            size_t new_hash = htab_hash_function(table->table[i]->key) % new_size;
            while (new_table[new_hash] != NULL) {
                new_hash = (new_hash + 1) % new_size;
            }
            new_table[new_hash] = table->table[i];
        }
    }

    free(table->table);
    table->table = new_table;
    table->capacity = new_size;
}

data_t *symbol_table_lookup_generic(symbol_table_t *table, htab_key_t key) {
    size_t hash = htab_hash_function(key) % table->capacity;
    while (table->table[hash] != NULL && strcmp(table->table[hash]->key, key) != 0) {
        hash = (hash + 1) % table->capacity;
    }

    if (table->table[hash] == NULL) {
        return NULL;
    }

    return &(table->table[hash]->data);
}

data_t *symbol_table_lookup_var(symbol_table_t *table, htab_key_t key) {
    size_t hash = htab_hash_function(key) % table->capacity;
    while (table->table[hash] != NULL && (!(table->table[hash]->data.type == VAR ||
     table->table[hash]->data.type == LET) || strcmp(table->table[hash]->key, key) != 0)) {
        hash = (hash + 1) % table->capacity;
    }

    if (table->table[hash] == NULL) {
        return NULL;
    }

    return &(table->table[hash]->data);
}

data_t *symbol_table_lookup_func(symbol_table_t *table, htab_key_t key) {
    size_t hash = htab_hash_function(key) % table->capacity;
    while (table->table[hash] != NULL && (table->table[hash]->data.type != FUNC || strcmp(table->table[hash]->key, key) != 0)) {
        hash = (hash + 1) % table->capacity;
    }

    if (table->table[hash] == NULL) {
        return NULL;
    }

    return &(table->table[hash]->data);
}

//-----------------------DYNAMIC ARRAY-----------------------

vector_t *vector_init(int capacity) {
    vector_t *arr = (vector_t *)malloc(sizeof(vector_t));
    if (!arr) {
        return NULL;
    }

    arr->data = (htab_func_param_t *)malloc((capacity + 1) * sizeof(htab_func_param_t));
    if (!arr->data) {
        free(arr);
        return NULL;
    }

    arr->size = 0;
    arr->capacity = capacity;

    return arr;
}


void vector_push(vector_t *arr, htab_func_param_t value) {
    if (arr->size == arr->capacity) {
        arr->capacity *= 2;
        arr->data = (htab_func_param_t *)realloc(arr->data, arr->capacity * sizeof(htab_func_param_t));
    }
    arr->data[arr->size++] = value;
}

void vector_destroy(vector_t *arr) {
    free(arr->data);
    arr->data = NULL;
    arr->size = arr->capacity = 0;
}

htab_func_param_t *vector_top(vector_t *arr) {
    return &(arr->data[arr->size - 1]);
}
