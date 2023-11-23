/**
 *  @file parser.c
 *
 *  @todo FIX FUNCTION CALL LL GRAMMAR
 *  @todo CHECK ERROR CODES
 *  @todo CHECK IN WITH PSYCHIATRIST
 *
 *  @authors Milan Jakubec (xjakub41)
 *  @authors Jakub Ráček (xracek12)
 */

#include "parser.h"

#include "error.h"
#include "expression.h"
#include <string.h>

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

<initialization> -> "=" <expression> | "=" <function_call>

<assignment> -> <identifier> "=" <assignment_type>

<assignment_type> -> <function_call> | <expression>

<conditional_statement> -> if <if_statement>

<if_statement> -> <classical_statement> | <variable_statement>

<classical_statement> -> <expression> "{" <statement_list>  "}" "else" "{" <statement_list> "}"

<variable_statement> -> let <identifier> "{" <statement_list> "}" "else" "{" <statement_list> "}" (pozn. identifier zastupuje drive
definovanou nemodifikovatelnou promennou)

<loop> -> "while" <expression> "{" <statement_list> "}"

<function_call> -> <identifier> "(" <arguments> ")"

<arguments> -> <argument> <more_arguments> | ε

<argument> -> <arg_name> <arg_value>

<arg_name> -> <identifier> ":" | ε

<arg_value> -> <identifier> | <int> | <double> | <string> | <nil>

<more_arguments> -> "," <argument> <more_arguments> | ε

<return_statement> -> "return" <returned_expression>

<returned_expression> -> <expression> | ε

================== LL GRAMMAR END ================== */

//================= FUNCTION DECLARATIONS START HERE ================= //
bool rule_program(parser_t *parser);
bool rule_statement_list(parser_t *parser);
bool rule_statement(parser_t *parser);
bool rule_function_definition(parser_t *parser);
bool rule_function_return_type_and_body(parser_t *parser, data_t *data);
bool rule_nonvoid_function(parser_t *parser);
bool rule_nonvoid_function_body(parser_t *parser);
bool rule_void_function_body(parser_t *parser);
bool rule_statement_nonvoid_function(parser_t *parser);
bool rule_statement_void_function(parser_t *parser);
bool rule_parameter_list(parser_t *parser, data_t *data);
bool rule_more_parameters(parser_t *parser, data_t *data);
bool rule_parameter(parser_t *parser, data_t *data);
bool rule_no_name_parameter(parser_t *parser, data_t *data);
bool rule_identifier_parameter(parser_t *parser, data_t *data);
bool rule_rest_of_identifier_parameter(parser_t *parser, data_t *data);
bool rule_no_id_parameter(parser_t *parser);
bool rule_all_parameters(parser_t *parser);
bool rule_type(parser_t *parser);
bool rule_variable_definition_let(parser_t *parser);
bool rule_variable_definition_var(parser_t *parser);
bool rule_definition_types(parser_t *parser, data_t *data);
bool rule_type_def(parser_t *parser, data_t *data);
bool rule_type_def_follow(parser_t *parser, data_t *data);
bool rule_initialization(parser_t *parser, data_t *data);
bool rule_assignment(parser_t *parser, data_t *data);
bool rule_assignment_type(parser_t *parser, data_t *data);
bool rule_conditional_statement(parser_t *parser);
bool rule_if_statement(parser_t *parser);
bool rule_classical_statement(parser_t *parser);
bool rule_variable_statement(parser_t *parser);
bool rule_loop(parser_t *parser);
bool rule_function_call(parser_t *parser, data_t *data);
bool rule_arguments(parser_t *parser, int argnum, int *argindex, data_t *data);
bool rule_argument(parser_t *parser, int *argindex, data_t *data);
bool rule_arg_name(parser_t *parser, int *argindex, data_t *data);
bool rule_arg_value(parser_t *parser, int *argindex, data_t *data);
bool rule_more_arguments(parser_t *parser, int argnum, int *argindex, data_t *data);
bool rule_return_statement(parser_t *parser);
bool rule_returned_expression(parser_t *parser);
bool rule_empty_return_statement(parser_t *parser);
//================= FUNCTION DECLARATIONS END HERE ================= //

