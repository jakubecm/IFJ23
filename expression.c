/**
 *  @file expression.c
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 */

#include <stdio.h>
#include "error.h"
#include "expression.h"

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

//TODO AFTER TOKEN TYPES INSERTED
static rules_enum test(int number, stack_terminal_t *tok1, stack_terminal_t *tok2, stack_terminal_t *tok3) {
    switch(number) {
        case 1:
            if(tok1->type == 2 || tok1->type == 3)
        case 2:
        case 3:
        default:
    }
} 