/**
 *  @file symtable.c
 * 
 * 
 *  @authors Jakub Ráček (xracek12)
 */

#include <string.h>
#include <stdint.h>
#include "symtable.h"

// pomocné funkce

size_t htab_hash_function(htab_key_t str) {
    uint32_t h = 0; // musí mít 32 bitů
    const unsigned char *p;
    for (p = (const unsigned char *)str; *p != '\0'; p++)
        h = 65599 * h + *p;
    return h;
}

symbol_table_t *symbol_table_init(size_t size) {
    symbol_table_t *table = malloc(sizeof(symbol_table_t));
    table->size = size;
    table->table = malloc(sizeof(symbol *) * size);
    for (size_t i = 0; i < size; i++) {
        table->table[i] = NULL;
    }
    return table;
}

void symbol_table_free(symbol_table_t *table) {
    for (size_t i = 0; i < table->size; i++) {
        symbol *s = table->table[i];
        while (s != NULL) {
            symbol *next = s->next;
            free((char*)s->key);
            free(s);
            s = next;
        }
    }
    free(table->table);
    free(table);
}

int symbol_table_insert(symbol_table_t *table, htab_key_t key, data_t data) {
    size_t hash = htab_hash_function(key) % table->size;
    symbol *s = table->table[hash];
    while (s != NULL && s->data.type != data.type && strcmp(s->key, key) != 0) {
        s = s->next;
    }
    if (s != NULL) {
        return 1; // symbol already exists
    }

    if (table->size == table->capacity) {
        symbol_table_resize(table, table->capacity * 2);
    }
    s = malloc(sizeof(symbol));
    s->key = malloc(strlen(key) + 1);
    if (s->key == NULL) {
        return 2; // memory allocation failed
    }
    strcpy((char*)s->key, key);
    s->data = data;
    s->next = table->table[hash];
    table->table[hash] = s;
    return 0; // success
}

void symbol_table_remove(symbol_table_t *table, htab_key_t key) {
    size_t hash = htab_hash_function(key) % table->size;
    symbol *prev = NULL;
    symbol *s = table->table[hash];
    while (s != NULL && strcmp(s->key, key) != 0) {
        prev = s;
        s = s->next;
    }
    if (s == NULL) {
        return; // symbol does not exist
    }

    if (prev == NULL) {
        table->table[hash] = s->next;
    } else {
        prev->next = s->next;
    }
    free((char*)s->key);
    free(s);
}

void symbol_table_resize(symbol_table_t *table, size_t new_size) {
    // alokujeme novou tabulku
    symbol **new_table = malloc(sizeof(symbol *) * new_size);
    for (size_t i = 0; i < new_size; i++) {
        new_table[i] = NULL;
    }

    // přesouváme prvky ze staré tabulky do nové
    for (size_t i = 0; i < table->size; i++) {
        symbol *s = table->table[i];
        while (s != NULL) {
            symbol *next = s->next;

            // vypočítáme index nového prvku v nové tabulce
            size_t hash = htab_hash_function(s->key) % new_size;

            // vložíme prvek do nové tabulky
            s->next = new_table[hash];
            new_table[hash] = s;

            s = next;
        }
    }

    // uvolníme starou tabulku
    free(table->table);

    // nastavíme novou velikost tabulky
    table->capacity = new_size;
    table->table = new_table;
}

data_t symbol_table_lookup_var(symbol_table_t *table, htab_key_t key) {
    size_t hash = htab_hash_function(key) % table->size;
    symbol *s = table->table[hash];
    while (s != NULL && s->data.type != VAR && strcmp(s->key, key) != 0) {
        s = s->next;
    }

    if (s == NULL) {
        data_t data;
        data.type = NOT_FOUND;
        return data;
    }
    return s->data;
}

data_t symbol_table_lookup_func(symbol_table_t *table, htab_key_t key) {
    size_t hash = htab_hash_function(key) % table->size;
    symbol *s = table->table[hash];
    while (s != NULL && s->data.type != FUNC && strcmp(s->key, key) != 0) {
        s = s->next;
    }
    
    if (s == NULL) {
        data_t data;
        data.type = NOT_FOUND;
        return data;
    }
    return s->data;
}

//-----------------------DYNAMIC ARRAY-----------------------



void vector_init(vector_t *arr, int capacity) {
    arr = (vector_t *)malloc(sizeof(vector_t));
    arr->data = (htab_func_param_t *)malloc(capacity * sizeof(htab_func_param_t));
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
    return &arr->data[arr->size - 1];
}

void vector_print(vector_t *arr) {
    for (int i = 0; i < arr->size; i++) {
        
    }
    printf("\n");
}