//================= PARSER FUNCTIONS ================= //

void parser_init(parser_t *parser, gen_t *gen)
{
    parser->stack = malloc(sizeof(stack_t));
    stack_init(parser->stack);
    parser->function = NULL;
    parser->in_function = false;
    parser->func_is_void = false;
    parser->returned = false;

    stack_push_table(parser->stack);

    parser->token = get_next_token();
    parser->next_token = get_next_token();
    parser->gen = gen;
    gen_init(gen);
}

void parser_destroy(parser_t *parser)
{
    stack_destroy_table(parser->stack);
    stack_free(parser->stack);
}

void run_parser(parser_t *parser)
{
    if(!rule_program(parser)){
        print_error_and_exit(error);
    }
}

bool is_type(parser_t *parser, token_type_t type) { return parser->token.type == type; }

bool is_type_next(parser_t *parser, token_type_t type) { return parser->next_token.type == type; }

void load_token(parser_t *parser)
{
    parser->token = parser->next_token;
    parser->next_token = get_next_token();
}

variable_type_t str_to_type(token_t t){
    if (t.type == K_DOUBLE){
        return VAL_DOUBLE;
    }
    else if (t.type == K_INT){
        return VAL_INT;
    }
    else if (t.type == K_STRING){
        return VAL_STRING;
    }
    else if (t.type == K_DOUBLEQ){
        return VAL_DOUBLEQ;
    }
    else if (t.type == K_INTQ){
        return VAL_INTQ;
    }
    else if (t.type == K_STRINGQ){
        return VAL_STRINGQ;
    }
    else{
        return VAL_UNKNOWN;
    }
}

bool is_expression(parser_t *parser) {
    switch (parser->token.type) {
        case TOK_IDENTIFIER:
        case TOK_INT:
        case TOK_DOUBLE:
        case TOK_STRING:
        case K_NIL:
        case TOK_LBRACKET:
        case TOK_MINUS:
        case TOK_PLUS:
            return true;
        default:
            return false;
    }
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
        return rule_program(parser);
    }
    else if (rule_function_definition(parser)){
        return rule_program(parser);
    }

    return false;
}

// <statement_list> -> <statement> <statement_list> | ε
// Note: statement list is only used in condtional statements and loops, thats why there is a check for RCURLYBRACKET

bool rule_statement_list(parser_t *parser){
    if (is_type(parser, TOK_RCURLYBRACKET) && parser->in_function){
        return true;
    }

    return rule_statement(parser) && rule_statement_list(parser);
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
                return rule_function_call(parser, NULL);
            }
            else{
                return rule_assignment(parser, NULL);
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
    if (parser->in_function) {
        return false;   // Attempt at defining a function within a function
    }
    parser->in_function = true;
    if (!is_type(parser, K_FUNC)){
        return false;
    }
    load_token(parser);
    if (!is_type(parser, TOK_IDENTIFIER)){
        return false;
    }
    data_t *symbol = symbol_table_lookup_generic(stack_top_table(parser->stack), parser->token.attribute.string);
    if (symbol != NULL){
        if (symbol->type == FUNC && !symbol->value.func_id.defined) {
            error = ERR_SEM_FUNCTION;
            return false;   // Attempt at defining an already existing function or global variable
        }
    }

    data_t func;
    func.type = FUNC;
    func.name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1));
    strcpy(func.name, parser->token.attribute.string);
    func.value.func_id.defined = true;
    load_token(parser);
    if (!is_type(parser, TOK_LBRACKET)){
        return false;
    }
    load_token(parser);
    func.value.func_id.parameters = vector_init(5);

    stack_push_table(parser->stack);
    if (!rule_parameter_list(parser, &func)){
        return false;
    }
    if (!is_type(parser, TOK_RBRACKET)){
        return false;
    }
    load_token(parser);
    if (!rule_function_return_type_and_body(parser, &func)){
        return false;
    }
    stack_pop_table(parser->stack);

    return true;
}

