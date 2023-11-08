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

bool is_type(parser_t *parser, token_type_t type) {
    return parser->token.type == type;
}


void parser_init(parser_t *parser, scanner_t *scanner)
{
    parser->scanner = scanner;
    parser->stack = malloc(sizeof(stack_t));
    stack_init(parser->stack);
    parser->function = NULL;
}

bool rule_program(parser_t *parser)
{
    if (is_type(parser, TOK_EOF)) { // je toto ok?
        return true;
    }

    return rule_statement(parser) && rule_program(parser);
}

void parser_destroy(parser_t *parser)
{
    stack_free(parser->stack); // TODO: funkci na uvolneni stacku s tabulkama
}