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

int precedence(stack_terminal_t* top, token_t input) {
    int result1, result2;
    switch(top->type) {
        case TOK_PLUS:
        case TOK_MINUS:
            result1 = 0;
            break;
        case TOK_MUL:
        case TOK_DIV:
            result1 = 1;
            break;
        case TOK_EQUAL:
        case TOK_NOTEQUAL:
        case TOK_LESS:
        case TOK_GREATER:
        case TOK_GREATEREQ:
        case TOK_LESSEQ:
            result1 = 2;
            break;
        case TOK_DQUESTMK:
            result1 = 3;
            break;
        case TOK_NOT:
            result1 = 4;
            break;
        case TOK_LBRACKET:
            result1 = 5;
            break;
        case TOK_RBRACKET:
            result1 = 6;
            break;
        case TOK_IDENTIFIER:
            result1 = 7;
            break;
        case TOK_DOLLAR:
            result1 = 8;
            break;
    }

    switch(input.type) {
        case TOK_PLUS:
        case TOK_MINUS:
            result2 = 0;
            break;
        case TOK_MUL:
        case TOK_DIV:
            result2 = 1;
            break;
        case TOK_EQUAL:
        case TOK_NOTEQUAL:
        case TOK_LESS:
        case TOK_GREATER:
        case TOK_GREATEREQ:
        case TOK_LESSEQ:
            result2 = 2;
            break;
        case TOK_DQUESTMK:
            result2 = 3;
            break;
        case TOK_NOT:
            result2 = 4;
            break;
        case TOK_LBRACKET:
            result2 = 5;
            break;
        case TOK_RBRACKET:
            result2 = 6;
            break;
        case TOK_IDENTIFIER:
            result2 = 7;
            break;
        case TOK_DOLLAR:
            result2 = 8;
            break;
    }
    return precedence_tab[result1][result2];
}

static rules_enum test(int number, stack_terminal_t *tok1, stack_terminal_t *tok2, stack_terminal_t *tok3) {
    switch(number) {
        case 3:
            if(tok1->type == TOK_NTERM && tok3->type == TOK_NTERM) {
                switch(tok2->type) {
                    case TOK_PLUS:
                        return PR_PLUS;
                    case TOK_MINUS:
                        return PR_MINUS;
                    case TOK_MUL:
                        return PR_MUL;
                    case TOK_DIV:
                        return PR_DIV;
                    case TOK_GREATER:
                        return PR_MORE;
                    case TOK_LESS:
                        return PR_LESS;
                    case TOK_EQUAL:
                        return PR_EQ;
                    case TOK_NOTEQUAL:
                        return PR_NEQ;
                    case TOK_GREATEREQ:
                        return PR_MEQ;
                    case TOK_LESSEQ:
                        return PR_LEQ;
                    case TOK_DQUESTMK:
                        return PR_DQUE;
                    default:
                        return PR_UNDEF;
                }
            }

            if(tok1->type == TOK_LBRACKET && tok2->type == TOK_NTERM && tok3->type == TOK_RBRACKET) {
                return PR_BRACKET;
            }
            return PR_UNDEF;

        case 2:
            if(tok1->type == TOK_NOT && tok2->type == TOK_NTERM) {
                return PR_NOT;
            }
            return PR_UNDEF;

        case 1:
            if(tok1->type == TOK_INT || tok1->type == TOK_DOUBLE || tok1->type == TOK_STRING) {
                return PR_OP;
            }
            return PR_UNDEF;
            
        default:
            return PR_UNDEF;
    }
}

void shift(Stack* stack, parser_t* parserData, sem_data_type_t* input_type) {
    stack_push_after(&stack, SEM_UNDEF, TOK_ENDMARKER);

    if(input_type == SEM_UNDEF) {
        error = ERR_SEM_NDEF;
        return;
    }

    stack_push_token(&stack, input_type, parserData->token.type);
    //GENERATOR IF TOKEN IS ID

    //parserData->token = get_next_token();
}

void exp_parsing(parser_t* parserData)  { //need to fix a b c for real stuff after parser header available
    Stack stack;
    stack_init(&stack);
    stack_terminal_t *tmp;
    bool continue_while = true;
    sem_data_type_t stack_type, input_type;

    stack_push_token(&stack, SEM_UNDEF, TOK_DOLLAR);

    while(continue_while) {
        tmp = stack_top_terminal(&stack)->type;

        if(parserData->token.type == TOK_ASSIGNMENT) {
            error = ERR_INTERNAL;
            return;
        }

        if((parserData->token.type == TOK_IDENTIFIER) && (stack_top_terminal(&stack)->type == TOK_NTERM || tmp == TOK_IDENTIFIER)) {
            parserData->token.type = TOK_DOLLAR;
        }

        int prec = precedence(tmp, parserData->token); // Need to add second param after parser header available
        stack_type = tok_term_type(stack_top_terminal(&stack));
        input_type = tok_type(parserData->token);

        switch(prec) {
            case '<':
                shift(&stack, parserData, input_type);
                break;

            case '>':
                //TODO REDUCING FUNCTION
                break;

            case '=':
                if(input_type == SEM_UNDEF) {
                    error = ERR_SEM_NDEF;
                    return;
                }

                stack_push_token(&stack, input_type, parserData->token.type);
                //parserData->token = get_next_token();
                break;
                
            default:
                error = ERR_SYN;
                return;
        }
    }

    if(stack_top_token(&stack)->type == TOK_NTERM) {
        //...
    }
}