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
void stack_init(stack_t *stack) {
    stack->top = NULL;
}

void stack_free(stack_t* stack) {
    free(stack);
}

void stack_push(stack_t *s, void *data) {
    Node *newNode = malloc(sizeof(Node));
    newNode->data = data;
    newNode->right = s->top;
    s->top = newNode;
}

bool stack_pop(stack_t *s) {
    if (s->top == NULL) {
        return false;
    }

    Node *topNode = s->top;
    s->top = topNode->right;
    s->top->data = topNode->data;
    free(topNode);
    return true;
}

void *stack_top(stack_t *s) {
    return s->top;
}
// konec obecnych funkci

void stack_push_token(stack_t* stack, sem_data_type_t data_type, token_type_t token_type) {
    stack_terminal_t* new_token = (stack_terminal_t*) malloc(sizeof(stack_terminal_t));
    if(new_token == NULL) {
        err = ERR_INTERNAL;
        return;
    }

    new_token->data = data_type;
    new_token->type = token_type;

    stack_push(stack, new_token);
}

stack_terminal_t* stack_top_token(stack_t* stack) {
    return (stack_top(stack) != NULL) ? stack->top->data : NULL;
}

stack_terminal_t* stack_top_terminal(stack_t* stack) {
    for (Node* currentNode = stack->top; currentNode != NULL; currentNode = currentNode->right) {
        stack_terminal_t* token = (stack_terminal_t*)currentNode->data;

        if (token != NULL && token->type < TOK_ENDMARKER) {
            return token;
        }
    }

    return NULL;
}

bool stack_push_after(stack_t* stack, sem_data_type_t data_type, token_type_t token_type) {
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

int stack_count_after(stack_t* stack, analysis_t* analysis) {
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

stack_terminal_t* stack_pop_token(stack_t* stack) {
    if (stack == NULL)
        return NULL;

    stack_terminal_t* popped_token = stack_top_token(stack);
    stack->top = stack->top->right;
    return popped_token;
}

void stack_free_token(stack_t* stack) {
    if (stack == NULL)
        return;

    while(stack->top != NULL) {
        stack_terminal_t* popped_token = stack_pop_token(stack);
        free(popped_token);
        stack_pop(stack);
    }
}

void print_stack_contents(stack_t *stack) {
    Node *current = stack->top;
    printf("stack_t contents:\n");
    while (current != NULL) {
        stack_terminal_t *terminal = (stack_terminal_t *)current->data;

        printf("Data Type: %d, Token Type: %d,\n ", terminal->data, terminal->type);
        current = current->right;
    }
    printf("\n");
}

// --------------------------------- Symtable functions ---------------------------------

void stack_push_table(stack_t* stack) {
    symbol_table_t* table = symbol_table_init(97);
    stack_push(stack, table);
}

void stack_pop_table(stack_t* stack) {
    symbol_table_t* table = stack_top(stack);
    symbol_table_free(table);
    stack_pop(stack);
}

symbol_table_t* stack_top_table(stack_t* stack) {
    return stack_top(stack);
}

void stack_destroy_table(stack_t* stack) {
    symbol_table_t* table = stack_top(stack);
    symbol_table_free(table);
    stack_pop(stack);
}

data_t stack_lookup_var(stack_t* stack, htab_key_t key) {
    Node* currentNode = stack->top;
    while (currentNode != NULL) {
        symbol_table_t* table = (symbol_table_t*)currentNode->data;
        data_t symbol = symbol_table_lookup_var(table, key);
        if (symbol.type != NOT_FOUND) {
            return symbol;
        }
        currentNode = currentNode->right;
    }

    return (data_t){.name = NULL, .value = {.var_id = 0}, .type = NOT_FOUND};
}