// <function_return_type_and_body> -> "{" <void_function_body> "}" | "->" <type> "{" <nonvoid_function_body> "}"

bool rule_function_return_type_and_body(parser_t *parser, data_t *data){
    if (is_type(parser, TOK_ARROW)){
        parser->func_is_void = false;
        load_token(parser);
        data->value.func_id.return_type = str_to_type(parser->token);
        parser->return_type = str_to_type(parser->token);
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

        parser->in_function = false;
        parser->returned = false;
        parser->returned_conditional = false;
        load_token(parser);
        return true;
    }

    parser->func_is_void = true;
    data->value.func_id.return_type = VAL_VOID;
    if (!is_type(parser, TOK_LCURLYBRACKET)){
        return false;
    }
    load_token(parser);

    if (!rule_void_function_body(parser)) {
        return false;
    }

    if (!is_type(parser, TOK_RCURLYBRACKET)) {
        return false;
    }

    parser->in_function = false;
    parser->returned = false;
    parser->returned_conditional = false;
    load_token(parser);
    return true;
}

// <nonvoid_function_body> -> <statement_list> | ε

bool rule_nonvoid_function_body(parser_t *parser){
    parser->in_function = true;
    if (rule_statement_list(parser)){
        return true;
    }
    else if (is_type(parser, TOK_RCURLYBRACKET)){
        parser->in_function = false;
        return true;
    }

    return false;
}

// <void_function_body> -> <statement_list> | ε

bool rule_void_function_body(parser_t *parser){
    parser->in_function = true;
    if (rule_statement_list(parser)){
        return true;
    }
    else if (is_type(parser, TOK_RCURLYBRACKET)){
        parser->in_function = false;
        return true;
    }

    return false;
}

// <parameter_list> -> <parameter> <more_parameters> | ε

bool rule_parameter_list(parser_t *parser, data_t *data){
    if (is_type(parser, TOK_RBRACKET)){
        return true;
    }
    return rule_parameter(parser, data) && rule_more_parameters(parser, data);
}

// <more_parameters> -> "," <parameter> <more_parameters> | ε

bool rule_more_parameters(parser_t *parser, data_t *data){
    load_token(parser);

    if (is_type(parser, TOK_RBRACKET)){
        return true;
    }

    if (!is_type(parser, TOK_COMMA)){
        return false;
    }
    load_token(parser);
    if (!rule_parameter(parser, data)){
        return false;
    }
    if (!rule_more_parameters(parser, data)){
        return false;
    }
    return true;
}

// <parameter> -> <no_name_parameter> | <identifier_parameters>

bool rule_parameter(parser_t *parser, data_t *data){
    vector_push(data->value.func_id.parameters, (htab_func_param_t){}); // Add a new parameter to the vector
    if (rule_no_name_parameter(parser, data)){
        return true;
    }
    return rule_identifier_parameter(parser, data);
}

// <no_name_parameter> -> "_" <identifier> ":" <type>

bool rule_no_name_parameter(parser_t *parser, data_t *data){
    if (!is_type(parser, TOK_UNDERSCORE)){
        return false;
    }
    vector_top(data->value.func_id.parameters)->call_name = malloc(sizeof(char) * 2); // _
    strcpy(vector_top(data->value.func_id.parameters)->call_name, "_");

    load_token(parser);
    if (!is_type(parser, TOK_IDENTIFIER)){
        return false;
    }

    data_t param; // symtable variable
    param.name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1));
    strcpy(param.name, parser->token.attribute.string);
    param.type = VAR;
    param.value.var_id.type = VAL_UNKNOWN;

    vector_top(data->value.func_id.parameters)->def_name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1)); // identifier
    strcpy(vector_top(data->value.func_id.parameters)->def_name, parser->token.attribute.string);
    load_token(parser);
    if (!is_type(parser, TOK_COLON)){
        return false;
    }
    load_token(parser);
    param.value.var_id.type = str_to_type(parser->token); // type
    symbol_table_insert(stack_top_table(parser->stack), param.name, param); // push to table
    vector_top(data->value.func_id.parameters)->parameter.type = str_to_type(parser->token); // type
    return rule_type(parser);
}

