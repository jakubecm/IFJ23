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

#define TABLE_SIZE 16

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

int precedence(token_t top, token_t input) {
    return precedence_tab[top.type][input.type];
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
                    case TOK_MORE:
                        return PR_MORE;
                    case TOK_LESS:
                        return PR_LESS;
                    case TOK_EQ:
                        return PR_EQ;
                    case TOK_NEQ:
                        return PR_NEQ;
                    case TOK_MEQ:
                        return PR_MEQ;
                    case TOK_LEQ:
                        return PR_LEQ;
                    default:
                        return PR_UNDEF;
                }
            }

            if(tok1->type == TOK_LBRACKET && tok2->type == TOK_NTERM && tok3->type == TOK_RBRACKET) {
                return PR_BRACKET;
            }

        case 2:
            if(tok1->type == a && tok2->type == TOK_NTERM) {
                return PR_NOT;
            }

        case 1:
            if(tok1->type == TOK_INT || tok1->type == TOK_DOUBLE || tok1->type == TOK_STRING) {
                return PR_OP;
            }
            
        default:
            return PR_UNDEF;
    }
}

bool sem_analysis(stack_terminal_t* tok1, stack_terminal_t* tok2, stack_terminal_t* tok3, rules_enum rule) {
    switch(rule) {
        case PR_NOT:
        case PR_BRACKET:
        case PR_PLUS:
        case PR_MINUS:
        case PR_MUL:
        case PR_DIV:
        case PR_LESS:
        case PR_MORE:
        case PR_EQ:
        case PR_NEQ:
        case PR_MEQ:
        case PR_LEQ:
    }
}

void exp_parsing(int a, int b, int c)  { //need to fix a b c for real stuff after parser header available
    stack_t stack;
    stack_init(&stack);
    stack_terminal_t *top;
    bool continue_while = true;

    stack_push(&stack, UNDEFINED, TOK_DOLLAR);

    while(continue_while) {
        top = stack_top_terminal(&stack);

        int prec = precedence(top->type, ???? ); // Need to add second param after parser header available

        switch(prec) {
            case '<':
            case '>':
            case '=':
        }
    }
}