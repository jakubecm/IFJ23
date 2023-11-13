/**
 *  @file parser.c
 *
 *
 *  @authors Milan Jakubec (xjakub41)
 *  @authors Jakub Ráček (xracek12)
 */

#include "parser.h"

#include "error.h"
#include "expression.h"

/** ================== LL GRAMMAR ==================

<program> -> <statement> <program> | <function_definition> <program> | ε

<statement_list> -> <statement> <statement_list> | ε

<statement> -> <variable_definition_let> | <variable_definition_var> | <assignment> | <conditional_statement> | <loop> | <function_call> | <return_statement>

<function_definition> -> "func" <identifier> "(" <parameter_list> ")" <function_return_type_and_body>

<function_return_type_and_body> -> "{" <void_function_body> "}" | "->" <type> "{" <nonvoid_function_body> "}"

<nonvoid_function_body> -> <statement_list> | ε

<void_function_body> -> <statement_list> | ε

<parameter_list> -> <parameter> <more_parameters> | ε

<more_parameters> -> "," <parameter> <more_parameters> | ε

<parameter> -> <no_name_parameter> | <identifier_parameters>

<no_name_parameter> -> "_" <identifier> ":" <type>

<identifier_parameter> -> <identifier> <rest_of_identifier_parameter>

<rest_of_identifier_parameter> -> "_" ":" <type> |  <identifier> ":" <type>

<type> -> "Double" | "Int" | "String" | "Double?" | "Int?" | "String?"

<variable_definition_let> -> "let" <identifier> <definition_types> (pozn. let je NEMODIFIKOVATELNA promenna)

<variable_definition_var> -> "var" <identifier> <definition_types>

<definition_types> -> <type_def> | <initialization>

<type_def> -> ":" <type> <type_def_follow>

<type_def_follow> -> <initialization> | ε

<initialization> -> "=" <expression>

<assignment> -> <identifier> "=" <assignment_type>

<assignment_type> -> <function_call> | <expression>

<conditional_statement> -> if <if_statement>

<if_statement> -> <classical_statement> | <variable_statement>

<classical_statement> -> <expression> "{" <statement_list>  "}" "else" "{" <statement_list> "}"

<variable_statement> -> let <identifier> "{" <statement_list> "}" "else" "{" <statement_list> "}" (pozn. identifier zastupuje drive
definovanou nemodifikovatelnou promennou)

<loop> -> "while" <expression> "{" <statement_list> "}"

<function_call> -> <identifier> "(" <arguments> ")"

<arguments> -> <expression> <more_arguments> | ε

<more_arguments> -> "," <expression> <more_arguments> | ε

<return_statement> -> "return" <returned_expression>

<returned_expression> -> <expression> | ε

================== LL GRAMMAR END ================== */

//================= FUNCTION DECLARATIONS START HERE ================= //
bool rule_program(parser_t *parser);
bool rule_statement_list(parser_t *parser);
bool rule_statement(parser_t *parser);
bool rule_function_definition(parser_t *parser);
bool rule_function_return_type_and_body(parser_t *parser);
bool rule_nonvoid_function(parser_t *parser);
bool rule_nonvoid_function_body(parser_t *parser);
bool rule_void_function_body(parser_t *parser);
bool rule_statement_nonvoid_function(parser_t *parser);
bool rule_statement_void_function(parser_t *parser);
bool rule_parameter_list(parser_t *parser);
bool rule_more_parameters(parser_t *parser);
bool rule_parameter(parser_t *parser);
bool rule_no_name_parameter(parser_t *parser);
bool rule_identifier_parameter(parser_t *parser);
bool rule_rest_of_identifier_parameter(parser_t *parser);
bool rule_no_id_parameter(parser_t *parser);
bool rule_all_parameters(parser_t *parser);
bool rule_type(parser_t *parser);
bool rule_variable_definition_let(parser_t *parser);
bool rule_variable_definition_var(parser_t *parser);
bool rule_definition_types(parser_t *parser);
bool rule_type_def(parser_t *parser);
bool rule_type_def_follow(parser_t *parser);
bool rule_initialization(parser_t *parser);
bool rule_assignment(parser_t *parser);
bool rule_assignment_type(parser_t *parser);
bool rule_conditional_statement(parser_t *parser);
bool rule_if_statement(parser_t *parser);
bool rule_classical_statement(parser_t *parser);
bool rule_variable_statement(parser_t *parser);
bool rule_loop(parser_t *parser);
bool rule_function_call(parser_t *parser);
bool rule_arguments(parser_t *parser);
bool rule_more_arguments(parser_t *parser);
bool rule_return_statement(parser_t *parser);
bool rule_returned_expression(parser_t *parser);
bool rule_empty_return_statement(parser_t *parser);
//================= FUNCTION DECLARATIONS END HERE ================= //