// <identifier_parameter> -> <identifier> <rest_of_identifier_parameter>

bool rule_identifier_parameter(parser_t *parser, data_t *data){
    if (!is_type(parser, TOK_IDENTIFIER)){
        return false;
    }
    vector_top(data->value.func_id.parameters)->call_name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1));
    strcpy(vector_top(data->value.func_id.parameters)->call_name, parser->token.attribute.string);
    load_token(parser);
    return rule_rest_of_identifier_parameter(parser, data);
}

// <rest_of_identifier_parameter> -> "_" ":" <type> |  <identifier> ":" <type>

bool rule_rest_of_identifier_parameter(parser_t *parser, data_t *data){
    if (is_type(parser, TOK_UNDERSCORE)){
        vector_top(data->value.func_id.parameters)->def_name = malloc(sizeof(char) * 2);
        strcpy(vector_top(data->value.func_id.parameters)->def_name, "_");
        load_token(parser);
        if (!is_type(parser, TOK_COLON)){
            return false;
        }
        load_token(parser);
        vector_top(data->value.func_id.parameters)->parameter.type = str_to_type(parser->token);
        return rule_type(parser);
    }
    else if (is_type(parser, TOK_IDENTIFIER)){
        vector_top(data->value.func_id.parameters)->def_name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1));
        strcpy(vector_top(data->value.func_id.parameters)->def_name, parser->token.attribute.string);

        data_t param; // symtable variable
        param.name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1));
        strcpy(param.name, parser->token.attribute.string);
        param.type = VAR;
        param.value.var_id.type = VAL_UNKNOWN;

        load_token(parser);
        if (!is_type(parser, TOK_COLON)){
            return false;
        }
        load_token(parser);
        vector_top(data->value.func_id.parameters)->parameter.type = str_to_type(parser->token);
        param.value.var_id.type = str_to_type(parser->token);
        symbol_table_insert(stack_top_table(parser->stack), param.name, param);
        return rule_type(parser);
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

    gen_var_definition(parser->gen, &parser->token, parser->in_function);

    data_t *var = symbol_table_lookup_generic(stack_top_table(parser->stack), parser->token.attribute.string);
    if (var != NULL){
        error = ERR_SEM_FUNCTION;
        return false;   // Attempt at defining an already existing variable or function within the current context
    }

    data_t data;
    data.type = LET;
    data.name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1));
    strcpy(data.name, parser->token.attribute.string);
    //symbol_table_insert(stack_top_table(parser->stack), data.name, data);
    symbol_table_lookup_var(stack_top_table(parser->stack), parser->token.attribute.string);

    load_token(parser);
    if(rule_definition_types(parser, &data)) {
        symbol_table_insert(stack_top_table(parser->stack), data.name, data);
        return true;
    }
    return false;
}

// <variable_definition_var> -> "var" <identifier> <definition_types>

bool rule_variable_definition_var(parser_t *parser){
    if (!is_type(parser, K_VAR)){
        return false;
    }
    load_token(parser);

    if(!is_type(parser, TOK_IDENTIFIER)){
        return false;
    }

    gen_var_definition(parser->gen, &parser->token, parser->in_function);

    data_t *var = symbol_table_lookup_generic(stack_top_table(parser->stack), parser->token.attribute.string);
    if (var != NULL){
        error = ERR_SEM_FUNCTION;
        return false;   // Attempt at defining an already existing variable or function withing the current context
    }

    data_t data;
    data.type = VAR;
    data.name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1));
    strcpy(data.name, parser->token.attribute.string);
    symbol_table_insert(stack_top_table(parser->stack), data.name, data);
    load_token(parser);
    if(rule_definition_types(parser, &data)) {
        symbol_table_insert(stack_top_table(parser->stack), data.name, data);
        return true;
    }
    return false;
}

// <definition_types> -> <type_def> | <initialization>

bool rule_definition_types(parser_t *parser, data_t *data){
    if (rule_type_def(parser, data)){
        return true;
    }

    // No type was provided - add a placeholder type
    data->value.var_id.type = VAL_UNKNOWN;
    return rule_initialization(parser, data);
}

