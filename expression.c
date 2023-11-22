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
    {   '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '<', '<', '>' },  // +
    {   '>', '>', '<', '<', '>', '>', '>', '>', '>', '>', '>', '>', '<', '>', '<', '<', '<', '<', '<', '>' },  // -
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // *
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // /
    {   '<', '<', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // ==
    {   '<', '<', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // !=
    {   '<', '<', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // <
    {   '<', '<', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // >
    {   '<', '<', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // <=
    {   '<', '<', '<', '<', 'X', 'X', 'X', 'X', 'X', 'X', '>', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // >=
    {   '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '<', '<', '<', '<', '<', '>' },  // ??
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'X', '<', '>', '>', 'X', 'X', 'X', 'X', '>' },  // !
    {   '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', '<', '<', '<', '<', 'X' },  // (
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'X', '>', 'X', 'X', 'X', 'X', 'X', '>' },  // )
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'X', '>', 'X', 'X', 'X', 'X', 'X', '>' },  // id
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'X', '>', 'X', 'X', 'X', 'X', 'X', '>' },  // int
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'X', '>', 'X', 'X', 'X', 'X', 'X', '>' },  // flo
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'X', '>', 'X', 'X', 'X', 'X', 'X', '>' },  // str
    {   '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', 'X', '>', 'X', 'X', 'X', 'X', 'X', '>' },  // nil
    {   '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', 'X', '<', '<', '<', '<', '<', 'X' }   // $
};

/** Handling functions for shift, reduce and next token **/

/**
 * @brief Function to handle reduce rule of a single operand
 * @param stack - stack we are working with
 * @param data_type - token data type to use in stack
 */
void handle_operand(stack_t* stack, sem_data_type_t data_type) {
    stack_push_token(stack, data_type, TOK_NTERM);
}

/**
 * @brief Function to handle reduce rule with brackets (E)
 * @param stack - stack we are working with
 * @param data_type - token data type to use in stack
 */
void handle_bracket(stack_t* stack, sem_data_type_t data_type) {
    stack_push_token(stack, data_type, TOK_NTERM);
}

/**
 * @brief Function to handle every expression rule with 3 tokens
 * @param stack - stack we are working with
 * @param end_type - data type received from semantic analysis
 */
void handle_other(stack_t* stack, sem_data_type_t end_type) {
    stack_push_token(stack, end_type, TOK_NTERM);
}

/**
 * @brief Function to handle the first, next and end tokens of the input
 * @param parserData - parser structure given by core parser
 * @param stack - stack we are working with
 * @param end - bool indicating the end of the expression
 * @param endToken - temporary variable to save the next token after the expression
 * @param tmp - temporary variable to get the top token of the stack
 */
void handle_upcoming(parser_t* parserData, stack_t* stack, bool* end, token_t* endToken, stack_terminal_t* tmp) {
    if (parserData->token.type == TOK_ASSIGNMENT) {
        error = ERR_INTERNAL;
        return;
    }

    if (parserData->token.type == TOK_LBRACKET) {
        stack_push_after(stack, SEM_UNDEF, TOK_ENDMARKER);
        stack_push_token(stack, SEM_OPERATOR, parserData->token.type);
        parserData->token = get_next_token();
    }

    if ((parserData->next_token.type == TOK_IDENTIFIER || parserData->next_token.type == TOK_EOF || parserData->next_token.type >= 20) &&
        (stack_top_token(stack)->type == TOK_NTERM || is_literal(tmp->type) || tmp->type == TOK_RBRACKET || tmp->type == TOK_NOT)) {
        *endToken = parserData->next_token;
        *end = true;
        parserData->next_token.type = TOK_DOLLAR;
    }
}


/** Helping functions for main of the exp parser **/
int precedence(stack_terminal_t* top, token_t* input) {
    token_t tmpInput = *input;
    stack_terminal_t tmpTop = *top;

    if(input->type == TOK_IDENTIFIER) {
        if(iskeyw(input) == true && input->type != K_NIL) {
            tmpInput.type = TOK_DOLLAR;
        }
    }
    if(top->type == TOK_MLSTRING) {
        tmpTop.type = TOK_STRING;
    }
    if(input->type == TOK_MLSTRING) {
        tmpInput.type = TOK_STRING;
    }

    if (top->type >= 20 || input->type >= 20) {
        return 'X';
    }

    return precedence_tab[tmpTop.type][tmpInput.type];
}

void shift(stack_t* stack, parser_t* parserData, sem_data_type_t input_type) {
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
        switch(tmpTok) {
            case TOK_INT:
                gen_push_int(gen, parserData->token.attribute.number);
                break;

            case TOK_DOUBLE:
                gen_push_float(gen, parserData->token.attribute.decimal);
                break;

            case TOK_STRING:
                gen_push_string(gen, parserData->token.attribute.string);
                break;

            case K_NIL:
                gen_push_nil(gen);
                break;

            case TOK_IDENTIFIER:
                gen_push_var(gen, parserData->token.attribute.string, parserData->in_function);
                break;

            default:
                break;
        }
    }

    load_token(&parserData);
}

