/**
 *  @file stack.h
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 *  @authors Jakub Ráček (xracek12)
 */

#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include "symtable.h"

/**
 *  @brief All the possible semantic data types
 */
typedef enum sem_data_type {
    SEM_OPERATOR,
    SEM_INT,
    SEM_FLOAT,
    SEM_STRING,
    SEM_BOOL,
    SEM_NIL,
    SEM_UNDEF
} sem_data_type_t;

/**
 *  @brief Struct for tokens in the stack
 */
typedef struct stack_terminal {
    sem_data_type_t data;
    token_type_t type;
    struct stack_terminal* right;
} stack_terminal_t;

/**
 *  @brief Struct for semantic analysis of expressions
 */
typedef struct analysis {
    stack_terminal_t* tok1;
    stack_terminal_t* tok2;
    stack_terminal_t* tok3;
    sem_data_type_t end_type;
} analysis_t;

/**
 *  @brief Struct to represent nodes in a void stack
 */
typedef struct Node {
    void *data;
    struct Node *right;
} Node;

/**
 *  @brief Struct to represent a void stack
 */
typedef struct Stack {
    Node *top;
} stack_t;

void analysis_init(analysis_t *data);
void analysis_free(analysis_t *data);
/**
 *   @brief Initialize new stack
 *   @param stack - stack_t that is being created
 *   @return New stack
 */ 
void stack_init(stack_t *stack);

/**
 *  @brief Pushes token to the stack and sets its attributes
 *  @param stack - stack_t to be pushed on 
 *  @param token_type - Token to be pushed (type of it, as: +, -...)
 *  @param data_type - Type of data for the token (as: INT, STRING, UNDEFINED...)
 */ 
void stack_push_token(stack_t* stack, sem_data_type_t data_type, token_type_t token_type);

/**
 *  @brief Pushes token after the top terminal
 *  @param stack - stack_t to be pushed on
 *  @param token - Token to be pushed after terminal
 */ 
bool stack_push_after(stack_t* stack, sem_data_type_t data_type, token_type_t token_type);

/**
 *  @brief Counts the amount of tokens after endmarker on the stack
 *  @param stack - stack we are working with
 *  @param analysis - Tokens to work with
 */
int stack_count_after(stack_t* stack, analysis_t* analysis);

/**
 *  @brief Completely frees the stack and resets values like before init
 *  @param stack - stack_t that will be freed
 */
void stack_free(stack_t* stack);

/**
 *  @brief Returns top symbol (token) in the stack
 *  @param stack - stack_t to be returned from
 *  @return Pointer to the token on the top of the stack
 */
stack_terminal_t* stack_top_token(stack_t* stack);

/**
 *  @brief Returns top terminal
 *  @param stack - stack_t to be returned from
 *  @return Pointer to the top terminal in the stack
 */ 
stack_terminal_t* stack_top_terminal(stack_t* stack);

/**
 *  @brief Pops top symbol from stack
 *  @param stack - stack_t to be popped from
 *  @return True if pop was successfull, else false
 */
stack_terminal_t* stack_pop_token(stack_t* stack);

/**
 *  @brief Frees tokens and the nodes on stack
 *  @param stack - stack_t to be freed
 */
void stack_free_token(stack_t* stack);

/**
 *  @brief Prints the stack for debugging purposes
 *  @param stack - stack to be printed
 */
void print_stack_contents(stack_t *stack);

/**
 * @brief Pushes symtable to the stack
*/
void stack_push_table(stack_t* stack);

/**
 * @brief Pops symtable from the stack
*/
void stack_pop_table(stack_t* stack);

/**
 * @brief Returns top symtable from the stack
*/
symbol_table_t* stack_top_table(stack_t* stack);

/**
 * @brief Returns bottom symtable from the stack (global table)
*/
symbol_table_t* stack_bottom_table(stack_t* stack);

/**
 * @brief Returns true if the top symtable is the global table
*/
bool is_table_global(stack_t* stack);

/**
 * @brief Looks through the symtable stack to find a variable with the given key
*/
data_t *stack_lookup_var(stack_t* stack, htab_key_t key);

/**
 * @brief Looks through the symtable stack to find a function with the given key
*/
data_t *stack_lookup_func(stack_t* stack, htab_key_t key);

/**
 * @brief Destroy the symtable stack
*/
void stack_destroy_table(stack_t* stack);

/**
 * @brief Looks through the symtable stack to find if a variable with the given key
 *        exists in the global or local scope
*/
bool stack_lookup_var_in_global(stack_t* stack, htab_key_t key);

/**
 * @brief Returns the current nested level of the stack
*/
int stack_get_nested_level(stack_t* stack);

int stack_get_nested_level_of_var(stack_t* stack, htab_key_t key);

int stack_height(stack_t* stack);

#endif // STACK_H
