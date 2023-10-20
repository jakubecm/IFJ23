/**
 *  @file stack.h
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 */

#include <stdbool.h>
#include "token.h"
#include "exp_semantic.h"

/**
 *  @brief Struct to represent nodes in a tree. Each has a token, type of it,
 *         pointer to children if they exist.
 */
typedef struct stack_terminal {
    variable_type_t data;
    token_type_t type;
    bool term;
    stack_terminal_t* right;
} stack_terminal_t;

/**
 *   @brief Struct to represent a stack 
 */ 
typedef struct Stack {
    stack_terminal_t *top; // Pointer to the top terminal token in the stack
} stack_t;

/**
 *   @brief Initialize new stack
 *   @param stack - Stack that is being created
 *   @return New stack
 */ 
void stack_init(stack_t *stack);

/**
 *  @brief Pushes token to the stack and sets its attributes
 *  @param stack - Stack to be pushed on 
 *  @param token_type - Token to be pushed (type of it, as: +, -...)
 *  @param data_type - Type of data for the token (as: INT, STRING, UNDEFINED...)
 */ 
void stack_push(stack_t* stack, sem_data_type_t data_type, token_type_t token_type);

/**
 *  @brief Pushes token after the top terminal
 *  @param stack - Stack to be pushed on
 *  @param token - Token to be pushed after terminal
 */ 
void stack_push_after(stack_t* stack, stack_terminal_t* token);

/**
 *  @brief Completely frees the stack and resets values like before init
 *  @param stack - Stack that will be freed
 */
void stack_free(stack_t* stack);

/**
 *  @brief Returns top symbol (token) in the stack
 *  @param stack - Stack to be returned from
 *  @return Pointer to the token on the top of the stack
 */
stack_terminal_t* stack_top(stack_t* stack);

/**
 *  @brief Returns top terminal
 *  @param stack - Stack to be returned from
 *  @return Pointer to the top terminal in the stack
 */ 
stack_terminal_t* stack_top_terminal(stack_t* stack);

/**
 *  @brief Pops top symbol from stack
 *  @param stack - Stack to be popped from
 *  @return True if pop was successfull, else false
 */
bool stack_pop(stack_t* stack);

/**
 *  @brief Pops stack multiple times
 *  @param stack - Stack to be popped from
 *  @param number - Number indicating how many pops are going to happen
 */
void stack_pop_more(stack_t* stack, int number);

/**
 *  @brief Initializes new stack terminal with it's information
 *  @param symbol - Symbol to be used
 *  @param term - Indicates for stack if symbol is terminal or not
 *  @return New term/symbol for stack
 */ 
stack_terminal_t* terminal_init(token_t symbol, bool term);

/**
 *  @brief Frees stack_terminal_t
 *  @param terminal - Highest terminal in the tree
 */
stack_terminal_t* terminal_free(stack_terminal_t* terminal);

