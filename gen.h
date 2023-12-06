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
    mystring_t footer;
    mystring_t functions;

    int label_counter;
    int while_counter;
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
 * @param gen pointer to struct
*/
void gen_print(gen_t *gen);

/**
 * @brief Generate code for variable if statement
 * @param gen pointer to struct
 * @param name name of the variable
 * @param in_function true if in function
 * @param is_global true if global context
 * @param label label number of the if let statement
*/
void gen_if_let(gen_t *gen, char *name, bool in_function, bool is_global, int label);

/**
 * @brief Generate code for classical if statement
 * @param gen pointer to struct
 * @param in_function true if in function
 * @param label label number of the if let statement
*/
void gen_if(gen_t *gen, bool in_function, int label);

/**
 * @brief Generate code for else part of if statement
 * @param gen pointer to struct
 * @param in_function true if in function
 * @param label label number of the if statement
*/
void gen_else(gen_t *gen, bool in_function, int label);

/**
 * @brief Generate code for end of if statement
 * @param gen pointer to struct
 * @param in_function true if in function
 * @param label label number of the statement
*/
void gen_endif(gen_t *gen, bool in_function, int label);

/**
 * @brief Generate code for while loop
 * @param gen pointer to struct
 * @param in_function true if in function
 * @param label label number of the while statement
*/
void gen_while(gen_t *gen, bool in_function, int label);

/**
 * @brief Generate code for exit condition of while loop
 * @param gen pointer to struct
 * @param in_function true if in function
 * @param label label number of the while statement
*/
void gen_while_exit(gen_t *gen, bool in_function, int label);

/**
 * @brief Generate code for end of while loop
 * @param gen pointer to struct
 * @param in_function true if in function
 * @param label label number of the while statement
*/
void gen_while_end(gen_t *gen, bool in_function, int label);

/**
 * @brief Generate code for function definition
 * @param gen pointer to struct
 * @param name name of the function
*/
void gen_func(gen_t *gen, token_t *name);

/**
 * @brief Generate code for end of function definition
 * @param gen pointer to struct
 * @param in_function true if in function
*/
void gen_func_end(gen_t *gen, bool in_function);

/**
 * @brief Generate code for moving return value of function call to a variable 
 * @param gen pointer to struct
 * @param name name of the variable
 * @param in_function true if in function
 * @param is_global true if global context
*/
void gen_func_return_to_var(gen_t *gen, char *name, bool in_function, bool is_global);

/**
 * @brief Generate code for function parameters
 * @param gen pointer to struct
 * @param parameters vector of parameters
*/
void gen_parameters(gen_t *gen, vector_t *parameters);

/**
 * @brief Generate code for function call
 * @param gen pointer to struct
 * @param name name of the function
 * @param in_function true if in function
*/
void gen_func_call(gen_t *gen, char *name, bool in_function);

/**
 * @brief Generate code for arguments start of function call
 * @param gen pointer to struct
 * @param in_function true if in function
*/
void gen_arguments_start(gen_t *gen, bool in_function);

/**
 * @brief Generate code for one argument of function call
 * @param gen pointer to struct
 * @param arg argument
 * @param in_function true if in function
 * @param is_global true if global context
*/
void gen_arguments(gen_t *gen, htab_func_param_t arg, bool in_function, bool is_global);

/**
 * @brief Generate code for return
 * @param gen pointer to struct
*/
void gen_return(gen_t *gen);

/**
 * @brief Helper funcition for pushing int to the stack
 * @param gen pointer to struct
 * @param value value to push
 * @param in_function true if in function
*/
void gen_push_int(gen_t *gen, int value, bool in_function);

/**
 * @brief Helper funcition for pushing double to the stack
 * @param gen pointer to struct
 * @param value value to push
 * @param in_function true if in function
*/
void gen_push_float(gen_t *gen, double value, bool in_function);

/**
 * @brief Helper funcition for pushing string to the stack
 * @param gen pointer to struct
 * @param value value to push
 * @param in_function true if in function
*/
void gen_push_string(gen_t *gen, char *value, bool in_function);

/**
 * @brief Helper funcition for pushing nil to the stack
 * @param gen pointer to struct
 * @param in_function true if in function
*/
void gen_push_nil(gen_t *gen, bool in_function);

/**
 * @brief Helper funcition for pushing variable to the stack
 * @param gen pointer to struct
 * @param name name of the variable
 * @param local true if local variable
 * @param is_global true if global context
*/
void gen_push_var(gen_t *gen, char *name, bool local, bool is_global);

/**
 * @brief Funcition for variable definition
 * @param gen pointer to struct
 * @param token name of the variable
 * @param in_function true if in function
 * @param is_if true if in if statement
*/
void gen_var_definition(gen_t *gen, token_t* token, bool in_function, bool in_if);

/**
 * @brief Funcition for expression generation
 * @param gen pointer to struct
 * @param type type of the token
 * @param in_function true if in function
*/
void gen_expression(gen_t *gen, token_type_t type, bool in_function);

/**
 * @brief Funcition for assigment
 * @param gen pointer to struct
*/
void gen_assign(gen_t *gen);

/**
 * @brief Funcition for footer generation
 * @param gen pointer to struct
*/
void gen_footer(gen_t *gen);

/**
 * @brief Helper function for converting Int to Double
 * @param gen pointer to struct
*/
void gen_call_convert(gen_t *gen);

/**
 * @brief Helper function for converting Int to Double AND pushing it to the stack
 * @param gen pointer to struct
*/
void gen_call_exp_convert(gen_t *gen);

/**
 * @brief Helper function for converting 2 Ints to 2 Doubles
 * @param gen pointer to struct
*/
void gen_call_convert2(gen_t *gen);

/**
 * @brief Function for popping values from stack
 * @param gen pointer to struct
 * @param name name of the variable
 * @param in_function true if in function
 * @param is_global true if global context
*/
void gen_pop_value(gen_t* gen, char* name, bool in_function, bool is_global);
