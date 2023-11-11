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
#include "stack.h"

/** LL - gramatika
<program> -> <statement> <program> | ε

<statement> -> <variable_definition_let> | <variable_definition_var> | <assignment> | <conditional_statement> | <loop> |
<function_call> | <function_definition> | <return_statement>

<function_definition> -> "func" <identifier> "(" <parameter_list> ")" <function_return_type> "{" <program> "}"

<function_return_type> -> ε | <nonvoid_function>

<nonvoid_function> -> "->" <type>

<parameter_list> -> <parameter> <more_parameters> | ε

<more_parameters> -> "," <parameter> <more_parameters> | ε

<parameter> -> <no_name_parameter> | <identifier_parameters>

<no_name_parameter> -> "_" <all_parameters>

<identifier_parameter> -> <identifier> <rest_of_identifier_parameter>

<rest_of_identifier_parameter> -> <no_id_parameter> | <all_parameters>

<no_id_parameter> -> "_" ":" <type>

<all_parameters> -> <identifier> ":" <type>

<type> -> "Double" | "Int" | "String" | "Double?" | "Int?" | "String?"

<variable_definition_let> -> "let" <identifier> <definition_types> (pozn. let je NEMODIFIKOVATELNA promenna)

<variable_definition_var> -> "var" <identifier> <definition_types>

<definition_types> -> <type_def> | <initialization>

<type_def> -> ":" <type> <type_def_follow>

<type_def_follow> -> <initialization> | ε

<initialization> -> "=" <expression>

<assignment> -> <identifier> "=" <expression>   TODO: Repair so assignment can derive to exp or a function call

<conditional_statement> -> if <if_statement>

<if_statement> -> <classical_statement> | <variable_statement>

<classical_statement> -> <expression> "{" <program> "}" "else" "{" <program> "}"

<variable_statement> -> let <identifier> "{" <program> "}" "else" "{" <program> "}" (pozn. identifier zastupuje drive
definovanou nemodifikovatelnou promennou)

<loop> -> "while" <expression> "{" <program> "}"

<function_call> -> <identifier> "(" <arguments> ")"

<arguments> -> <expression> <more_arguments> | ε

<more_arguments> -> "," <expression> <more_arguments> | ε

<return_statement> -> "return" <returned_expression>

<returned_expression> -> <expression> | ε
*/

bool rule_program(parser_t *parser);
bool rule_statement(parser_t *parser);
bool rule_function_definition(parser_t *parser);
bool rule_function_return_type(parser_t *parser);
bool nonvoid_function(parser_t *parser);
bool rule_parameter_list(parser_t *parser);
bool rule_more_parameters(parser_t *parser);
bool rule_parameter(parser_t *parser);
bool rule_identifier_parameter(parser_t *parser);
bool rule_rest_of_identifier_parameter(parser_t *parser);
bool rule_no_name_parameter(parser_t *parser);
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

void parser_init(parser_t *parser, scanner_t *scanner) {
    parser->scanner = scanner;
    parser->stack = malloc(sizeof(stack_t));
    stack_init(parser->stack);
    parser->function = NULL;

    parser->token = get_next_token();
    // Nebude lepsi udelat funkci ktera jen peekne na dalsi token a scanner ho uchova misto tohoto?
    parser->next_token = get_next_token();
}

void parser_destroy(parser_t *parser) {
    stack_free(parser->stack);  // TODO: funkci na uvolneni stacku s tabulkama
}

bool is_type(parser_t *parser, token_type_t type) { return parser->token.type == type; }

bool is_type_next(parser_t *parser, token_type_t type) { return parser->next_token.type == type; }

void load_token(parser_t *parser) {
    parser->token = parser->next_token;
    parser->next_token = get_next_token();
}

//================= GRAMMAR RULES START HERE ================= //

// Note: pls implement these rules in the order they are in the grammar otherwise we will get lost

bool rule_program(parser_t *parser) {
    if (is_type(parser, TOK_EOF)) {  // is this ok?
        // life update: this is probably ok
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

        default:
            return false;
            break;
    }
}

bool rule_function_definition(parser_t *parser) {
    if (!is_type_next(parser, TOK_IDENTIFIER)) {
        return false;
    }
    load_token(parser);  // identifier | (

    //data_t func;
    //func.name = parser->token.attribute.string;

    load_token(parser);  // ( | param list

    if (!is_type(parser, TOK_LBRACKET)) {
        return false;
    }


    load_token(parser);  // param list OR )

    if (!rule_parameter_list(parser)) {
        return false;
    }

    if (!is_type(parser, TOK_RBRACKET))  // param list was not empty
    {
        load_token(parser);  // ) | return type
    }

    if (!is_type(parser, TOK_RBRACKET)) {
        return false;
    }

    load_token(parser);  // return type | {

    if (!rule_function_return_type(parser)) {
        return false;
    }

    load_token(parser);  // { | program

    if (!is_type(parser, TOK_LCURLYBRACKET)) {
        return false;
    }

    load_token(parser);  // program | }

    if (!rule_program(parser)) {
        return false;
    }

    load_token(parser);  // } | ???

    if (!is_type(parser, TOK_RCURLYBRACKET)) {
        return false;
    }

    load_token(parser);  // ??? | ???

    return true;
}

