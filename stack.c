/**
 *  @file stack.c
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 *  @authors Jakub Ráček (xracek12)
 */

#include <stdio.h>
#include "error.h"
#include "stack.h"

// obecne funkce
void stack_init(Stack *stack) {
    stack->top = NULL;
}

void push(Stack *s, void *data) {
    Node *newNode = malloc(sizeof(Node));
    newNode->data = data;
    newNode->right = s->top;
    s->top = newNode;
}

bool stack_pop(Stack *s) {
    if (s->top == NULL) {
        return false;
    }

    Node *topNode = s->top;
    s->top = topNode->right;
    void *data = topNode->data;
    free(topNode);
    return true;
}

void *stack_top(Stack *s) {
    return s->top;
}
// konec obecnych funkci

error_t err;

void stack_free_token(Stack* stack) {
    while(stack_pop_token(stack));
}

void stack_push_token(Stack* stack, sem_data_type_t data_type, token_type_t token_type) {
    stack_terminal_t* new_token = (stack_terminal_t*) malloc(sizeof(stack_terminal_t));
    if(new_token == NULL) {
        err = ERR_INTERNAL;
        return;
    }

    new_token->data = data_type;
    new_token->type = token_type;

    push(stack, new_token);
}

stack_terminal_t* stack_top_token(Stack* stack) {
    return (stack_top(stack) != NULL) ? stack->top->data : NULL;
}

stack_terminal_t* stack_top_terminal(Stack* stack) {
    Node* currentNode = stack->top;

    while (currentNode != NULL) {
        stack_terminal_t* token = (stack_terminal_t*)currentNode->data;

        if (token != NULL && token->type < TOK_ENDMARKER) {
            return token;
        }

        currentNode = currentNode->right;
    }

    return NULL;
}


bool stack_pop_token(Stack* stack) {
    if(stack->top == NULL)
        return false;

    stack_terminal_t* token = stack->top->data;
    free(token);
    pop(stack);
    return true;
}

void stack_pop_more(Stack* stack, int number) {
    for(int i = 0; i < number; i++) {
        stack_pop_token(stack);
    }
}

//NEED FIXING
/*stack_terminal_t* terminal_init(token_t symbol, bool term) {
    stack_terminal_t *new_term = malloc(sizeof(stack_terminal_t));
    if(new_term == NULL) {
        err = ERR_INTERNAL;
        return;
    }

    new_term->right = NULL;
    new_term->type = symbol.type;

    new_term->token = symbol;
    new_term->term = term;

    return new_term;
}

stack_terminal_t* terminal_free(stack_terminal_t* terminal) {
    if(terminal) {
        terminal_free(terminal->right);

        terminal->right = NULL;
        //TODO: need to free the token from terminal
    }
}*/