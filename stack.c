/**
 *  @file stack.c
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 *  @authors Jakub Ráček (xracek12)
 */

#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "stack.h"

// obecne funkce
void stack_init(Stack *stack) {
    stack->top = NULL;
}

void stack_push(Stack *s, void *data) {
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
    s->top->data = topNode->data;
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

    stack_push(stack, new_token);
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

bool stack_push_after(Stack* stack, sem_data_type_t data_type, token_type_t token_type) {
    Node* prev = NULL;
    Node* continueTok = stack->top;

    while (continueTok != NULL) {
        if (((stack_terminal_t*)continueTok->data)->type < TOK_ENDMARKER) {
            Node* new_node = (Node*)malloc(sizeof(Node));
            if (!new_node) {
                return false;
            }

            stack_terminal_t* new_terminal = (stack_terminal_t*)malloc(sizeof(stack_terminal_t));
            if (!new_terminal) {
                free(new_node);
                return false;
            }

            new_terminal->data = data_type;
            new_terminal->type = token_type;
            new_terminal->right = NULL;

            new_node->data = new_terminal;

            if (prev == NULL) {
                new_node->right = stack->top;
                stack->top = new_node;
            } else {
                new_node->right = prev->right;
                prev->right = new_node;
            }

            return true;
        }
        prev = continueTok;
        continueTok = continueTok->right;
    }
    return false;
}


bool stack_pop_token(Stack* stack) {
    if(stack->top == NULL)
        return false;

    stack_terminal_t* token = stack->top->data;
    free(token);
    stack_pop(stack);
    return true;
}