//================= PARSER FUNCTIONS ================= //

void parser_init(parser_t *parser, scanner_t *scanner)
{
    parser->scanner = scanner;
    parser->stack = malloc(sizeof(stack_t));
    stack_init(parser->stack);
    parser->function = NULL;

    parser->token = get_next_token();
    // Nebude lepsi udelat funkci ktera jen peekne na dalsi token a scanner ho uchova misto tohoto?
    parser->next_token = get_next_token();
}

void parser_destroy(parser_t *parser)
{
    stack_destroy_table(parser->stack);
    stack_free(parser->stack);
}

bool is_type(parser_t *parser, token_type_t type) { return parser->token.type == type; }

bool is_type_next(parser_t *parser, token_type_t type) { return parser->next_token.type == type; }

void load_token(parser_t *parser)
{
    parser->token = parser->next_token;
    parser->next_token = get_next_token();
}

//================= PARSER FUNCTIONS END HERE ================= //
//-------------------------------------------------------------//
//================= GRAMMAR RULES START HERE ================= //

// <program> -> <statement> <program> | <function_definition> <program> | ε

bool rule_program(parser_t *parser){
    if (is_type(parser, TOK_EOF)){
        return true;
    }
    else if (rule_statement(parser)){
        if (rule_program(parser)){
            return true;
        }
    }
    else if (rule_function_definition(parser)){
        if (rule_program(parser)){
            return true;
        }
    }

    return false;
}

// <statement_list> -> <statement> <statement_list> | ε
// Note: statement list is only used in condtional statements and loops, thats why there is a check for RCURLYBRACKET

bool rule_statement_list(parser_t *parser){
    if (rule_statement(parser)){

        if (rule_statement_list(parser)){
            return true;
        }
    }
    else if (is_type(parser, TOK_RCURLYBRACKET)){
        return true;
    }

    return false;
}

// <statement> -> <variable_definition_let> | <variable_definition_var> | <assignment> | <conditional_statement> | <loop> | <function_call> | <return_statement>

bool rule_statement(parser_t *parser){
    switch(parser->token.type){
        case K_LET:
            return rule_variable_definition_let(parser);
        case K_VAR:
            return rule_variable_definition_var(parser);
        case TOK_IDENTIFIER:
            if(is_type_next(parser, TOK_LBRACKET)){
                return rule_function_call(parser);
            }
            else{
                return rule_assignment(parser);
            }
        case K_IF:
            return rule_conditional_statement(parser);
        case K_WHILE:
            return rule_loop(parser);
        case K_RETURN:
            return rule_return_statement(parser);
        default:
            return false;
    }
}

// <function_definition> -> "func" <identifier> "(" <parameter_list> ")" <function_return_type_and_body>

bool rule_function_definition(parser_t *parser){
    if (!is_type(parser, K_FUNC)){
        return false;
    }
    load_token(parser);
    if (!is_type(parser, TOK_IDENTIFIER)){
        return false;
    }
    load_token(parser);
    if (!is_type(parser, TOK_LBRACKET)){
        return false;
    }
    load_token(parser);
    if (!rule_parameter_list(parser)){
        return false;
    }
    if (!is_type(parser, TOK_RBRACKET)){
        return false;
    }
    load_token(parser);
    if (!rule_function_return_type_and_body(parser)){
        return false;
    }

    return true;
}