bool rule_function_return_type(parser_t *parser)  // pls do not mix 2 rules together, it is confusing, better to follow LL grammar
{
    if (is_type(parser, TOK_LCURLYBRACKET)) {
        return true;  // Successfully parsed an epsilon production
    } else {
        return nonvoid_function(parser);
    }
}

bool nonvoid_function(parser_t *parser) {
    if (!is_type(parser, TOK_ARROW)) {
        return false;
    }

    load_token(parser);
    return rule_type(parser);
}

bool rule_parameter_list(parser_t *parser) {
    if (is_type(parser, TOK_RBRACKET)) {
        return true;  // function has no params
    }

    return rule_parameter(parser) && rule_more_parameters(parser);
}

bool rule_more_parameters(parser_t *parser) {
    if (is_type(parser, TOK_RBRACKET)) {
        return true;  // no more params
    }

    if (is_type(parser, TOK_COMMA)) {
        load_token(parser); // identifier OR _ | identifier OR _

        return rule_parameter(parser) && rule_more_parameters(parser);
    }
    return false;  // Neither comma nor ), not a valid parameter list
}

bool rule_parameter(parser_t *parser) {
    if (is_type(parser, TOK_UNDERSCORE)) {
        return rule_no_name_parameter(parser);
    } else if (is_type(parser, TOK_IDENTIFIER)) {
        return rule_identifier_parameter(parser);
    } else {
        return false;  // Neither _ nor identifier, not a valid parameter
    }
}

bool rule_identifier_parameter(parser_t *parser) {
    load_token(parser);  // rest of identifier parameter | :

    return rule_rest_of_identifier_parameter(parser);
}

bool rule_rest_of_identifier_parameter(parser_t *parser) {
    if (is_type(parser, TOK_UNDERSCORE)) {
        return rule_no_id_parameter(parser);
    } else if (is_type(parser, TOK_IDENTIFIER)) {
        return rule_all_parameters(parser);
    } else {
        return false;  // Neither _ nor id, not a valid parameter
    }
}

bool rule_no_name_parameter(parser_t *parser) {
    load_token(parser);  // identifier | :

    if (!is_type(parser, TOK_IDENTIFIER)) {
        return false; // for named parametres we check this in rule_rest_of_identifier_parameter
    }
    return rule_all_parameters(parser);
}

bool rule_no_id_parameter(parser_t *parser) {
    load_token(parser);  // : | type

    if (!is_type(parser, TOK_COLON)) {
        return false;
    }

    load_token(parser);  // type | ???

    return rule_type(parser);
}

bool rule_all_parameters(parser_t *parser) {
    load_token(parser);  // : | type

    if (!is_type(parser, TOK_COLON)) {
        return false;
    }

    load_token(parser);  // type | ???

    return rule_type(parser);
}

bool rule_type(parser_t *parser) {
    switch (parser->token.type) {
        case K_DOUBLE:
        case K_INT:
        case K_STRING:
        case K_DOUBLEQ:
        case K_INTQ:
        case K_STRINGQ:
            return true;
            break;

        default:
            return false;
            break;
    }
}

bool rule_variable_definition_let(parser_t *parser) {
    load_token(parser);

    // If next token is not identifier, it is not a valid variable definition
    if (!is_type(parser, TOK_IDENTIFIER)) {
        return false;
    }

    //char *variable_name = parser->token.attribute.string;
    // We'll have to add it to the symbol table later

    return rule_definition_types(parser);
}

bool rule_variable_definition_var(parser_t *parser) {
    load_token(parser);

    // If next token is not identifier, it is not a valid variable definition
    if (!is_type(parser, TOK_IDENTIFIER)) {
        return false;
    }

    //char *variable_name = parser->token.attribute.string;
    // We'll have to add it to the symbol table later

    return rule_definition_types(parser);
}

bool rule_definition_types(parser_t *parser) {
    load_token(parser);

    if (is_type(parser, TOK_COLON)) {
        return rule_type_def(parser);
    } else if (is_type(parser, TOK_ASSIGNMENT)) {
        return rule_initialization(parser);
    } else {
        return false;  // If it's not colon or assignment, it's not a valid variable definition
    }
}

bool rule_type_def(parser_t *parser) {
    load_token(parser);  // Consume the colon

    if (rule_type(parser)) {
        return rule_type_def_follow(parser);
    } else {
        return false;  // If it's not a type, it's not a valid variable definition
    }
}

bool rule_type_def_follow(parser_t *parser) {
    load_token(parser);  // Consume the type

    if (is_type(parser, TOK_ASSIGNMENT)) {
        return rule_initialization(parser);
    } else {
        return true;  // If it's not assignment, it's an epsilon production
    }
}

bool rule_initialization(parser_t *parser) {
    return true;
    //return (exp_parsing(parser) != -1) ? true : false;  // Not sure if this is the right way to do it lol
}