void reduce(stack_t* stack, int num, analysis_t* analysis) {
    switch(num) {
        case 1:
            if(is_literal(analysis->tok1->type)) { 
                //operand rule
                handle_operand(stack, analysis->tok1->data);
                return;
            }

            error = ERR_SYN;
            break;

        case 2:
            if(analysis->tok2->type == TOK_NTERM && analysis->tok1->type == TOK_NOT) {
                //Semantic check for IDENTIFIER_POSTFIX! cannot be nil
                if(analysis->tok2->data == SEM_NIL) {
                    error = ERR_SEM_TYPE;
                    return;
                }
                
                stack_push_token(stack, analysis->tok2->data, TOK_NTERM);

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
                DEBUG_PRINT("END TYPE: %d\n", analysis->end_type);
                gen_expression(gen, analysis->tok2->type);

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

void prec_analysis(stack_t *stack, parser_t* parserData, stack_terminal_t* tmp, analysis_t* analysis) {
    int prec = precedence(tmp, &parserData->token);
    sem_data_type_t input_type = tok_type(parserData);
    DEBUG_PRINT("prec: %c\n", prec);
    DEBUG_PRINT("input data: %d\n", input_type);
    switch(prec) {
        case '<':
            DEBUG_PRINT("==============SHIFT\n");
            shift(stack, parserData, input_type);
            if(error != ERR_OK) {
                return;
            }

            #ifdef DEBUG
                print_stack_contents(stack);
            #endif
            DEBUG_PRINT("==============END SHIFT\n");

            break;

        case '>':
            analysis->end_type = SEM_UNDEF;
            DEBUG_PRINT("==============REDUCE\n");
            int num = stack_count_after(stack, analysis); //get token amount from stack before endmarker
            if(error != ERR_OK) {
                return;
            }

            DEBUG_PRINT("num reduce: %d\n", num);
            DEBUG_PRINT("BEFORE REDUCE\n");
            #ifdef DEBUG
                    print_stack_contents(stack);
            #endif

            reduce(stack, num, analysis);
            if(error != ERR_OK) {
                return;
            }

            #ifdef DEBUG
                print_stack_contents(stack);
            #endif
            DEBUG_PRINT("==============END REDUCE\n");
            break;

        case '=':
            if(input_type == SEM_UNDEF) {
                error = ERR_SEM_NDEF;
                return;
            }

            stack_push_token(stack, input_type, parserData->token.type);
            load_token(&parserData);
            break;
                
        default:
            error = ERR_SYN;
            return;
        }
}

variable_type_t convert_type(sem_data_type_t type) {
    switch(type) {
        case SEM_INT:
            return VAL_INT;
        case SEM_FLOAT:
            return VAL_DOUBLE;
        case SEM_STRING:
            return VAL_STRING;
        case SEM_BOOL:
            return VAL_BOOL;
        default:
            return VAL_UNKNOWN;
    }
}

/** Main expression parser **/
variable_type_t exp_parsing(parser_t* parserData)  {
    /** Structure declarations **/
    stack_t stack;
    stack_init(&stack);
        
    analysis_t* analysis = malloc(sizeof(analysis_t));
    analysis_init(analysis);

    /** Variable declarations **/
    stack_terminal_t *tmp;
    token_t endToken; 
    bool continue_while = true, end = false;
    sem_data_type_t stack_type;
    variable_type_t return_type;
    DEBUG_PRINT("data token: %d\n", parserData->token.type);

    /** Exp parser start **/
    stack_push_token(&stack, SEM_UNDEF, TOK_DOLLAR);

    while(continue_while) {
        tmp = stack_top_terminal(&stack);
        DEBUG_PRINT("Tmp: %d\n", tmp->type);
        DEBUG_PRINT("data token: %d\n", parserData->token.type);

        handle_upcoming(parserData, &stack, &end, &endToken, tmp);
        if(error != ERR_OK) {
            CLEANUP_RESOURCES(stack, analysis);
            print_error_and_exit(error);
        }

        prec_analysis(&stack, parserData, tmp, analysis);
        if(error != ERR_OK) {
            CLEANUP_RESOURCES(stack, analysis);
            print_error_and_exit(error);
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
    DEBUG_PRINT("end type: %d\n", stack_type);

    return_type = convert_type(stack_type);
    DEBUG_PRINT("Converted: %d\n", return_type);
    CLEANUP_RESOURCES(stack, analysis);
    return return_type;
}

#ifdef DEBUG
int main() {
    error = ERR_OK;

    parser_t* parserData = malloc(sizeof(parser_t));
    parserData->token = get_next_token();
    variable_type_t test = exp_parsing(parserData);

    DEBUG_PRINT("Variable for core: %d\n", test);
    DEBUG_PRINT("Parser next token: %d\n", parserData->token.type);
    DEBUG_PRINT("exit: %d\n", error);
}
#endif