// <type_def> -> ":" <type> <type_def_follow>

bool rule_type_def(parser_t *parser, data_t *data){
    if (!is_type(parser, TOK_COLON)){
        return false;
    }
    load_token(parser);
    if (!rule_type(parser)){
        return false;
    }

    // loaded token is the type of the variable
    data->value.var_id.type = str_to_type(parser->token);

    load_token(parser);
    return rule_type_def_follow(parser, data);
}

// <type_def_follow> -> <initialization> | ε
// TODO: Check correctness here, this might not be correct, I am not sure how to check for ε // test this, hopefully it just works :D

bool rule_type_def_follow(parser_t *parser, data_t *data){
    if (!is_type(parser, TOK_ASSIGNMENT)) {
        return true;
    }

    return rule_initialization(parser, data);
}

// <initialization> -> "=" <expression> | "=" <function_call>

bool rule_initialization(parser_t *parser, data_t *data){

    if (!is_type(parser, TOK_ASSIGNMENT)){
        return false;
    }

    load_token(parser);

    if (is_type(parser, TOK_IDENTIFIER) && is_type_next(parser, TOK_LBRACKET)){
        return rule_function_call(parser, data); // init type in function call
    }

    variable_type_t type = exp_parsing(parser);
    if (type == EXP_ERR) {
        return false;
    }

    switch (type) {
    case VAL_INT:
        if (data->value.var_id.type != VAL_INT && data->value.var_id.type != VAL_INTQ && data->value.var_id.type != VAL_UNKNOWN){
            error = ERR_SEM_TYPE;
            return false;
        }
        if (data->value.var_id.type == VAL_UNKNOWN){
            data->value.var_id.type = VAL_INT;
        }
        data->value.var_id.value.number = parser->token.attribute.number;
        break;
    case VAL_DOUBLE:
        if (data->value.var_id.type != VAL_DOUBLE && data->value.var_id.type != VAL_DOUBLEQ && data->value.var_id.type != VAL_UNKNOWN){
            error = ERR_SEM_TYPE;
            return false;
        }
        if (data->value.var_id.type == VAL_UNKNOWN){
            data->value.var_id.type = VAL_DOUBLE;
        }
        data->value.var_id.value.decimal = parser->token.attribute.decimal;
        break;
    case VAL_STRING:
        if (data->value.var_id.type != VAL_STRING && data->value.var_id.type != VAL_STRINGQ && data->value.var_id.type != VAL_UNKNOWN){
            error = ERR_SEM_TYPE;
            return false;
        }
        if (data->value.var_id.type == VAL_UNKNOWN){
            data->value.var_id.type = VAL_STRING;
        }
        data->value.var_id.value.string = parser->token.attribute.string;
        break;   
    case VAL_NIL:
        if (data->value.var_id.type != VAL_INTQ && data->value.var_id.type != VAL_DOUBLEQ && data->value.var_id.type != VAL_STRINGQ){
            error = ERR_SEM_TYPE;
            return false;
        }
        break;
    case VAL_UNKNOWN:
        if (data->value.var_id.type == VAL_UNKNOWN){
            error = ERR_SEM_TYPE;
            return false;
        }
        break;
    case VAL_BOOL: // what to do with this? answer: nothing, we dont have BOOL types unless you want to implement them (ho right ahead)
        break;
    default:
        break;
    }

    return true;
}

// <assignment> -> <identifier> "=" <assignment_type>

bool rule_assignment(parser_t *parser, data_t *data){
    if (!is_type(parser, TOK_IDENTIFIER)){
        return false;
    }
    load_token(parser);
    if (!is_type(parser, TOK_ASSIGNMENT)){
        return false;
    }
    load_token(parser);
    return rule_assignment_type(parser, data);
}

// <assignment_type> -> <function_call> | <expression>

