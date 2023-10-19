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

#define TABLE_SIZE 16

error_t error;

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
    return precedence_tab[top->type][input.type];
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

void exp_parsing(int a, int b, int c)  { //need to fix a b c for real stuff after parser header available
    stack_t stack;
    stack_init(&stack);
    stack_terminal_t *top;
    bool continue_while = true;

    stack_push(&stack, TOK_UNDEF, TOK_DOLLAR);

    while(continue_while) {
        top = stack_top_terminal(&stack);

        int prec = precedence(top, ???); // Need to add second param after parser header available

        switch(prec) {
            case '<':
            case '>':
            case '=':
        }
    }
}