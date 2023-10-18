/**
 *  @file stack.c
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 */

#include <stdio.h>
#include "error.h"
#include "stack.h"

error_t err;

void stack_init(stack_t *stack) {
    stack->top = NULL;
}

void stack_free(stack_t* stack) {
    while(stack_pop(stack));
}


void stack_push(stack_t* stack, data_enum data_type, token_type_t token_type) {
    stack_terminal_t* new_token = (stack_terminal_t*) malloc(sizeof(stack_terminal_t));
    if(new_token == NULL) {
        err = ERR_INTERNAL;
        return;
    }

    new_token->data = data_type;
    new_token->type = token_type;

    new_token->right = stack->top;
    stack->top = new_token;
}

stack_terminal_t* stack_top(stack_t* stack) {
    return stack->top;
}

stack_terminal_t* stack_top_terminal(stack_t* stack) {
    stack_terminal_t* new_item = stack->top;

    if(new_item == NULL) {
        return NULL;
    }

    for(new_item; new_item != NULL; new_item->right) {
        if(new_item < STACK_END) {
            return new_item;
        }
    }
}

bool stack_pop(stack_t* stack) {
    if(stack->top != NULL) {
        stack_terminal_t* token = stack->top;
        stack->top = token->right;
        free(token);

        return true;
    }
    return false;
}

void stack_pop_more(stack_t* stack, int number) {
    for(int i = 0; i < number; i++) {
        stack_pop(stack);
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