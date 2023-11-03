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

int get_num(Stack* stack, analysis_t* analysis) {
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

    stack_pop_token(stack); //pop endmarker
    printf("NUM COUNT %d\n", num);
    print_stack_contents(stack);

    if(num > 3) {
        error = ERR_SYN;
        return -1;
    }

    return num;
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
    
    if(tmpTok == TOK_IDENTIFIER || tmpTok == TOK_INT || tmpTok == TOK_DOUBLE || tmpTok == TOK_STRING) {
        //geenerate instruction
    }

    parserData->token = get_next_token();
}

void reduce(Stack* stack, int num, analysis_t* analysis) {
    switch(num) {
        case 1:
            if(analysis->tok1->type == TOK_INT || analysis->tok1->type == TOK_DOUBLE || analysis->tok1->type == TOK_STRING 
               || analysis->tok1->type == TOK_IDENTIFIER || analysis->tok1->type == K_NIL) { 
                //operand rule
                stack_push_token(stack, analysis->tok1->data, TOK_NTERM);

            }
            break;

        case 3:
            printf("%d %d %d\n", analysis->tok1->type, analysis->tok2->type, analysis->tok3->type);
            if(analysis->tok3->type == TOK_LBRACKET && analysis->tok2->type == TOK_NTERM && analysis->tok1->type == TOK_RBRACKET) { //bracket rule
                stack_push_token(stack, analysis->tok2->data, TOK_NTERM);

            } else if(analysis->tok1->type == TOK_NTERM && analysis->tok3->type == TOK_NTERM) {
                //Sem check if there are 3 tokens on the top of stack
                if(!sem_analysis(analysis)) {
                    return;
                }
                stack_push_token(stack, analysis->end_type, TOK_NTERM);
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
    Stack stack;
    stack_init(&stack);
    stack_terminal_t *tmp;
    token_t endToken;
    bool continue_while = true, end = false;
    sem_data_type_t stack_type, input_type;
    int num = 0;

    analysis_t* analysis = malloc(sizeof(analysis_t));
    analysis->tok1 = malloc(sizeof(stack_terminal_t));
    analysis->tok2 = malloc(sizeof(stack_terminal_t));
    analysis->tok3 = malloc(sizeof(stack_terminal_t));
    printf("data token: %d\n", parserData->token.type);

    stack_push_token(&stack, SEM_UNDEF, TOK_DOLLAR);
    /*stack_push_after(&stack, SEM_UNDEF, TOK_ENDMARKER);
    stack_push_token(&stack, SEM_INT, TOK_INT);
    num = get_num(&stack, analysis);
    reduce(&stack, num, analysis);
    print_stack_contents(&stack);
    stack_push_after(&stack, SEM_UNDEF, TOK_ENDMARKER);
    stack_push_token(&stack, SEM_OPERATOR, TOK_PLUS);
    print_stack_contents(&stack);

    stack_push_after(&stack, SEM_UNDEF, TOK_ENDMARKER);
    stack_push_token(&stack, SEM_INT, TOK_INT);
    num = get_num(&stack, analysis);
    reduce(&stack, num, analysis);
    print_stack_contents(&stack);

    stack_push_after(&stack, SEM_UNDEF, TOK_ENDMARKER);
    stack_push_token(&stack, SEM_OPERATOR, TOK_MUL);
    print_stack_contents(&stack);*/

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

        if((parserData->token.type == TOK_IDENTIFIER || parserData->token.type == TOK_EOF) 
            && (stack_top_token(&stack)->type == TOK_NTERM || tmp->type == TOK_IDENTIFIER || tmp->type == TOK_INT || tmp->type == TOK_STRING || tmp->type == TOK_DOUBLE || tmp->type == TOK_RBRACKET)) {
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
                num = get_num(&stack, analysis); //get token amount from stack before endmarker
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


                if(parserData->token.type == TOK_DOLLAR && stack_top_terminal(&stack)->type == TOK_DOLLAR) {
                    continue_while = false;
                }
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
    }

    if(end == true) {
        parserData->token = endToken;
    }

    //Last token on the top of stack
    if(stack_top_token(&stack)->type == TOK_NTERM) {
        stack_type = stack_top_token(&stack)->data;
        printf("end type: %d\n", stack_type);
        //...
    }

    free(analysis->tok1);
    free(analysis->tok2);
    free(analysis->tok3);
    free(analysis);
}


int main() {
    error = ERR_OK;
    Stack stack;
    stack_init(&stack);
    printf("%p\n", stack.top);

    //$<E + <E * < I 
    //$< E + <E * E
    print_stack_contents(&stack);
    
    parser_t* parserData = malloc(sizeof(parser_t));
    parserData->token = get_next_token();
    exp_parsing(parserData);
    print_stack_contents(&stack);

    printf("Parser next token: %d", parserData->token.type);
    printf("exit: %d\n", error);
}