// <function_return_type_and_body> -> "{" <void_function_body> "}" | "->" <type> "{" <nonvoid_function_body> "}"

bool rule_function_return_type_and_body(parser_t *parser){
    if (!is_type(parser, TOK_LCURLYBRACKET)){
        return false;
    }
    load_token(parser);
    if (rule_void_function_body(parser)){
        if (!is_type(parser, TOK_RCURLYBRACKET)){
            return false;
        }
        load_token(parser);
        return true;
    }
    else if (is_type(parser, TOK_ARROW)){
        load_token(parser);
        if (!rule_type(parser)){
            return false;
        }
        load_token(parser);
        if (!is_type(parser, TOK_LCURLYBRACKET)){
            return false;
        }
        load_token(parser);
        if (!rule_nonvoid_function_body(parser)){
            return false;
        }
        if (!is_type(parser, TOK_RCURLYBRACKET)){
            return false;
        }
        load_token(parser);
        return true;
    }
}

// <nonvoid_function_body> -> <statement_list> | ε

bool rule_nonvoid_function_body(parser_t *parser){
    if (rule_statement_list(parser)){
        return true;
    }
    else if (is_type(parser, TOK_RCURLYBRACKET)){
        return true;
    }

    return false;
}

// <void_function_body> -> <statement_list> | ε

bool rule_void_function_body(parser_t *parser){
    if (rule_statement_list(parser)){
        return true;
    }
    else if (is_type(parser, TOK_RCURLYBRACKET)){
        return true;
    }

    return false;
}

// <parameter_list> -> <parameter> <more_parameters> | ε

bool rule_parameter_list(parser_t *parser){
    if (rule_parameter(parser)){
        if (rule_more_parameters(parser)){
            return true;
        }
    }
    else if (is_type(parser, TOK_RBRACKET)){
        return true;
    }

    return false;
}

// <more_parameters> -> "," <parameter> <more_parameters> | ε

bool rule_more_parameters(parser_t *parser){
    if (!is_type(parser, TOK_COMMA)){
        return false;
    }
    load_token(parser);
    if (!rule_parameter(parser)){
        return false;
    }
    if (!rule_more_parameters(parser)){
        return false;
    }
    return true;
}

// <parameter> -> <no_name_parameter> | <identifier_parameters>

bool rule_parameter(parser_t *parser){
    if (rule_no_name_parameter(parser)){
        return true;
    }
    else if (rule_identifier_parameter(parser)){
        return true;
    }

    return false;
}

// <no_name_parameter> -> "_" <identifier> ":" <type>

bool rule_no_name_parameter(parser_t *parser){
    if (!is_type(parser, TOK_UNDERSCORE)){
        return false;
    }
    load_token(parser);
    if (!is_type(parser, TOK_IDENTIFIER)){
        return false;
    }
    load_token(parser);
    if (!is_type(parser, TOK_COLON)){
        return false;
    }
    load_token(parser);
    if (!rule_type(parser)){
        return false;
    }

    return true;
}

// <identifier_parameter> -> <identifier> <rest_of_identifier_parameter>

bool rule_identifier_parameter(parser_t *parser){
    if (is_type(parser, TOK_IDENTIFIER)){
        load_token(parser);
        if (rule_rest_of_identifier_parameter(parser)){
            return true;
        }
    }

    return false;
}

// <rest_of_identifier_parameter> -> "_" ":" <type> |  <identifier> ":" <type>

bool rule_rest_of_identifier_parameter(parser_t *parser){
    
    if (is_type(parser, TOK_UNDERSCORE)){
        load_token(parser);
        if (!is_type(parser, TOK_COLON)){
            return false;
        }
        load_token(parser);
        if (!rule_type(parser)){
            return false;
        }
        return true;
    }
    else if (is_type(parser, TOK_IDENTIFIER)){
        load_token(parser);
        if (!is_type(parser, TOK_COLON)){
            return false;
        }
        if (!rule_type(parser)){
            return false;
        }
        return true;
    }

    return false;
        
}

