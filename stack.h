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
#include "token.h"

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
 *  @brief Struct to represent nodes in a tree. Each has a token, type of it,
 *         pointer to children if they exist.
 */

typedef struct stack_terminal {
    sem_data_type_t data;
    token_type_t type;
    struct stack_terminal* right;
} stack_terminal_t;

typedef struct analysis {
    stack_terminal_t* tok1;
    stack_terminal_t* tok2;
    stack_terminal_t* tok3;
    sem_data_type_t end_type;
} analysis_t;

typedef struct Node {
    void *data;
    struct Node *right;
} Node;

typedef struct {
    Node *top;
} Stack;

/**
 *   @brief Initialize new stack
 *   @param stack - Stack that is being created
 *   @return New stack
 */ 
void stack_init(Stack *stack);

/**
 *  @brief Pushes token to the stack and sets its attributes
 *  @param stack - Stack to be pushed on 
 *  @param token_type - Token to be pushed (type of it, as: +, -...)
 *  @param data_type - Type of data for the token (as: INT, STRING, UNDEFINED...)
 */ 
void stack_push_token(Stack* stack, sem_data_type_t data_type, token_type_t token_type);

/**
 *  @brief Pushes token after the top terminal
 *  @param stack - Stack to be pushed on
 *  @param token - Token to be pushed after terminal
 */ 
bool stack_push_after(Stack* stack, sem_data_type_t data_type, token_type_t token_type);

/**
 *  @brief Completely frees the stack and resets values like before init
 *  @param stack - Stack that will be freed
 */
void stack_free(Stack* stack);

/**
 *  @brief Returns top symbol (token) in the stack
 *  @param stack - Stack to be returned from
 *  @return Pointer to the token on the top of the stack
 */
stack_terminal_t* stack_top_token(Stack* stack);

/**
 *  @brief Returns top terminal
 *  @param stack - Stack to be returned from
 *  @return Pointer to the top terminal in the stack
 */ 
stack_terminal_t* stack_top_terminal(Stack* stack);

/**
 *  @brief Pops top symbol from stack
 *  @param stack - Stack to be popped from
 *  @return True if pop was successfull, else false
 */
stack_terminal_t* stack_pop_token(Stack* stack);

void print_stack_contents(Stack *stack);

#endif // STACK_H