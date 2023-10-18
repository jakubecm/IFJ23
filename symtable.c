#include "symtable.h"
#include <stdio.h> // fprintf, stderr
#include <stdlib.h> // malloc, free
#include <stdint.h> // uint32_t

size_t htab_bucket_count(const htab_t * t) {
    return t->arr_size;
}

void htab_clear(htab_t * t) {
    for (size_t i = 0; i < t->arr_size; i++)    // projdu vsechny buckety v tabulce
    { 
        htab_item_t *item = t->arr_ptr[i];        // ziskam prvni prvek v bucketu
        while (item != NULL)                    // dokud nedojdu na konec bucketu
        {
            htab_item_t *next = item->next;
            htab_erase(t, item->pair.key);      // smazu prvek a posunu se na dalsi
            item = next;
        }
    }
}

bool htab_erase(htab_t * t, htab_key_t key) {
    size_t index = htab_hash_function(key) % t->arr_size;
    htab_item_t *item = t->arr_ptr[index];
    htab_item_t *prev = NULL; // predchazejici zaznam
    while (item != NULL) {  // dokud nedojdu na konec bucketu
        if (strcmp(item->pair.key, key) == 0) // hledam shodu zaznamu a hledaneho klice
        {
            if (prev == NULL) {
                t->arr_ptr[index] = item->next; // pokud jsem vymazal prvni zaznam, tak ho nahradim dalsim
            } else {
                prev->next = item->next; // jinak vymazu zaznam a propojim predchozi a nasledujici
            }

            // uvolnim pamet
            free((char *)item->pair.key);
            free(item);
            t->size--;
            return true;
        }

        // jestli jsem nenalezl hledany zaznam, tak posunu ukazatele
        prev = item;
        item = item->next;
    }
    return false; // neuspel
}

htab_pair_t * htab_find(const htab_t * t, htab_key_t key) {
    htab_item_t *item = t->arr_ptr[htab_hash_function(key) % t->arr_size]; // najdu prvni zaznam v bucketu
    
    while (item != NULL) // dokud nedojdu na konec bucketu
    {
        if (!strcmp(item->pair.key, key)) { // hledam shodu zaznamu a hledaneho klice
            return &(item->pair);
        }
        item = item->next; // posunu ukazatel na dalsi zaznam
    }
    return NULL;
}

void htab_for_each(const htab_t * t, void (*f)(htab_pair_t *data)) {
    for (size_t i = 0; i < t->arr_size; i++)    // projdu vsechny buckety v tabulce
    { 
        htab_item_t *item = t->arr_ptr[i];        // ziskam prvni prvek v bucketu
        while (item != NULL)                    // dokud nedojdu na konec bucketu
        {
            f(&item->pair);                     // zavolam funkci f na kazdy prvek
            item = item->next;
        }
    }
}

void htab_free(htab_t * t) {
    htab_clear(t);
    free(t->arr_ptr);
    free(t);
}

size_t htab_hash_function(const char *str) {
    uint32_t h = 0; // musí mít 32 bitů
    const unsigned char *p;
    for (p = (const unsigned char *)str; *p != '\0'; p++)
        h = 65599 * h + *p;
    return h;
}

htab_t *htab_init(const size_t n) {
    htab_t *table = malloc(sizeof(htab_t));
    if (table == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(1);
    }
    table->size = 0;
    table->arr_size = n;
    table->arr_ptr = malloc(n * sizeof(struct htab_item_t *));
    if (table->arr_ptr == NULL) {
        free(table);
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(1);
    }
    for (size_t i = 0; i < n; i++) {
        table->arr_ptr[i] = NULL;
    }
    return table;
}

htab_pair_t * htab_lookup_add(htab_t *t, htab_key_t key) {
    htab_pair_t *pair;
    // pokud je položka nalezena, vrátí se ukazatel na ni
    if ((pair = htab_find(t, key)) != NULL) {
        return pair;
    }

    // vytvoření nové položky
    htab_item_t *item;
    item = malloc(sizeof(struct htab_item_t_t)); // alokace místa pro položku
    if (item == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(1);
    }

    item->pair.key = malloc(strlen(key) * sizeof(char) + 1); // alokace místa pro klíč
    if (item->pair.key == NULL) {
        free(item);
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(1);
    }
    strcpy((char *)item->pair.key, key); // zkopírování klíče
    item->pair.value = 0;
    item->next = NULL;

    size_t index = htab_hash_function(key) % t->arr_size; // výpočet indexu

    // napojení nové položky do seznamu
    htab_item_t *current = t->arr_ptr[index];
    if (current == NULL) {
        t->arr_ptr[index] = item; // pokud je bucket na indexu prázdný, položka se napojuje na začátek
    } else {
        while (current->next != NULL) {
            current = current->next; // hledam poslední prvek bucketu
        }
        current->next = item; // napojení na konec bucketu
    }

    t->size++; // zvýšení počtu položek v tabulce

    return &item->pair;
}

size_t htab_size(const htab_t * t) {
    return t->size;
}

void htab_statistics(const htab_t * t) {
    size_t min_bucket = 0;
    size_t max_bucket = 0;
    size_t avg_bucket = 0;

    for (size_t i = 0; i < t->arr_size; i++) {
        size_t count = 0;                       // pro kazdy bucket pocitam prvky
        htab_item_t *item = t->arr_ptr[i];
        while (item != NULL) {
            count++;
            item = item->next;                  // projdu vsechny prvky v bucketu
        }
        if (count > max_bucket) {
            max_bucket = count;                 // ulozim nove maximum
        }
        if (count < min_bucket) {
            min_bucket = count;                 // ulozim nove minimum  
        }
    }
    if (t->arr_size != 0)
        avg_bucket =  t->size / t->arr_size;    // vypocitam prumer
    else 
        avg_bucket = 0;

    fprintf(stderr, "Max: %lu\nMin: %lu\nAvg: %lu\n",max_bucket, min_bucket, avg_bucket); // vypisu vysledky
}