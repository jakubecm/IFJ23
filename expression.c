/**
 *  @file expression.c
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 */

#include <stdio.h>
#include "error.h"
#include "expression.h"
#include "stack.h"
#include "exp_semantic.h"
#include "parser.h"

#define TABLE_SIZE 16

error_t error;

//NEED TO RECHECK THE TABLE!!!!!!!!!!!!!!!!!!!!!!!!!!!!
static char precedence_tab[TABLE_SIZE][TABLE_SIZE] =
{
    //   +-  */   ==   !=    <    >   <=   >=   ??    !    (    )    i    $
    {   '>', '>', '<', '<', '<', '<', '<', '<', '<', '>', '<', '>', '<', '>' },  // +-
    {   '<', '>', '<', '<', '<', '<', '<', '<', '<', '>', '<', '>', '<', '>' },  // */
    {   '>', '>', 'X', 'X', 'X', 'X', 'X', 'X', '<', '>', '<', '>', '<', '>' },  // ==
    {   '>', '>', 'X', 'X', 'X', 'X', 'X', 'X', '<', '>', '<', '>', '<', '>' },  // !=
    {   '>', '>', 'X', 'X', 'X', 'X', 'X', 'X', '<', '>', '<', '>', '<', '>' },  // <
    {   '>', '>', 'X', 'X', 'X', 'X', 'X', 'X', '<', '>', '<', '>', '<', '>' },  // >
    {   '>', '>', 'X', 'X', 'X', 'X', 'X', 'X', '<', '>', '<', '>', '<', '>' },  // <=
    {   '>', '>', 'X', 'X', 'X', 'X', 'X', 'X', '<', '>', '<', '>', '<', '>' },  // >=
    {   '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>', '<', '>' },  // ??
    {   '<', '<', '<', '<', '<', '<', '<', '<', '<', 'X', '<', '>', '<', '>' },  // !
    {   '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', 'X' },  // (
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'X', '>', 'X', '>' },  // )
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'X', '>', 'X', '>' },  // i
    {   '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', 'X', '<', 'X' }   // $
};

int get_precedence_value(token_type_t type) {
    switch (type) {
        case TOK_PLUS:
        case TOK_MINUS:
            return 0;
        case TOK_MUL:
        case TOK_DIV:
            return 1;
        case TOK_EQUAL:
        case TOK_NOTEQUAL:
        case TOK_LESS:
        case TOK_GREATER:
        case TOK_GREATEREQ:
        case TOK_LESSEQ:
            return 2;
        case TOK_DQUESTMK:
            return 3;
        case TOK_NOT:
            return 4;
        case TOK_LBRACKET:
            return 5;
        case TOK_RBRACKET:
            return 6;
        case TOK_IDENTIFIER:
        case TOK_INT:
        case TOK_DOUBLE:
        case TOK_STRING:
            return 7;
        case TOK_DOLLAR:
            return 8;
        default:
            return -1;
    }
}

int precedence(stack_terminal_t* top, token_t input) {
    int result1 = get_precedence_value(top->type);
    int result2 = get_precedence_value(input.type);

    if (result1 == -1 || result2 == -1) {
        //TODO ERROR
        return -1;
    }

    return precedence_tab[result1][result2];
}

int get_num(Stack stack) {
    int num = 0;
    stack_terminal_t *top = stack_top_token(&stack);

    while (top != NULL && top->type != TOK_ENDMARKER) {
        num++;
        top = top->right;
    }

    if(num == 0) {
        error = ERR_SYN;
        return -1;
    }

    return num;
}

void shift(Stack* stack, parser_t* parserData, sem_data_type_t* input_type) {
    stack_push_after(&stack, SEM_UNDEF, TOK_ENDMARKER);

    if(input_type == SEM_UNDEF) {
        error = ERR_SEM_NDEF;
        return;
    }

    stack_push_token(&stack, input_type, parserData->token.type);
    //GENERATOR IF TOKEN IS ID

    get_next_token();
}

void reduce(Stack* stack, int num, stack_terminal_t* tok1, stack_terminal_t* tok2, stack_terminal_t* tok3, sem_data_type_t* end_type) {
    switch(num) {
        case 1:
            if(tok1->type == TOK_INT || tok1->type == TOK_DOUBLE || tok1->type == TOK_STRING) { //operand rule, ADD SEM CHECK TO OPERATOR
                stack_push_token(&stack, tok1->data, TOK_NTERM);

            } else {
                error = ERR_SYN;
                return;
            }
            break;

        case 2:
            if(tok1->type == TOK_NOT && tok2->type == TOK_NTERM) { //NOT rule
                if(tok2->data != SEM_STRING) {
                    error = ERR_SEM_INCOMPATIBLE;
                    return;
                }
            stack_push_token(&stack, SEM_INT, TOK_NTERM);
            //GENERATOR

            } else {
                error = ERR_SYN;
                return;
            }
            break;

        case 3:
            if(tok1->type == TOK_LBRACKET && tok2->type == TOK_NTERM && tok3->type == TOK_RBRACKET) { //bracket rule
                stack_push_token(&stack, tok2->data, TOK_NTERM);

            } else if(tok1->type == TOK_NTERM && tok3->type == TOK_NTERM) {
                //Sem check if there are 3 tokens on the top of stack
                if(!sem_analysis(&tok1, &tok2, &tok3, &end_type)) {
                    return;
                }
                stack_push_token(&stack, end_type, TOK_NTERM);
                //GENERATOR

            } else {
                error = ERR_SYN;
                return;
            }
            break;

        default:
            error = ERR_SYN;
            return;
    } 
}

void exp_parsing(parser_t* parserData)  {
    Stack stack;
    stack_init(&stack);
    stack_terminal_t *tmp;
    bool continue_while = true;
    sem_data_type_t stack_type, input_type, end_type = SEM_UNDEF;
    int num = 0;

    stack_push_token(&stack, SEM_UNDEF, TOK_DOLLAR);

    while(continue_while) {
        tmp = stack_top_terminal(&stack)->type;

        if(parserData->token.type == TOK_ASSIGNMENT) {
            error = ERR_INTERNAL;
            return;
        }

        if((parserData->token.type == TOK_IDENTIFIER) && (stack_top_token(&stack)->type == TOK_NTERM || tmp == TOK_IDENTIFIER)) {
            parserData->token.type = TOK_DOLLAR;
        }

        int prec = precedence(tmp, parserData->token);
        stack_type = tok_term_type(stack_top_terminal(&stack));
        input_type = tok_type(parserData->token);

        switch(prec) {
            case '<':
                shift(&stack, parserData, input_type);
                break;

            case '>':
                stack_terminal_t *tok1, *tok2, *tok3;
                num = get_num(stack); //get token amount from stack before endmarker

                reduce(&stack, num, &tok1, &tok2, &tok3, &end_type);

                if(parserData->token.type == TOK_DOLLAR && stack_top_terminal(&stack)->type == TOK_DOLLAR) {
                    continue_while = false;
                }
                
                break;

            case '=':
                if(input_type == SEM_UNDEF) {
                    error = ERR_SEM_NDEF;
                    return;
                }

                stack_push_token(&stack, input_type, parserData->token.type);
                get_next_token();
                break;
                
            default:
                error = ERR_SYN;
                return;
        }
    }

    //Last token on the top of stack
    if(stack_top_token(&stack)->type == TOK_NTERM) {
        //...
    }
}