bool rule_assignment_type(parser_t *parser, data_t *data){
    if(is_type(parser, TOK_IDENTIFIER) && is_type_next(parser, TOK_LBRACKET)){
        return rule_function_call(parser, data);
    }
    else{
        variable_type_t type = exp_parsing(parser);
        if (type == EXP_ERR){
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
    return rule_if_statement(parser);
}

// <if_statement> -> <classical_statement> | <variable_statement>

bool rule_if_statement(parser_t *parser){
    if (is_type(parser, K_LET)){
        return rule_variable_statement(parser);
    }
    return rule_classical_statement(parser);
}

// <classical_statement> -> <expression> "{" <statement_list>  "}" "else" "{" <statement_list> "}"

bool rule_classical_statement(parser_t *parser){

    variable_type_t type = exp_parsing(parser);
    if (type == EXP_ERR){
        return false;
    }
    gen_if(parser->gen);

    if (!is_type(parser, TOK_LCURLYBRACKET)){
        return false;
    }
    parser->in_if = true;
    load_token(parser);
    stack_push_table(parser->stack);
    if (!rule_statement_list(parser)){
        return false;
    }
    stack_pop_table(parser->stack);
    if (!is_type(parser, TOK_RCURLYBRACKET)){
        return false;
    }
    parser->in_if = false;
    load_token(parser);
    if (!is_type(parser, K_ELSE)){
        return false;
    }

    gen_else(parser->gen);

    load_token(parser);
    if (!is_type(parser, TOK_LCURLYBRACKET)){
        return false;
    }
    parser->in_else = true;
    load_token(parser);
    stack_push_table(parser->stack);
    if (!rule_statement_list(parser)){
        return false;
    }
    stack_pop_table(parser->stack);
    if (!is_type(parser, TOK_RCURLYBRACKET)){
        return false;
    }
    parser->in_else = false;
    gen_endif(parser->gen);
    load_token(parser);
    return true;
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

    data_t *data = stack_lookup_var(parser->stack, parser->token.attribute.string);
    if (data->type != LET){
        error = ERR_SEM_FUNCTION;
        return false;   // variable not found
    }
    // je promenna typ_nil?
    //gen_variable_statement(gen, typpromenne)
    load_token(parser);
    if (!is_type(parser, TOK_LCURLYBRACKET)){
        return false;
    }
    parser->in_if = true;
    load_token(parser);
    stack_push_table(parser->stack);
    if (!rule_statement_list(parser)){
        return false;
    }
    stack_pop_table(parser->stack);
    if (!is_type(parser, TOK_RCURLYBRACKET)){
        return false;
    }
    parser->in_if = false;
    load_token(parser);
    if (!is_type(parser, K_ELSE)){
        return false;
    }
    load_token(parser);
    if (!is_type(parser, TOK_LCURLYBRACKET)){
        return false;
    }
    parser->in_else = true;
    load_token(parser);
    stack_push_table(parser->stack);
    if (!rule_statement_list(parser)){
        return false;
    }
    stack_pop_table(parser->stack);
    if (!is_type(parser, TOK_RCURLYBRACKET)){
        return false;
    }
    parser->in_else = false;
    load_token(parser);
    return true;
}

// <loop> -> "while" <expression> "{" <statement_list> "}"

bool rule_loop(parser_t *parser){
    if (!is_type(parser, K_WHILE)){
        return false;
    }
    load_token(parser);
    
    variable_type_t type = exp_parsing(parser);
    if (type == EXP_ERR){
        return false;
    }

    if (!is_type(parser, TOK_LCURLYBRACKET)){
        return false;
    }

    parser->in_cycle = true;
    load_token(parser);
    stack_push_table(parser->stack);
    if (!rule_statement_list(parser)){
        return false;
    }
    stack_pop_table(parser->stack);
    if (!is_type(parser, TOK_RCURLYBRACKET)){
        return false;
    }

    parser->in_cycle = false;
    load_token(parser);
    return true;
}

// <function_call> -> <identifier> "(" <arguments> ")"
// TODO: fix da rules
bool rule_function_call(parser_t *parser, data_t *var){
    if (!is_type(parser, TOK_IDENTIFIER)){
        return false;
    }

    data_t *data = symbol_table_lookup_var(stack_top_table(parser->stack), parser->token.attribute.string);
    if (data != NULL){
        error = ERR_SEM_FUNCTION;
        return false;   // Attempt at calling a function, that exists as a variable in the current context
    }

    int argnum = 0;
    data = symbol_table_lookup_func(stack_top_table(parser->stack), parser->token.attribute.string);
    if (data == NULL){
        // calling a non-existing function
        data_t data;
        data.name = parser->token.attribute.string;
        data.type = FUNC;
        data.value.func_id.defined = false;
        data.value.func_id.return_type = VAL_UNKNOWN;
        if (var != NULL){
            data.value.func_id.return_type = var->value.var_id.type;
        }
        symbol_table_insert(stack_top_table(parser->stack), data.name, data);
    } else {
        if (var != NULL){
            if (data->value.func_id.return_type != var->value.var_id.type){
                error = ERR_SEM_TYPE;
                return false;   // Attempt at calling a function with a return type that does not match the type of the variable
            }
        }
        argnum = data->value.func_id.parameters->size;
    }

    data = symbol_table_lookup_func(stack_top_table(parser->stack), parser->token.attribute.string);
    if (!data->value.func_id.defined && var != NULL) {
        data_t *variable_assignent = stack_lookup_var(parser->stack, data->name);
        if (variable_assignent == NULL) {
            if (data->name == var->name) {
                error = ERR_SEM_FUNCTION;
                return false;   // Attempt at calling a function that is not defined yet, but is being defined as a variable in the current context
            }
        }
    }

    load_token(parser);
    if (!is_type(parser, TOK_LBRACKET)){
        return false;
    }
    load_token(parser);
    int argindex = 0;
    if (!rule_arguments(parser, argnum, &argindex, data)){
        return false;
    }
    if (!is_type(parser, TOK_RBRACKET)){
        return false;
    }

    // todo: check if i should consume token here or in arguments

    return true;
}

// <arguments> -> <argument> <more_arguments> | ε

bool rule_arguments(parser_t *parser, int argnum, int *argindex, data_t *data){
    if (is_type(parser, TOK_RBRACKET)){
        return true;
    }
    if(!rule_argument(parser, argindex, data)){
        return false;
    }
    if (*argindex >= argnum){
        return false;  // Attempt at calling a function with too many arguments
    }

    return rule_more_arguments(parser, argnum, argindex, data);
}

// <argument> -> <arg_name> <arg_value>

bool rule_argument(parser_t *parser, int *argindex, data_t *data){
    if (!rule_arg_name(parser, argindex, data)){
        return false;
    }
    return rule_arg_value(parser, argindex, data);
}

// <arg_name> -> <identifier> ":" | ε

bool rule_arg_name(parser_t *parser, int *argindex, data_t *data){
    switch(parser->token.type){
        case TOK_IDENTIFIER:
            // argument is named
            if (strcmp(data->value.func_id.parameters->data[*argindex].call_name, "_")) {
                if (strcmp(parser->token.attribute.string, data->value.func_id.parameters->data[*argindex].call_name)) {
                    return false; // argument should be named but doesnt match
                    // TODO
                }
                load_token(parser);
                if (!is_type(parser, TOK_COLON)){
                    return false;
                }
                load_token(parser);
                return true;
            } else {
                // is not named -> pass over to arg_value
                return true;
            }
        default:
            // argument is named
            if (strcmp(data->value.func_id.parameters->data[*argindex].call_name, "_")) {
                return false; // argument should be named but isnt
                // TODO
            }
            // argument is not named
            return true;
            break;
    }
}

// <arg_value> -> <identifier> | <int> | <double> | <string> | <nil>

bool rule_arg_value(parser_t *parser, int *argindex, data_t *data){
    variable_type_t param_type = data->value.func_id.parameters->data[*argindex].parameter.type;
    data_t *arg = NULL;
    switch(parser->token.type){
        case TOK_IDENTIFIER:
            // check if var exists
            arg = symbol_table_lookup_var(stack_top_table(parser->stack), parser->token.attribute.string);
            if (arg == NULL){
                error = ERR_SEM_FUNCTION;
                return false;   // Attempt at calling a function with a non-existing variable
            }
            // check correct type
            if (arg->value.var_id.type != param_type){
                error = ERR_SEM_TYPE;
                return false;   // Attempt at calling a function with a variable of a different type than the function expects
            }
            break;
        case TOK_INT:
            if (param_type != VAL_INT && param_type != VAL_INTQ){
                error = ERR_SEM_TYPE;
                return false;   // Attempt at calling a function with an int argument, where the function expects a different type
            }
            break;
        case TOK_DOUBLE:
            if (param_type != VAL_DOUBLE && param_type != VAL_DOUBLEQ){
                error = ERR_SEM_TYPE;
                return false;   // Attempt at calling a function with a double argument, where the function expects a different type
            }
            break;
        case TOK_STRING:
            if (param_type != VAL_STRING && param_type != VAL_STRINGQ){
                error = ERR_SEM_TYPE;
                return false;   // Attempt at calling a function with a string argument, where the function expects a different type
            }
            break;
        case K_NIL:
            if (param_type != VAL_INTQ && param_type != VAL_DOUBLEQ && param_type != VAL_STRINGQ){
                error = ERR_SEM_TYPE;
                return false;   // Attempt at calling a function with a nil argument, where the function expects a different type
            }
            break;
        default:
            // TODO
            return false;
    }
    (*argindex)++;
    load_token(parser);
    return true;
}

// <more_arguments> -> "," <argument> <more_arguments> | ε

bool rule_more_arguments(parser_t *parser, int argnum, int *argindex, data_t *data){
    if(is_type(parser, TOK_RBRACKET)){
        return true;
    }

    if (!is_type(parser, TOK_COMMA)){
        return false;
    }
    load_token(parser);
    if (!rule_argument(parser, argindex, data)){
        return false;
    }
    if (*argindex >= argnum){
        return false;  // Attempt at calling a function with too many arguments
    }
    if (!rule_more_arguments(parser, argnum, argindex, data)){
        return false;
    }
    return true;
}

// <return_statement> -> "return" <returned_expression>

bool rule_return_statement(parser_t *parser){
    if (!is_type(parser, K_RETURN)){
        return false;
    }

    if (!parser->in_function){
        error = ERR_SEM_FUNCTION;
        return false;   // Attempt at returning from outside of a function
    }

    if (!parser->func_is_void && !parser->returned) {
    // checking function returns for non-void functions:
        if (parser->in_cycle){
            // Returning from a cycle holds no real meaning on the function having a return value, so we ignore it
            // meaning: `while (a == 5) {return 5}` is the same as `while (a == 5) {}`, since we cant check the condition statically
        } else if (!parser->in_if && !parser->in_else) {
            parser->returned = true;                        // Returning form main function body is always valid
        } else if (parser->in_if) {
            parser->returned_conditional = true;            // Returning from an if statement is valid, but we need to check if the else branch is also returned from
        } else if (parser->returned_conditional && parser->in_else) {
            parser->returned_conditional = false;
            parser->returned = true;                        // If both branches return, the condition is satysfied
        }
    }


    load_token(parser);
    return rule_returned_expression(parser);
}

// <returned_expression> -> <expression> | ε
// TODO: poresit return ve void
bool rule_returned_expression(parser_t *parser){
    if (is_expression(parser)){
        if(parser->func_is_void){   // PROBLEM: muze se stat, ze budu mit 
                                    // return
                                    // x = 5
                                    // toto je validni kod, ale zpusobil by chybu, protoze nemame eol
            error = ERR_SEM_TYPE;
            return false;
        }
        variable_type_t type = exp_parsing(parser);
        if (type == EXP_ERR){
            return false;
        }
        if (type != parser->return_type){
            error = ERR_SEM_TYPE;
            return false;   // Attempt at returning a value of a different type than the function expects
        }
        return true;
    }

    if (!parser->func_is_void){
        error = ERR_SEM_TYPE;
        return false;
    }
    return true;
}


//================= GRAMMAR RULES END HERE ================= //