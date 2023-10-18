#include <string.h>     // size_t
#include <stdbool.h>    // bool
#include "token.h"

typedef token_type_t variable_type_t;
typedef const char * htab_key_t;

typedef struct htab_item {
    struct htab_pair pair;
    struct htab_item_t *next;
} htab_item_t;


typedef struct htab {
    size_t              size;
    size_t              arr_size;
    struct htab_item_t  **arr_ptr;
} htab_t;

typedef struct htab_entry {
    char * name;
    htab_value_t value;
} htab_entry_t;

typedef union htab_value {
    htab_var_t var_id;
    htab_func_t func_id;
} htab_value_t;

typedef struct htab_func {
    htab_var_t *parameters;
    htab_var_t *return_values;
} htab_func_t;

typedef struct htab_var {
    htab_value_t      value;
    variable_type_t type;
} htab_var_t;

// Dvojice dat v tabulce:
typedef struct htab_pair {
    htab_key_t    key;          // klíč
    htab_entry_t  entry;        // asociovaná hodnota
} htab_pair_t;                 

// Rozptylovací (hash) funkce (stejná pro všechny tabulky v programu)
// Pokud si v programu definujete stejnou funkci, použije se ta vaše.
size_t htab_hash_function(htab_key_t str);

// Funkce pro práci s tabulkou:
htab_t *htab_init(const size_t n);              // konstruktor tabulky
size_t htab_size(const htab_t * t);             // počet záznamů v tabulce
size_t htab_bucket_count(const htab_t * t);     // velikost pole

htab_pair_t * htab_find(const htab_t * t, htab_key_t key);  // hledání
htab_pair_t * htab_lookup_add(htab_t * t, htab_key_t key);

bool htab_erase(htab_t * t, htab_key_t key);    // ruší zadaný záznam

// for_each: projde všechny záznamy a zavolá na ně funkci f
// Pozor: f nesmí měnit klíč .key ani přidávat/rušit položky
void htab_for_each(const htab_t * t, void (*f)(htab_pair_t *data));

void htab_clear(htab_t * t);    // ruší všechny záznamy
void htab_free(htab_t * t);     // destruktor tabulky

// výpočet a tisk statistik délky seznamů (min,max,avg) do stderr:
void htab_statistics(const htab_t * t);
