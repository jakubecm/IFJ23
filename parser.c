/**
 *  @file parser.c
 * 
 * 
 *  @authors Milan Jakubec (xjakub41)
 *  @authors Jakub Ráček (xracek12)
 */

#include "parser.h"

/** LL - gramatika
<program> -> <statement> <program> | ε

<statement> -> <variable_definition_let> | <variable_definition_var> | <assignment> | <conditional_statement> | <loop> | <function_call> | <function_definition> | <return_statement>

<function_definition> -> "func" <identifier> "(" <parameter_list> ")" <function_return_type> "{" <program> "}"

<function_return_type> -> ε | <nonvoid_function>

<nonvoid_function> -> "->" <type>

<parameter_list> -> <parameter> <more_parameters> | ε

<more_parameters> -> "," <parameter> <more_parameters> | ε

<parameter> -> <no_name_parameter> | <no_id_parameter> | <all_parameters>

<no_name_parameter> -> "_" <identifier> ":" <type>

<no_id_parameter> -> <identifier> "_" ":" <type>

<all_parameters> -> <identifier> <identifier> ":" <type>

<type> -> "Double" | "Int" | "String" | "Double?" | "Int?" | "String?"

<variable_definition_let> -> "let" <identifier> <definition_types> (pozn. let je NEMODIFIKOVATELNA promenna)

<variable_definition_var> -> "var" <identifier> <definition_types>

<definition_types> -> <type_def> | <initialization>

<type_def> -> ":" <type> <initialization>

<initialization> -> "=" <expression> | ε

<assignment> -> <identifier> "=" <expression>

<conditional_statement> -> if <if_statement>

<if_statement> -> <classical_statement> | <variable_statement>

<classical_statement> -> <expression> "{" <program> "}" "else" "{" <program> "}"

<variable_statement> -> let <identifier> "{" <program> "}" "else" "{" <program> "}" (pozn. identifier zastupuje drive definovanou nemodifikovatelnou promennou)

<loop> -> "while" <expression> "{" <program> "}"

<function_call> -> <identifier> "(" <arguments> ")"

<arguments> -> <expression> <more_arguments> | ε

<more_arguments> -> "," <expression> <more_arguments> | ε

<return_statement> -> "return" <returned_expression>

<returned_expression> -> <expression> | ε
*/

void parser_init(parser_t *parser, scanner_t *scanner)
{
    parser->scanner = scanner;
    parser->stack = malloc(sizeof(stack_t));
    stack_init(parser->stack);
    parser->function = NULL;

    parser->token = get_next_token();
    parser->next_token = get_next_token();
}

void parser_destroy(parser_t *parser)
{
    stack_free(parser->stack); // TODO: funkci na uvolneni stacku s tabulkama
}

bool is_type(parser_t *parser, token_type_t type) {
    return parser->token.type == type;
}

bool is_type_next(parser_t *parser, token_type_t type) {
    return parser->next_token.type == type;
}

bool load_token(parser_t *parser) {
    parser->token = parser->next_token;
    if (is_type(parser, TOK_EOF)) {
        return false;
    }
    parser->next_token = get_next_token();
}

bool rule_program(parser_t *parser)
{
    if (is_type(parser, TOK_EOF)) { // je toto ok?
        return true;
    }

    return rule_statement(parser) && rule_program(parser);
}

bool rule_statement(parser_t *parser) {
    switch (parser->token.type) {
    case K_LET:
        return rule_variable_definition_let(parser);
        break;
    
    case K_VAR:
        return rule_variable_definition_var(parser);
        break;

    case TOK_IDENTIFIER:
        if (is_type_next(parser, TOK_LBRACKET)) {
            return rule_function_call(parser);
        }
        return rule_assignment(parser);
        break;

    case K_IF:
        return rule_conditional_statement(parser);
        break;

    case K_WHILE:
        return rule_loop(parser);
        break;

    case K_FUNC:
        return rule_function_definition(parser);
        break;

    case K_RETURN:
        return rule_return_statement(parser);
        break;
    
    default:
        break;
    }
    
}

bool rule_variable_definition_let(parser_t *parser) {}

bool rule_variable_definition_var(parser_t *parser) {}

bool rule_assignment(parser_t *parser) {}

bool rule_conditional_statement(parser_t *parser) {}

bool rule_loop(parser_t *parser) {}

bool rule_function_call(parser_t *parser) {}

bool rule_function_definition(parser_t *parser) {
    if (!is_type_next(parser, TOK_IDENTIFIER)) {  
        return false;                               
    }
    load_token(parser); // identifier | (

    data_t func;
    func.name = parser->token.attribute.string;

    load_token(parser); // ( | param list

    if (!is_type(parser, TOK_LBRACKET)) {
        return false;
    }

    load_token(parser); // param list | )

    if (!rule_parameter_list(parser)) {
        return false;
    }

    load_token(parser); // ) | return type

    if (!is_type(parser, TOK_RBRACKET)) {
        return false;
    }

    load_token(parser); // return type | {

    if (!rule_function_return_type(parser)) {
        return false;
    }

    load_token(parser); // { | program

    if (!is_type(parser, TOK_LCURLYBRACKET)) {
        return false;
    }

    load_token(parser); // program | }

    if (!rule_program(parser)) {
        return false;
    }

    load_token(parser); // } | ???

    if (!is_type(parser, TOK_RCURLYBRACKET)) {
        return false;
    }

    load_token(parser); // ??? | ???

    return true;
}

bool rule_return_statement(parser_t *parser) {}

bool rule_parameter_list(parser_t *parser) {}