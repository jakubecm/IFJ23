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
symbol_table *symbol_table_init(size_t size) {
    symbol_table *table = malloc(sizeof(symbol_table));
    table->size = size;
    table->table = malloc(sizeof(symbol *) * size);
    for (size_t i = 0; i < size; i++) {
        table->table[i] = NULL;
    }
    return table;
}

void symbol_table_free(symbol_table *table) {
    for (size_t i = 0; i < table->size; i++) {
        symbol *s = table->table[i];
        while (s != NULL) {
            symbol *next = s->next;
            free(s->key);
            free(s);
            s = next;
        }
    }
    free(table->table);
    free(table);
}

// TODO: co kdyz mam funkci a promennou se stejnym nazvem? mozna staci pridat pred klic nejaky konstantni prefix?
symbol *symbol_table_lookup(symbol_table *table, htab_key_t key) {
    size_t hash = hash_function(key) % table->size;
    symbol *s = table->table[hash];
    while (s != NULL && strcmp(s->key, key) != 0) {
        s = s->next;
    }
    return s;
}

int symbol_table_insert(symbol_table *table, htab_key_t key, void *value) {
    size_t hash = hash_function(key) % table->size;
    symbol *s = table->table[hash];
    while (s != NULL && strcmp(s->key, key) != 0) {
        s = s->next;
    }
    if (s != NULL) {
        return 1; // symbol already exists
    }

    s = malloc(sizeof(symbol));
    s->key = strdup(key);
    s->value = value;
    s->next = table->table[hash];
    table->table[hash] = s;
    return 0;
}

void symbol_table_remove(symbol_table *table, htab_key_t key) {
    size_t hash = hash_function(key) % table->size;
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
    free(s->key);
    free(s);
}

void symbol_table_resize(symbol_table *table, size_t new_size) {
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
            size_t hash = hash_function(s->key) % new_size;

            // vložíme prvek do nové tabulky
            s->next = new_table[hash];
            new_table[hash] = s;

            s = next;
        }
    }

    // uvolníme starou tabulku
    free(table->table);

    // nastavíme novou velikost tabulky
    table->size = new_size;
    table->table = new_table;
}