bool rule_assignment(parser_t *parser) {
    load_token(parser);

    if (!is_type(parser, TOK_ASSIGNMENT)) {
        return false;
    }

    load_token(parser);

    exp_parsing(parser);  // Again not sure if this is the right way to do it

    if (error != ERR_OK) {
        return false;
    }

    return true;
}

bool rule_conditional_statement(parser_t *parser) {
    load_token(parser);

    return rule_if_statement(parser);
}

bool rule_if_statement(parser_t *parser) {
    if (is_type(parser, K_LET)) {
        return rule_variable_statement(parser);
    } else {
        return rule_classical_statement(parser);
    }
}

bool rule_classical_statement(parser_t *parser) {
    // Parse the expression
    exp_parsing(parser);

    if (error != ERR_OK) {
        return false;
    }

    // Expect and handle the '{' token
    if (!is_type(parser, TOK_LCURLYBRACKET)) {
        return false;
    }
    load_token(parser);  // Move past '{'

    // Parse the program within the if block
    if (!rule_program(parser)) {
        return false;
    }

    // Expect and handle the '}' token
    // Note: Not really sure if the token is already loaded here or if i need to load it beforehand, check pls
    if (!is_type(parser, TOK_RCURLYBRACKET)) {
        return false;
    }
    load_token(parser);  // Move past '}'

    // Expect and handle the 'else' keyword
    if (!is_type(parser, K_ELSE)) {
        return false;
    }
    load_token(parser);  // Move past 'else'

    // Expect and handle the '{' token for the else block
    if (!is_type(parser, TOK_LCURLYBRACKET)) {
        return false;
    }
    load_token(parser);  // Move past '{'

    // Parse the program within the else block
    if (!rule_program(parser)) {
        return false;
    }

    // Expect and handle the '}' token
    if (!is_type(parser, TOK_RCURLYBRACKET)) {
        return false;
    }
    load_token(parser);  // Move past '}'

    return true;
}

bool rule_variable_statement(parser_t *parser) {
    load_token(parser);  // Move past 'let'

    // Expect and handle the identifier
    if (!is_type(parser, TOK_IDENTIFIER)) {
        return false;
    }

    //char *variable_name = parser->token.attribute.string;
    // TODO: Check if the variable is in the symtable and is unmodifiable (should be as it is let)

    load_token(parser);  // Move past the identifier

    // Expect and handle the '{' token
    if (!is_type(parser, TOK_LCURLYBRACKET)) {
        return false;
    }
    load_token(parser);  // Move past '{'

    // Parse the program within the if block
    if (!rule_program(parser)) {
        return false;
    }

    // Expect and handle the '}' token
    if (!is_type(parser, TOK_RCURLYBRACKET)) {
        return false;
    }
    load_token(parser);  // Move past '}'

    // Expect and handle the 'else' keyword
    if (!is_type(parser, K_ELSE)) {
        return false;
    }
    load_token(parser);  // Move past 'else'

    // Expect and handle the '{' token for the else block
    if (!is_type(parser, TOK_LCURLYBRACKET)) {
        return false;
    }
    load_token(parser);  // Move past '{'

    // Parse the program within the else block
    if (!rule_program(parser)) {
        return false;
    }

    // Expect and handle the '}' token
    if (!is_type(parser, TOK_RCURLYBRACKET)) {
        return false;
    }
    load_token(parser);  // Move past '}'

    return true;
}

bool rule_loop(parser_t *parser) {
    load_token(parser);  // Move past 'while'

    // Parse the expression
    exp_parsing(parser);

    if (error != ERR_OK) {
        return false;
    }

    // Expect and handle the '{' token
    if (!is_type(parser, TOK_LCURLYBRACKET)) {
        return false;
    }
    load_token(parser);  // Move past '{'

    // Parse the program within the loop block
    if (!rule_program(parser)) {
        return false;
    }

    // Expect and handle the '}' token
    if (!is_type(parser, TOK_RCURLYBRACKET)) {
        return false;
    }
    load_token(parser);  // Move past '}'

    return true;
}

bool rule_function_call(parser_t *parser) {
    load_token(parser);  // Move past the identifier, now we are at '(' and we know it
    load_token(parser);  // Move past '('

    if (rule_arguments(parser)) {
        load_token(parser);  // Move past ')'
        return true;
    } else {
        return false;
    }
}

bool rule_arguments(parser_t *parser) {
    if (is_type(parser, TOK_RBRACKET)) {
        return true;  // Successfully parsed an epsilon
    }

    exp_parsing(parser);

    if (error != ERR_OK) {
        return false;
    } else {
        return rule_more_arguments(parser);
    }
}

bool rule_more_arguments(parser_t *parser) {
    load_token(parser);  // Move past the expression

    if (is_type(parser, TOK_RBRACKET)) {
        return true;  // Successfully parsed an epsilon
    } else if (is_type(parser, TOK_COMMA)) {
        load_token(parser);  // Move past ','
        exp_parsing(parser);

        if (error != ERR_OK) {
            return false;
        } else {
            return rule_more_arguments(parser);
        }
    } else {
        return false;  // Neither comma nor ), not a valid argument list
    }
}

// To discuss: return statement and returned experssion, needs reviewing, working on it