// <type> -> "Double" | "Int" | "String" | "Double?" | "Int?" | "String?"

bool rule_type(parser_t *parser){
    switch(parser->token.type){
        case K_DOUBLE:
        case K_INT:
        case K_STRING:
        case K_DOUBLEQ:
        case K_INTQ:
        case K_STRINGQ:
            return true;
        default:
            return false;
    }
}

// <variable_definition_let> -> "let" <identifier> <definition_types> (pozn. let je NEMODIFIKOVATELNA promenna)

bool rule_variable_definition_let(parser_t *parser){
    if (!is_type(parser, K_LET)){
        return false;
    }
    load_token(parser);
    if (!is_type(parser, TOK_IDENTIFIER)){
        return false;
    }
    load_token(parser);
    if (!rule_definition_types(parser)){
        return false;
    }

    return true;
}

// <variable_definition_var> -> "var" <identifier> <definition_types>

bool rule_variable_definition_var(parser_t *parser){
    if (!is_type(parser, K_VAR)){
        return false;
    }
    load_token(parser);
    if (!is_type(parser, TOK_IDENTIFIER)){
        return false;
    }
    load_token(parser);
    if (!rule_definition_types(parser)){
        return false;
    }

    return true;
}

// <definition_types> -> <type_def> | <initialization>

bool rule_definition_types(parser_t *parser){
    if (rule_type_def(parser)){
        return true;
    }
    else if (rule_initialization(parser)){
        return true;
    }

    return false;
}

// <type_def> -> ":" <type> <type_def_follow>

bool rule_type_def(parser_t *parser){
    if (!is_type(parser, TOK_COLON)){
        return false;
    }
    load_token(parser);
    if (!rule_type(parser)){
        return false;
    }
    load_token(parser);
    if (!rule_type_def_follow(parser)){
        return false;
    }

    return true;
}

// <type_def_follow> -> <initialization> | ε
// TODO: Check correctness here, this might not be correct, I am not sure how to check for ε

bool rule_type_def_follow(parser_t *parser){
    if (rule_initialization(parser)){
        return true;
    }
    else return true;
}

// <initialization> -> "=" <expression>

bool rule_initialization(parser_t *parser){

    if (is_type(parser, TOK_ASSIGNMENT)){
        load_token(parser);
        
        exp_parsing(parser);
        if (error != ERR_OK){
            return false;
        }
        return true;
    }

    return false;
}

// <assignment> -> <identifier> "=" <assignment_type>

bool rule_assignment(parser_t *parser){
    if (!is_type(parser, TOK_IDENTIFIER)){
        return false;
    }
    load_token(parser);
    if (!is_type(parser, TOK_ASSIGNMENT)){
        return false;
    }
    load_token(parser);
    if (!rule_assignment_type(parser)){
        return false;
    }

    return true;
}

// <assignment_type> -> <function_call> | <expression>

bool rule_assignment_type(parser_t *parser){
    if(is_type(parser, TOK_IDENTIFIER) && is_type_next(parser, TOK_LBRACKET)){
        return rule_function_call(parser);
    }
    else{
        exp_parsing(parser);
        if (error != ERR_OK){
            return false;
        }
        return true;
    }
}

// <conditional_statement> -> if <if_statement>

bool rule_conditional_statement(parser_t *parser){
    if (!is_type(parser, K_IF)){
        return false;
    }
    load_token(parser);
    if (!rule_if_statement(parser)){
        return false;
    }

    return true;
}

// <if_statement> -> <classical_statement> | <variable_statement>

bool rule_if_statement(parser_t *parser){
    if (rule_classical_statement(parser)){
        return true;
    }
    else if (rule_variable_statement(parser)){
        return true;
    }

    return false;
}

// <classical_statement> -> <expression> "{" <statement_list>  "}" "else" "{" <statement_list> "}"

