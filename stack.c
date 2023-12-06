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

extern error_t error;

// ---------------------------------- Basic functions -----------------------------------

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
    free(topNode);
    topNode = NULL;
    return true;
}

void *stack_top(stack_t *s) {
    return s->top;
}

// -------------------------------- Exp semantic functions -------------------------------

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

// -------------------------------- Exp parser functions --------------------------------

void stack_push_token(stack_t* stack, sem_data_type_t data_type, token_type_t token_type) {
    stack_terminal_t* new_token = (stack_terminal_t*) malloc(sizeof(stack_terminal_t));
    if(new_token == NULL) {
        error = ERR_INTERNAL;
        print_error_and_exit(error);
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
    stack_terminal_t* token;

    for (Node* currentNode = stack->top; currentNode != NULL; currentNode = currentNode->right) {
        token = (stack_terminal_t*)currentNode->data;

        if (token != NULL && token->type < TOK_ENDMARKER) {
            return token;
        }
    }

    return NULL;
}

bool stack_push_after(stack_t* stack, sem_data_type_t data_type, token_type_t token_type) {
    Node **ptr = &stack->top;
    while (*ptr && ((stack_terminal_t*)(*ptr)->data)->type >= TOK_ENDMARKER) {
        ptr = &(*ptr)->right;
    }

    Node* newNode = malloc(sizeof(Node));
    stack_terminal_t* newTerminal = malloc(sizeof(stack_terminal_t));
    if (!newNode || !newTerminal) {
        free(newNode);
        free(newTerminal);
        return false;
    }

    *newTerminal = (stack_terminal_t){.data = data_type, 
                                      .type = token_type, 
                                      .right = NULL};

    *newNode = (Node){.data = newTerminal, 
                      .right = *ptr};
                      
    *ptr = newNode;

    return true;
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
    if (stack == NULL) {
        return;
    }

    while (stack->top != NULL) {
        stack_terminal_t* popped_token = stack_pop_token(stack);

        free(popped_token);

        Node* topNode = stack->top;
        if (topNode != NULL) {
            stack->top = topNode->right;
            free(topNode);
        }
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
    size_t size = 19;
    symbol_table_t* table = symbol_table_init(size);
    stack_push(stack, table);
}

bool is_table_global(stack_t* stack) {
    return stack->top->right == NULL;
}

void stack_pop_table(stack_t* stack) {
    symbol_table_t* table = stack_top_table(stack);
    symbol_table_free(table);
    stack_pop(stack);
}

symbol_table_t* stack_top_table(stack_t* stack) {
    return (stack_top(stack) != NULL) ? stack->top->data : NULL;
}

symbol_table_t* stack_bottom_table(stack_t* stack) {
    Node* currentNode = stack->top;
    while (currentNode->right != NULL) {
        currentNode = currentNode->right;
    }

    return (currentNode != NULL) ? currentNode->data : NULL;
}

data_t *stack_lookup_var(stack_t* stack, htab_key_t key) {
    Node* currentNode = stack->top;
    while (currentNode != NULL) {
        symbol_table_t* table = (symbol_table_t*)currentNode->data;
        data_t *symbol = symbol_table_lookup_var(table, key);
        if (symbol != NULL) {
            return symbol;
        }
        currentNode = currentNode->right;
    }

    return NULL;
}

data_t *stack_lookup_func(stack_t* stack, htab_key_t key) {
    Node* currentNode = stack->top;
    while (currentNode != NULL) {
        symbol_table_t* table = (symbol_table_t*)currentNode->data;
        data_t *symbol = symbol_table_lookup_func(table, key);
        if (symbol != NULL) {
            return symbol;
        }
        currentNode = currentNode->right;
    }

    return NULL;
}

bool stack_lookup_var_in_global(stack_t* stack, htab_key_t key) {
    Node* currentNode = stack->top;
    while (currentNode != NULL) {
        symbol_table_t* table = (symbol_table_t*)currentNode->data;
        data_t *symbol = symbol_table_lookup_var(table, key);
        if (symbol != NULL && table != stack_bottom_table(stack)) {
            return false;
        }
        currentNode = currentNode->right;
    }

    return true;
}

int stack_get_nested_level(stack_t* stack) {
    int level = 0;
    Node* currentNode = stack->top;
    while (currentNode != NULL) {
        level++;
        currentNode = currentNode->right;
    }

    return level;
}

int stack_get_nested_level_of_var(stack_t* stack, htab_key_t key) {
    int level = 0;
    Node* currentNode = stack->top;
    while (currentNode != NULL) {
        symbol_table_t* table = (symbol_table_t*)currentNode->data;
        data_t *symbol = symbol_table_lookup_var(table, key);
        if (symbol != NULL) {
            return level;
        }
        level++;
        currentNode = currentNode->right;
    }

    return -1;
}

int stack_height(stack_t* stack) {
    int height = 0;
    Node* currentNode = stack->top;
    while (currentNode != NULL) {
        height++;
        currentNode = currentNode->right;
    }

    return height;
}
