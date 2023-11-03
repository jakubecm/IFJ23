/**
 *  @file expression.c
 * 
 * 
 *  @authors Norman Babiak (xbabia01)
 */

#include <stdio.h>
#include "error.h"
#include "exp_semantic.h"

#define TABLE_SIZE 20

error_t error;

static char precedence_tab[TABLE_SIZE][TABLE_SIZE] =
{
    //   +    -    *    /   ==   !=    <    >   <=   >=   ??    !    (    )    id  int  flo  str  nil   $
    {   '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // +
    {   '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // -
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // *
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // /
    {   '<', '<', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // ==
    {   '<', '<', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // !=
    {   '<', '<', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // <
    {   '<', '<', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // >
    {   '<', '<', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // <=
    {   '<', '<', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // >=
    {   '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // ??
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'X', '<', '>', '<', '<', '<', '<', '<', '>' },  // !
    {   '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', '<', '<', '<', '<', 'X' },  // (
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'X', '>', 'X', 'X', 'X', 'X', 'X', '>' },  // )
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'X', '>', 'X', 'X', 'X', 'X', 'X', '>' },  // id
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'X', '>', 'X', 'X', 'X', 'X', 'X', '>' },  // int
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'X', '>', 'X', 'X', 'X', 'X', 'X', '>' },  // flo
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'X', '>', 'X', 'X', 'X', 'X', 'X', '>' },  // str
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'X', '>', 'X', 'X', 'X', 'X', 'X', '>' },  // nil
    {   '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', 'X', '<', '<', '<', '<', '<', 'X' }   // $
};

/** Helping functions for main of the exp parser **/
int precedence(stack_terminal_t* top, token_t* input) {
    //if(iskeyw(input) == true && input->type != K_NIL) {
    //    input->type = TOK_DOLLAR;
    //}
    if(top->type == TOK_MLSTRING) {
        top->type = TOK_STRING;
    }
    if(input->type == TOK_MLSTRING) {
        input->type = TOK_STRING;
    }

    if (top->type >= 20 || input->type >= 20) {
        return '>';
    }

    return precedence_tab[top->type][input->type];
}

void handle_operand(Stack* stack, sem_data_type_t data_type) {
    stack_push_token(stack, data_type, TOK_NTERM);
}

void handle_bracket(Stack* stack, sem_data_type_t data_type) {
    stack_push_token(stack, data_type, TOK_NTERM);
}

void handle_other(Stack* stack, sem_data_type_t end_type) {
    stack_push_token(stack, end_type, TOK_NTERM);
}

void shift(Stack* stack, parser_t* parserData, sem_data_type_t input_type) {
    if(!stack_push_after(stack, SEM_UNDEF, TOK_ENDMARKER)) {
        error = ERR_INTERNAL;
        return;
    }

    if(input_type == SEM_UNDEF) {
        error = ERR_SEM_NDEF;
        return;
    }

    stack_push_token(stack, input_type, parserData->token.type);

    token_type_t tmpTok = parserData->token.type;
    
    if(is_literal(tmpTok)) {
        //geenerate instruction
    }

    parserData->token = get_next_token();
}

void reduce(Stack* stack, int num, analysis_t* analysis) {
    switch(num) {
        case 1:
            if(is_literal(analysis->tok1->type)) { 
                //operand rule
                handle_operand(stack, analysis->tok1->data);

            }
            break;

        case 3:
            if(is_bracket(analysis->tok3->type, analysis->tok2->type, analysis->tok1->type)) { //bracket rule
                handle_bracket(stack, analysis->tok2->data);

            } else if(analysis->tok1->type == TOK_NTERM && analysis->tok3->type == TOK_NTERM) {
                //Sem check if there are 3 tokens on the top of stack
                if(!sem_analysis(analysis)) {
                    return;
                }
                handle_other(stack, analysis->end_type);
                printf("END TYPE: %d\n", analysis->end_type);
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
    /** Structure declarations **/
    Stack stack;
    stack_init(&stack);
        
    analysis_t* analysis = malloc(sizeof(analysis_t));
    analysis_init(analysis);

    /** Variable declarations **/
    stack_terminal_t *tmp;
    token_t endToken;
    bool continue_while = true, end = false;
    sem_data_type_t stack_type, input_type;
    int num = 0;
    printf("data token: %d\n", parserData->token.type);

    /** Exp parser start **/
    stack_push_token(&stack, SEM_UNDEF, TOK_DOLLAR);

    while(continue_while) {
        tmp = stack_top_terminal(&stack);
        printf("Tmp: %d\n", tmp->type);
        printf("data token: %d\n", parserData->token.type);

        if(parserData->token.type == TOK_ASSIGNMENT) {
            error = ERR_INTERNAL;
            return;
        }

        if(parserData->token.type == TOK_LBRACKET) {
            stack_push_after(&stack, SEM_UNDEF, TOK_ENDMARKER);
            stack_push_token(&stack, SEM_OPERATOR, parserData->token.type);
            parserData->token = get_next_token();
        }

        if((parserData->token.type == TOK_IDENTIFIER || parserData->token.type == TOK_EOF || parserData->token.type >= 20) 
            && 
           (stack_top_token(&stack)->type == TOK_NTERM || is_literal(tmp->type))) {
            endToken = parserData->token;
            end = true;
            parserData->token.type = TOK_DOLLAR;
        }

        int prec = precedence(tmp, &parserData->token);
        printf("prec: %c\n", prec);
        input_type = tok_type(parserData->token);
        printf("input data: %d\n", input_type);

        switch(prec) {
            case '<':
                printf("==============SHIFT\n");
                shift(&stack, parserData, input_type);
                if(error != ERR_OK) {
                    return;
                }
                //print_stack_contents(&stack);
                printf("==============END SHIFT\n");
                break;

            case '>':
                analysis->end_type = SEM_UNDEF;
                printf("==============REDUCE\n");
                num = stack_count_after(&stack, analysis); //get token amount from stack before endmarker
                if(error != ERR_OK) {
                    return;
                }
                printf("num reduce: %d\n", num);
                printf("BEFORE REDUCE\n");
                //print_stack_contents(&stack);

                reduce(&stack, num, analysis);
                if(error != ERR_OK) {
                    return;
                }
                print_stack_contents(&stack);
                printf("==============END REDUCE\n");
                break;

            case '=':
                if(input_type == SEM_UNDEF) {
                    error = ERR_SEM_NDEF;
                    return;
                }

                stack_push_token(&stack, input_type, parserData->token.type);
                parserData->token = get_next_token();
                break;
                
            default:
                error = ERR_SYN;
                return;
        }

        if(parserData->token.type == TOK_DOLLAR && stack_top_terminal(&stack)->type == TOK_DOLLAR) {
            continue_while = false;
        }
    }

    if(end == true) {
        parserData->token = endToken;
    }

    //Last token on the top of stack
    stack_type = stack_top_token(&stack)->data;
    printf("end type: %d\n", stack_type);
    //...

    //stack_free_token(&stack);
    analysis_free(analysis);
    free(analysis);
}


int main() {
    error = ERR_OK;
    
    parser_t* parserData = malloc(sizeof(parser_t));
    parserData->token = get_next_token();
    exp_parsing(parserData);

    printf("Parser next token: %d", parserData->token.type);
    printf("exit: %d\n", error);
}