bool rule_classical_statement(parser_t *parser){

    exp_parsing(parser);
    if (error != ERR_OK){
        return false;
    }
    if (!is_type(parser, TOK_LCURLYBRACKET)){
        return false;
    }
    load_token(parser);
    if (!rule_statement_list(parser)){
        return false;
    }
    if (!is_type(parser, TOK_RCURLYBRACKET)){
        return false;
    }
    load_token(parser);
    if (!is_type(parser, K_ELSE)){
        return false;
    }
    load_token(parser);
    if (!is_type(parser, TOK_LCURLYBRACKET)){
        return false;
    }
    load_token(parser);
    if (!rule_statement_list(parser)){
        return false;
    }
    if (!is_type(parser, TOK_RCURLYBRACKET)){
        return false;
    }
}

// <variable_statement> -> let <identifier> "{" <statement_list> "}" "else" "{" <statement_list> "}" (pozn. identifier zastupuje drive def. nemodifikovatelnou promennou)

bool rule_variable_statement(parser_t *parser){
    if (!is_type(parser, K_LET)){
        return false;
    }
    load_token(parser);
    if (!is_type(parser, TOK_IDENTIFIER)){
        return false;
    }
    load_token(parser); // Somewhere around here we will check if the variable is defined
    if (!is_type(parser, TOK_LCURLYBRACKET)){
        return false;
    }
    load_token(parser);
    if (!rule_statement_list(parser)){
        return false;
    }
    if (!is_type(parser, TOK_RCURLYBRACKET)){
        return false;
    }
    load_token(parser);
    if (!is_type(parser, K_ELSE)){
        return false;
    }
    load_token(parser);
    if (!is_type(parser, TOK_LCURLYBRACKET)){
        return false;
    }
    load_token(parser);
    if (!rule_statement_list(parser)){
        return false;
    }
    if (!is_type(parser, TOK_RCURLYBRACKET)){
        return false;
    }
    load_token(parser);

    return true;
}

// <loop> -> "while" <expression> "{" <statement_list> "}"

bool rule_loop(parser_t *parser){
    if (!is_type(parser, K_WHILE)){
        return false;
    }
    load_token(parser);
    
    exp_parsing(parser);

    if (error != ERR_OK){
        return false;
    }

    if (!is_type(parser, TOK_LCURLYBRACKET)){
        return false;
    }
    load_token(parser);
    if (!rule_statement_list(parser)){
        return false;
    }
    if (!is_type(parser, TOK_RCURLYBRACKET)){
        return false;
    }
    load_token(parser);

    return true;
}

// <function_call> -> <identifier> "(" <arguments> ")"

bool rule_function_call(parser_t *parser){
    if (!is_type(parser, TOK_IDENTIFIER)){
        return false;
    }
    load_token(parser);
    if (!is_type(parser, TOK_LBRACKET)){
        return false;
    }
    load_token(parser);
    if (!rule_arguments(parser)){
        return false;
    }
    if (!is_type(parser, TOK_RBRACKET)){
        return false;
    }
    load_token(parser);

    return true;
}

// <arguments> -> <expression> <more_arguments> | ε

bool rule_arguments(parser_t *parser){

    if(is_type(parser, TOK_RBRACKET)){
        return true;
    }

    exp_parsing(parser);
    if (error != ERR_OK){
        return false;
    }

    if (!rule_more_arguments(parser)){
        return false;
    }

    return true;
}

// <more_arguments> -> "," <expression> <more_arguments> | ε

bool rule_more_arguments(parser_t *parser){
    if(is_type(parser, TOK_RBRACKET)){
        return true;
    }
    else if (is_type(parser, TOK_COMMA)){
        load_token(parser);
        exp_parsing(parser);
        if (error != ERR_OK){
            return false;
        }
        if (!rule_more_arguments(parser)){
            return false;
        }
    }

    return true;
}

// <return_statement> -> "return" <returned_expression>

bool rule_return_statement(parser_t *parser){
    if (!is_type(parser, K_RETURN)){
        return false;
    }
    load_token(parser);
    if (!rule_returned_expression(parser)){
        return false;
    }

    return true;
}


//================= GRAMMAR RULES END HERE ================= //