/**
 *  @file gen.h
 *
 *
 *  @authors Milan Jakubec (xjakub41)
 *  @authors Jakub Ráček (xracek12)
 *  @authors Norman Babiak (xbabia01)
 */

#include "str.h"
#include "token.h"
#include "stack.h"

typedef struct gen{
    mystring_t header;
    mystring_t global;
    mystring_t temp;
    mystring_t local;
    mystring_t functions;

    int label_counter;
    int arg_counter;
} gen_t;

/**
 * @brief Initialize the generator
 *
 * @param gen pointer to struct
 */
void gen_init(gen_t *gen);

/**
 * @brief Destroy the generator
 *
 * @param gen pointer to struct
 */
void gen_free(gen_t *gen);

/**
 * @brief Generate header of the code
 *
 * @param gen pointer to struct
 */
void gen_header(gen_t *gen);

/**
 * @brief Print generated code
*/
void gen_print(gen_t *gen);

void gen_if(gen_t *gen, bool in_function);

void gen_else(gen_t *gen, bool in_function);

void gen_endif(gen_t *gen, bool in_function);

void gen_while(gen_t *gen, bool in_function);

void gen_while_exit(gen_t *gen, bool in_function);

void gen_while_end(gen_t *gen, bool in_function);

void gen_func(gen_t *gen, token_t *name);

void gen_func_end(gen_t *gen);

void gen_parameters(gen_t *gen, vector_t *parameters);

void gen_func_call(gen_t *gen, char *name);

void gen_return(gen_t *gen);

void gen_push_int(gen_t *gen, int value, bool in_function);

void gen_push_float(gen_t *gen, double value, bool in_function);

void gen_push_string(gen_t *gen, char *value, bool in_function);

void gen_push_nil(gen_t *gen, bool in_function);

void gen_push_var(gen_t *gen, char *name, bool local);

void gen_var_definition(gen_t *gen, token_t* token, bool in_function);

void gen_expression(gen_t *gen, token_type_t type, bool in_function);

void gen_assign(gen_t *gen);

void gen_footer(gen_t *gen);

void gen_call_convert(gen_t *gen);

void gen_call_convert2(gen_t *gen);

void gen_pop_value(gen_t* gen, char* name, bool in_function);
