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

error_t err;

void analysis_init(analysis_t *data) {
    data->tok1 = malloc(sizeof(stack_terminal_t));
    data->tok2 = malloc(sizeof(stack_terminal_t));
    data->tok3 = malloc(sizeof(stack_terminal_t));
}

void analysis_free(analysis_t *data) {
    free(data->tok1);
    free(data->tok2);
    free(data->tok3);
}

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
    for (Node* currentNode = stack->top; currentNode != NULL; currentNode = currentNode->right) {
        stack_terminal_t* token = (stack_terminal_t*)currentNode->data;

        if (token != NULL && token->type < TOK_ENDMARKER) {
            return token;
        }
    }

    return NULL;
}

bool stack_push_after(Stack* stack, sem_data_type_t data_type, token_type_t token_type) {
    Node* prev = NULL;

    for(Node* continueTok = stack->top; continueTok != NULL; continueTok = continueTok->right) {
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

            // If 'prev' is NULL, update 'stack->top' to point to the new node; otherwise, update 'prev->right' to point to the new node
            new_node->right = (prev == NULL) ? stack->top : prev->right;
            (prev == NULL) ? (stack->top = new_node) : (prev->right = new_node);

            return true;
        }

        prev = continueTok;
    }

    return false;
}

int stack_count_after(Stack* stack, analysis_t* analysis) {
    int num = 0;
    stack_terminal_t *top = stack_top_token(stack);
    analysis->tok1 = NULL;
    analysis->tok2 = NULL;
    analysis->tok3 = NULL;

    while (top != NULL && top->type != TOK_ENDMARKER) {
        num++;
        if (num == 1) {
            analysis->tok1 = top;
        } else if (num == 2) {
            analysis->tok2 = top;
        } else if (num == 3) {
            analysis->tok3 = top;
        }
        stack_pop_token(stack);
        top = stack_top_token(stack);
    }

    stack_pop_token(stack);

    if(num > 3 || num == 0) {
        error = ERR_SYN;
        return -1;
    }

    return num;
}

stack_terminal_t* stack_pop_token(Stack* stack) {
    if (stack == NULL)
        return NULL;

    stack_terminal_t* popped_token = stack_top_token(stack);
    stack->top = stack->top->right;
    return popped_token;
}

void stack_free_token(Stack* stack) {
    if (stack == NULL)
        return;

    while(stack->top != NULL) {
        stack_terminal_t* popped_token = stack_pop_token(stack);
        free(popped_token);
        stack_pop(stack);
    }
}

void print_stack_contents(Stack *stack) {
    Node *current = stack->top;
    printf("Stack contents:\n");
    while (current != NULL) {
        stack_terminal_t *terminal = (stack_terminal_t *)current->data;

        printf("Data Type: %d, Token Type: %d,\n ", terminal->data, terminal->type);
        current = current->right;
    }
    printf("\n");
}
