/**
 *  @file parser.c
 *
 *  @authors Milan Jakubec (xjakub41)
 *  @authors Jakub Ráček (xracek12)
 */

#include "parser.h"
#include "error.h"
#include "expression.h"
#include <string.h>

error_t error; // Global error variable

/** ================== LL GRAMMAR ==================

<program> -> <statement> <program> | <function_definition> <program> | ε

<statement_list> -> <statement> <statement_list> | ε

<statement> -> <variable_definition_let> | <variable_definition_var> | <assignment> | <conditional_statement> | <loop> | <function_call> | <return_statement>

<function_definition> -> "func" <identifier> "(" <parameter_list> ")" <function_return_type_and_body>

<function_return_type_and_body> -> "{" <void_function_body> "}" | "->" <type> "{" <nonvoid_function_body> "}"

<nonvoid_function_body> -> <statement_list>

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
bool rule_parameter_list(parser_t *parser, data_t *data, int *index, int *argnum);
bool rule_more_parameters(parser_t *parser, data_t *data, int *index, int *argnum);
bool rule_parameter(parser_t *parser, data_t *data, int *index);
bool rule_no_name_parameter(parser_t *parser, data_t *data, int *index);
bool rule_identifier_parameter(parser_t *parser, data_t *data, int *index);
bool rule_rest_of_identifier_parameter(parser_t *parser, data_t *data, int *index);
bool rule_no_id_parameter(parser_t *parser);
bool rule_all_parameters(parser_t *parser);
bool rule_type(parser_t *parser);
bool rule_variable_definition_let(parser_t *parser);
bool rule_variable_definition_var(parser_t *parser);
bool rule_definition_types(parser_t *parser, data_t *data);
bool rule_type_def(parser_t *parser, data_t *data);
bool rule_type_def_follow(parser_t *parser, data_t *data);
bool rule_initialization(parser_t *parser, data_t *data);
bool rule_assignment(parser_t *parser);
bool rule_assignment_type(parser_t *parser, data_t *data);
bool rule_conditional_statement(parser_t *parser);
bool rule_if_statement(parser_t *parser);
bool rule_classical_statement(parser_t *parser);
bool rule_variable_statement(parser_t *parser);
bool rule_loop(parser_t *parser);
bool rule_function_call(parser_t *parser, data_t *data);
bool rule_arguments(parser_t *parser, int argnum, int *argindex, data_t *data, vector_t *params);
bool rule_argument(parser_t *parser, int *argindex, data_t *data, vector_t *params);
bool rule_arg_name(parser_t *parser, int *argindex, data_t *data);
bool rule_arg_value(parser_t *parser, int *argindex, data_t *data, vector_t *params);
bool rule_more_arguments(parser_t *parser, int argnum, int *argindex, data_t *data, vector_t *params);
bool rule_return_statement(parser_t *parser);
bool rule_returned_expression(parser_t *parser);
void insert_builtins_to_table(parser_t *parser);
void check_function_definitions(parser_t *parser);
//================= FUNCTION DECLARATIONS END HERE ================= //

//================= PARSER FUNCTIONS ================= //

void parser_init(parser_t *parser, gen_t *gen)
{
    error = ERR_OK;
    parser->stack = malloc(sizeof(stack_t));
    stack_init(parser->stack);
    parser->function = NULL;
    parser->in_function = false;
    parser->func_is_void = false;
    parser->in_if = false;
    parser->in_cycle = false;
    parser->in_else = false;
    parser->returned = false;
    parser->returned_conditional = false;
    parser->return_type = VAL_UNKNOWN;

    stack_push_table(parser->stack);
    insert_builtins_to_table(parser);

    parser->token = get_next_token();
    parser->next_token = get_next_token();
    parser->gen = gen;
    gen_init(gen);
}

void parser_destroy(parser_t *parser)
{
    stack_pop_table(parser->stack);
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

void check_function_definitions(parser_t *parser) {
    symbol_table_t *table = stack_bottom_table(parser->stack);
    for (int i = 0; i < table->capacity; i++) {
        if (table->table[i] != NULL && table->table[i]->data.type == FUNC && !table->table[i]->data.value.func_id.defined) {
            error = ERR_SEM_FUNCTION;
            print_error_and_exit(error);
        }
    }
}

//================= PARSER FUNCTIONS END HERE ================= //
//-------------------------------------------------------------//
//================= GRAMMAR RULES START HERE ================= //

// <program> -> <statement> <program> | <function_definition> <program> | ε

bool rule_program(parser_t *parser){
    if (is_type(parser, TOK_EOF)){                  // ε
        check_function_definitions(parser);
        return true;
    }

    else if (rule_statement(parser)){               // <statement>
        return rule_program(parser);                // <program>
    }
    else if (rule_function_definition(parser)){     // <function_definition>
        return rule_program(parser);                // <program>
    }
    else{
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }

    return false;
}

// <statement_list> -> <statement> <statement_list> | ε
// Note: statement list is only used in condtional statements and loops, thats why there is a check for RCURLYBRACKET

bool rule_statement_list(parser_t *parser){
    if (is_type(parser, TOK_RCURLYBRACKET) && 
    (parser->in_function || parser->in_cycle || parser->in_if || parser->in_else)) {
        if (parser->in_function && !(parser->in_cycle || parser->in_if || parser->in_else) && !parser->func_is_void && !parser->returned) {
            error = ERR_SEM_RETURN;
            print_error_and_exit(error);
            return false; // Function does not return a value

        }

        return true;                                                      // ε
    }

    return rule_statement(parser) && rule_statement_list(parser);         // <statement> <statement_list>
}

// <statement> -> <variable_definition_let> | <variable_definition_var> | <assignment> | <conditional_statement> | <loop> | <function_call> | <return_statement>

bool rule_statement(parser_t *parser){
    switch(parser->token.type){
        case K_LET:
            return rule_variable_definition_let(parser);    // <variable_definition_let>
        case K_VAR:
            return rule_variable_definition_var(parser);    // <variable_definition_var>
        case TOK_IDENTIFIER:
            if(is_type_next(parser, TOK_LBRACKET)){
                return rule_function_call(parser, NULL);    // <function_call>
            }
            else{
                return rule_assignment(parser);             // <assignment>
            }
        case K_IF:
            return rule_conditional_statement(parser);      // <conditional_statement>
        case K_WHILE:
            return rule_loop(parser);                       // <loop>
        case K_RETURN:
            return rule_return_statement(parser);           // <return_statement>
        case K_FUNC:
            return false; // function definition has a separate rule not under statement
        default:
            // Got a token that does not start a statement, syntax error
            error = ERR_SYN;
            print_error_and_exit(error);
            return false;
    }
}

// <function_definition> -> "func" <identifier> "(" <parameter_list> ")" <function_return_type_and_body>

bool rule_function_definition(parser_t *parser){
    if (stack_top_table(parser->stack) != stack_bottom_table(parser->stack)) {
        error = ERR_SEM_OTHER;
        print_error_and_exit(error);   // Function definition outside of global scope is not supported
        return false;
    }
    parser->in_function = true;
    if (!is_type(parser, K_FUNC)){                              // "func"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    load_token(parser);
    if (!is_type(parser, TOK_IDENTIFIER)){                      // <identifier>
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }

    gen_func(parser->gen, &parser->token);

    // ----------------- SEMANTICS START ----------------- 
    data_t *func = symbol_table_lookup_generic(stack_top_table(parser->stack), parser->token.attribute.string);
    int index = 0;
    int argnum = 0;
    if (func != NULL){
        if (func->type != FUNC || (func->type == FUNC && func->value.func_id.defined)) {
            error = ERR_SEM_FUNCTION;
            print_error_and_exit(error);
            return false;   // Attempt at defining an already defined function or global variable of the same name
        }
        // Funtion was called before, now we are defining it
        argnum = func->value.func_id.parameters->size;  
        func->value.func_id.defined = true;
    } else {
        // Function does not exist, create a new function object
        data_t func;
        func.type = FUNC;
        func.name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1));
        strcpy(func.name, parser->token.attribute.string);
        func.value.func_id.defined = true;
        func.value.func_id.arguments_defined = false;
        func.value.func_id.return_type = VAL_UNKNOWN;
        func.value.func_id.parameters = vector_init(5);
        symbol_table_insert(stack_top_table(parser->stack), func.name, func);
    }
    func = symbol_table_lookup_generic(stack_top_table(parser->stack), parser->token.attribute.string);
    // ----------------- SEMANTICS END -----------------

    load_token(parser);
    if (!is_type(parser, TOK_LBRACKET)){                        // "("
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    load_token(parser);
    stack_push_table(parser->stack);
    if (!rule_parameter_list(parser, func, &index, &argnum)){   // <parameter_list>
        return false;
    }
    gen_parameters(parser->gen, func->value.func_id.parameters);
    argnum = index;
    func->value.func_id.arguments_defined = true;
    if (!is_type(parser, TOK_RBRACKET)){                        // ")"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    load_token(parser);
    stack_push_table(parser->stack);
    if (!rule_function_return_type_and_body(parser, func)){    // <function_return_type_and_body>
        return false;
    }
    stack_pop_table(parser->stack);
    stack_pop_table(parser->stack);

    // ----------------- SEMANTICS START -----------------
    // loop through all variables in current table and check if they were initialized in the function
    symbol_table_t *table = stack_top_table(parser->stack);
    for (int i = 0; i < table->capacity; i++) {
        if (table->table[i] != NULL && (table->table[i]->data.type == LET || table->table[i]->data.type == VAR) && table->table[i]->data.value.var_id.func_init) {
            table->table[i]->data.value.var_id.func_init = false;
            table->table[i]->data.value.var_id.initialized = false;
        }
    }
    // ----------------- SEMANTICS END -----------------
    
    gen_func_end(parser->gen, parser->func_is_void);

    return true;
}

// <function_return_type_and_body> -> "{" <void_function_body> "}" | "->" <type> "{" <nonvoid_function_body> "}"

bool rule_function_return_type_and_body(parser_t *parser, data_t *data){
    if (is_type(parser, TOK_ARROW)){                        // "->"
        parser->func_is_void = false;
        load_token(parser);
        data->value.func_id.return_type = str_to_type(parser->token);
        parser->return_type = str_to_type(parser->token);
        if (!rule_type(parser)){                            // <type>
            error = ERR_SYN;
            print_error_and_exit(error);
            return false;
        }
        load_token(parser);
        if (!is_type(parser, TOK_LCURLYBRACKET)){           // "{"
            error = ERR_SYN;
            print_error_and_exit(error);
            return false;
        }
        load_token(parser);
        if (!rule_nonvoid_function_body(parser)){           // <nonvoid_function_body>
            return false;
        }
        if (!is_type(parser, TOK_RCURLYBRACKET)){           // "}"
            error = ERR_SYN;
            print_error_and_exit(error);
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
    if (!is_type(parser, TOK_LCURLYBRACKET)){               // "{"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    load_token(parser);

    if (!rule_void_function_body(parser)) {                 // <void_function_body>
        return false;
    }

    if (!is_type(parser, TOK_RCURLYBRACKET)) {              // "}"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }

    parser->in_function = false;
    parser->returned = false;
    parser->returned_conditional = false;
    load_token(parser);
    return true;
}

// <nonvoid_function_body> -> <statement_list>

bool rule_nonvoid_function_body(parser_t *parser){
    parser->in_function = true;
    if (is_type(parser, TOK_RCURLYBRACKET)){
        error = ERR_SEM_RETURN;
        print_error_and_exit(error);
        return true;
    } else if (rule_statement_list(parser)){        // <statement_list>
        return true;
    }

    return false;
}

// <void_function_body> -> <statement_list> | ε

bool rule_void_function_body(parser_t *parser){
    parser->in_function = true;
    if (!rule_statement_list(parser)){              // <statement_list>
        return false;
    }
    else if (is_type(parser, TOK_RCURLYBRACKET)){   // ε
        parser->in_function = false;
        return true;
    }

    error = ERR_SYN;
    print_error_and_exit(error);
    return false;
}

// <parameter_list> -> <parameter> <more_parameters> | ε

bool rule_parameter_list(parser_t *parser, data_t *data, int *index, int *argnum){
    if (is_type(parser, TOK_RBRACKET)){                         // ε
        return true;
    }
    return rule_parameter(parser, data, index)                  // <parameter>  
        && rule_more_parameters(parser, data, index, argnum);   // <more_parameters>
}

// <more_parameters> -> "," <parameter> <more_parameters> | ε

bool rule_more_parameters(parser_t *parser, data_t *data, int *index, int *argnum){
    load_token(parser);

    if (is_type(parser, TOK_RBRACKET)){                         // ε
        return true;
    }

    if (!is_type(parser, TOK_COMMA)){                           // ","
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    load_token(parser);
    if (!rule_parameter(parser, data, index)){                  // <parameter>
        return false;
    }

    // ----------------- SEMANTICS START -----------------
    if (data->value.func_id.arguments_defined && *index > *argnum){
        error = ERR_SEM_FUNCTION;
        print_error_and_exit(error);
        return false;  // Attempt at calling a function with too many arguments
    }
    // ----------------- SEMANTICS END -----------------

    if (!rule_more_parameters(parser, data, index, argnum)){    // <more_parameters>
        return false;
    }
    return true;
}

// <parameter> -> <no_name_parameter> | <identifier_parameters>

bool rule_parameter(parser_t *parser, data_t *data, int *index){
    // ----------------- SEMANTICS START -----------------
    if (!data->value.func_id.arguments_defined){
        vector_push(data->value.func_id.parameters, (htab_func_param_t){}); // Add a new parameter to the vector
    }
    // ----------------- SEMANTICS END -----------------

    if (rule_no_name_parameter(parser, data, index)){           // <no_name_parameter>
        return true;
    }
    return rule_identifier_parameter(parser, data, index);      // <identifier_parameters>
}

// <no_name_parameter> -> "_" <identifier> ":" <type>

bool rule_no_name_parameter(parser_t *parser, data_t *data, int *index){
    if (!is_type(parser, TOK_UNDERSCORE)){                    // "_"
        return false;
    }

    // ----------------- SEMANTICS START -----------------
    if (data->value.func_id.arguments_defined){
        if (strcmp(data->value.func_id.parameters->data[*index].call_name, "_")) {
            error = ERR_SEM_CALL;
            print_error_and_exit(error);
            return false; // argument should not be named but isnt
        }
    } else {
        vector_top(data->value.func_id.parameters)->call_name = malloc(sizeof(char) * 2);
        strcpy(vector_top(data->value.func_id.parameters)->call_name, "_");
    }
    // ----------------- SEMANTICS END -----------------

    load_token(parser);
    if (!is_type(parser, TOK_IDENTIFIER)){                      // <identifier>
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }

    // ----------------- SEMANTICS START -----------------
    data_t param; // symtable variable
    param.name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1));
    strcpy(param.name, parser->token.attribute.string);
    param.type = LET;
    param.value.var_id.type = VAL_UNKNOWN;
    param.value.var_id.func_init = true;

    if (data->value.func_id.arguments_defined) {
        data->value.func_id.parameters->data[*index].def_name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1)); // identifier
        strcpy(data->value.func_id.parameters->data[*index].def_name, parser->token.attribute.string);
    } else {
        vector_top(data->value.func_id.parameters)->def_name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1)); // identifier
        strcpy(vector_top(data->value.func_id.parameters)->def_name, parser->token.attribute.string);
    }
    // ----------------- SEMANTICS END -----------------

    load_token(parser);
    if (!is_type(parser, TOK_COLON)){                           // ":"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }

    load_token(parser);
    // ----------------- SEMANTICS START -----------------
    param.value.var_id.type = str_to_type(parser->token); // type
    param.value.var_id.initialized = true;
    symbol_table_insert(stack_top_table(parser->stack), param.name, param); // push to table
    if (data->value.func_id.arguments_defined) {
        if (data->value.func_id.parameters->data[*index].parameter.type != str_to_type(parser->token)) {
            error = ERR_SEM_CALL;
            print_error_and_exit(error);
            return false; // argument type doesnt match
        }
    } else {
        vector_top(data->value.func_id.parameters)->parameter.type = str_to_type(parser->token);
    }
    (*index)++;
    // ----------------- SEMANTICS END -----------------

    return rule_type(parser);                                   // <type>
}

// <identifier_parameter> -> <identifier> <rest_of_identifier_parameter>

bool rule_identifier_parameter(parser_t *parser, data_t *data, int *index){
    if (!is_type(parser, TOK_IDENTIFIER)){                          // <identifier>
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }

    // ----------------- SEMANTICS START -----------------
    if (data->value.func_id.arguments_defined){
        if (strcmp(data->value.func_id.parameters->data[*index].call_name, parser->token.attribute.string)) {
            error = ERR_SEM_CALL;
            print_error_and_exit(error);
            return false; // argument should be named but isnt
        }
    } else {
        vector_top(data->value.func_id.parameters)->call_name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1)); // identifier
        strcpy(vector_top(data->value.func_id.parameters)->call_name, parser->token.attribute.string);
    }
    // ----------------- SEMANTICS END -----------------

    load_token(parser);
    return rule_rest_of_identifier_parameter(parser, data, index);  // <rest_of_identifier_parameter>
}

// <rest_of_identifier_parameter> -> "_" ":" <type> |  <identifier> ":" <type>

bool rule_rest_of_identifier_parameter(parser_t *parser, data_t *data, int *index){
    if (is_type(parser, TOK_UNDERSCORE)){                           // "_"

        // ----------------- SEMANTICS START -----------------
        vector_top(data->value.func_id.parameters)->def_name = malloc(sizeof(char) * 2);
        strcpy(vector_top(data->value.func_id.parameters)->def_name, "_");

        load_token(parser);
        if (!is_type(parser, TOK_COLON)){
            error = ERR_SYN;
            print_error_and_exit(error);
            return false;
        }
        load_token(parser);
        if (data->value.func_id.arguments_defined) {
            if (data->value.func_id.parameters->data[*index].parameter.type != str_to_type(parser->token)) {
                error = ERR_SEM_CALL;
                print_error_and_exit(error);
                return false; // argument type doesnt match
            }
        } else {
            vector_top(data->value.func_id.parameters)->parameter.type = str_to_type(parser->token);
        }
        (*index)++;
        // ----------------- SEMANTICS END -----------------

        return rule_type(parser);                                   // <type>
    }
    else if (is_type(parser, TOK_IDENTIFIER)){                      // <identifier>
        // ----------------- SEMANTICS START -----------------
        if (!strcmp(vector_top(data->value.func_id.parameters)->call_name, parser->token.attribute.string)) {
            error = ERR_SEM_OTHER;
            print_error_and_exit(error);
            return false; // def name and call name match
        }

        vector_top(data->value.func_id.parameters)->def_name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1)); // identifier
        strcpy(vector_top(data->value.func_id.parameters)->def_name, parser->token.attribute.string);

        data_t param; // symtable variable
        param.name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1));
        strcpy(param.name, parser->token.attribute.string);
        param.type = LET;
        param.value.var_id.type = VAL_UNKNOWN;
        param.value.var_id.func_init = true;

        load_token(parser);
        if (!is_type(parser, TOK_COLON)){                           // ":"
            error = ERR_SYN;
            print_error_and_exit(error);
            return false;
        }
        load_token(parser);
        if (data->value.func_id.arguments_defined) {
            if (data->value.func_id.parameters->data[*index].parameter.type != str_to_type(parser->token)) {
                error = ERR_SEM_CALL;
                print_error_and_exit(error);
                return false; // argument type doesnt match
            }
        } else {
            vector_top(data->value.func_id.parameters)->parameter.type = str_to_type(parser->token);
        }
        param.value.var_id.type = str_to_type(parser->token);
        param.value.var_id.initialized = true;
        symbol_table_insert(stack_top_table(parser->stack), param.name, param);
        (*index)++;
        // ----------------- SEMANTICS END -----------------

        return rule_type(parser);                                   // <type>
    }

    error = ERR_SYN;
    print_error_and_exit(error);
    return false;
        
}

// <type> -> "Double" | "Int" | "String" | "Double?" | "Int?" | "String?"

bool rule_type(parser_t *parser){
    switch(parser->token.type){
        case K_DOUBLE:                                          // "Double"
        case K_INT:                                             // "Int"
        case K_STRING:                                          // "String"
        case K_DOUBLEQ:                                         // "Double?"
        case K_INTQ:                                            // "Int?"
        case K_STRINGQ:                                         // "String?"
            return true;
        default:
            error = ERR_SYN;
            print_error_and_exit(error);
            return false;
    }
}

// <variable_definition_let> -> "let" <identifier> <definition_types> (pozn. let je NEMODIFIKOVATELNA promenna)

bool rule_variable_definition_let(parser_t *parser){
    if (!is_type(parser, K_LET)){                               // "let"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    load_token(parser);
    if (!is_type(parser, TOK_IDENTIFIER)){                      // <identifier>
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }

    gen_var_definition(parser->gen, &parser->token, parser->in_function, (parser->in_cycle || parser->in_if || parser->in_else));

    // ----------------- SEMANTICS START -----------------
    data_t *var = symbol_table_lookup_generic(stack_top_table(parser->stack), parser->token.attribute.string);
    if (var != NULL){
        error = ERR_SEM_FUNCTION;
        print_error_and_exit(error);
        return false;   // Attempt at defining an already existing variable or function within the current context
    }

    data_t data;
    data.type = LET;
    data.name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1));
    strcpy(data.name, parser->token.attribute.string);
    data.value.var_id.type = VAL_UNKNOWN;
    data.value.var_id.initialized = false;
    // ----------------- SEMANTICS END -----------------

    load_token(parser);
    if(rule_definition_types(parser, &data)) {                  // <definition_types>
        symbol_table_insert(stack_top_table(parser->stack), data.name, data);
        return true;
    }
    return false;
}

// <variable_definition_var> -> "var" <identifier> <definition_types>

bool rule_variable_definition_var(parser_t *parser){
    if (!is_type(parser, K_VAR)){                               // "var"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    load_token(parser);

    if(!is_type(parser, TOK_IDENTIFIER)){                       // <identifier>
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }

    gen_var_definition(parser->gen, &parser->token, parser->in_function, (parser->in_cycle || parser->in_if || parser->in_else));

    // ----------------- SEMANTICS START -----------------
    data_t *var = symbol_table_lookup_generic(stack_top_table(parser->stack), parser->token.attribute.string);
    if (var != NULL){
        error = ERR_SEM_FUNCTION;
        print_error_and_exit(error);
        return false;   // Attempt at defining an already existing variable or function withing the current context
    }

    data_t data;
    data.type = VAR;
    data.name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1));
    strcpy(data.name, parser->token.attribute.string);
    data.value.var_id.type = VAL_UNKNOWN;
    data.value.var_id.initialized = false;
    // ----------------- SEMANTICS END -----------------

    load_token(parser);
    if(rule_definition_types(parser, &data)) {                  // <definition_types>
        symbol_table_insert(stack_top_table(parser->stack), data.name, data);
        return true;
    }
    return false;
}

// <definition_types> -> <type_def> | <initialization>

bool rule_definition_types(parser_t *parser, data_t *data){
    if (rule_type_def(parser, data)){                           // <type_def>
        return true;
    }

    // No type was provided - add a placeholder type
    data->value.var_id.type = VAL_UNKNOWN;
    return rule_initialization(parser, data);                   // <initialization>
}

// <type_def> -> ":" <type> <type_def_follow>

bool rule_type_def(parser_t *parser, data_t *data){
    if (!is_type(parser, TOK_COLON)){                           // ":"
        return false;
    }
    load_token(parser);
    if (!rule_type(parser)){                                    // <type>
        return false;
    }

    // loaded token is the type of the variable
    data->value.var_id.type = str_to_type(parser->token);

    load_token(parser);
    return rule_type_def_follow(parser, data);                  // <type_def_follow>
}

// <type_def_follow> -> <initialization> | ε

bool rule_type_def_follow(parser_t *parser, data_t *data){
    if (!is_type(parser, TOK_ASSIGNMENT)) {                     // ε    
        if(data->value.var_id.type == VAL_INTQ || data->value.var_id.type == VAL_DOUBLEQ || data->value.var_id.type == VAL_STRINGQ){
            gen_push_nil(parser->gen, parser->in_function);
            bool is_global = !(parser->in_cycle || parser->in_if || parser->in_else || parser->in_function);
            gen_pop_value(parser->gen, data->name, parser->in_function, is_global);
        }
        return true;
    }

    return rule_initialization(parser, data);                   // <initialization>
}

// <initialization> -> "=" <expression> | "=" <function_call>

bool rule_initialization(parser_t *parser, data_t *data){

    if (!is_type(parser, TOK_ASSIGNMENT)){                                           // "="
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }

    load_token(parser);

    if (is_type(parser, TOK_IDENTIFIER) && is_type_next(parser, TOK_LBRACKET)){     // <function_call>
        bool ret = rule_function_call(parser, data); // init type in function call
        gen_func_return_to_var(parser->gen, data->name, parser->in_function, !(parser->in_if || parser->in_else || parser->in_cycle || parser->in_function));
        data->value.var_id.initialized = true;
        return ret;
    }

    variable_type_t type = exp_parsing(parser);                                     // <expression>

    gen_pop_value(parser->gen, data->name, parser->in_function, !(parser->in_if || parser->in_else || parser->in_cycle || parser->in_function));

    // ----------------- SEMANTICS START -----------------
    switch (type) { // check if the expr result matches the type of the variable
    case VAL_INT:
        if (type == VAL_INT && (data->value.var_id.type == VAL_DOUBLE || data->value.var_id.type == VAL_DOUBLEQ)) {
            gen_push_var(parser->gen, data->name, parser->in_function, !(parser->in_if || parser->in_else || parser->in_cycle || parser->in_function));
            gen_call_convert(parser->gen); // evil int2float hacks
            gen_func_return_to_var(parser->gen, data->name, parser->in_function, !(parser->in_if || parser->in_else || parser->in_cycle || parser->in_function));
        } else if (data->value.var_id.type != VAL_INT && data->value.var_id.type != VAL_INTQ && data->value.var_id.type != VAL_UNKNOWN){
            error = ERR_SEM_TYPE;
            print_error_and_exit(error);
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
            print_error_and_exit(error);
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
            print_error_and_exit(error);
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
            if (data->value.var_id.type == VAL_UNKNOWN){
                error = ERR_SEM_INCOMPATIBLE;
            }
            print_error_and_exit(error);
            return false;
        }
        break;
    case VAL_UNKNOWN:
        if (data->value.var_id.type == VAL_UNKNOWN){
            error = ERR_SEM_TYPE;
            print_error_and_exit(error);
            return false;
        }
        break;
    default:
        break;
    }
    data->value.var_id.initialized = true; 
    // ----------------- SEMANTICS END -----------------

    return true;
}

// <assignment> -> <identifier> "=" <assignment_type>

bool rule_assignment(parser_t *parser){
    if (!is_type(parser, TOK_IDENTIFIER)){              // <identifier>
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }

    // ----------------- SEMANTICS START -----------------
    data_t *var = stack_lookup_var(parser->stack, parser->token.attribute.string);
    if (var == NULL){
        error = ERR_SEM_FUNCTION;
        print_error_and_exit(error);
        return false;   // Attempt at assigning to a non-existing variable
    }
    // ----------------- SEMANTICS END -----------------

    load_token(parser);
    if (!is_type(parser, TOK_ASSIGNMENT)){              // "="
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    load_token(parser);
    if (!rule_assignment_type(parser, var)){            // <assignment_type>
        return false;
    }

    // ----------------- SEMANTICS START -----------------
    bool is_global = stack_lookup_var_in_global(parser->stack, var->name);
    if (!var->value.var_id.initialized) {
    // checking var inits for non-void functions:
        if (parser->in_cycle){
            // do nothing
        } else if (!parser->in_if && !parser->in_else) {
            var->value.var_id.initialized = true;        // Returning form main function body is always valid
            if (is_global && parser->in_function) {
                var->value.var_id.func_init = true;
            }
        } else if (parser->in_if) {
            var->value.var_id.if_initialized = true;     // Returning from an if statement is valid, but we need to check if the else branch is also returned from
        } else if (var->value.var_id.if_initialized && parser->in_else) {
            var->value.var_id.initialized = true;     // If both branches return, the condition is satysfied
            var->value.var_id.if_initialized = false;
            if (is_global && parser->in_function) {
                var->value.var_id.func_init = true;
            }
        }
    }
    // ----------------- SEMANTICS END -----------------
    
    return true;
}

// <assignment_type> -> <function_call> | <expression>

bool rule_assignment_type(parser_t *parser, data_t *data){
    if(is_type(parser, TOK_IDENTIFIER) && is_type_next(parser, TOK_LBRACKET)){      // <function_call>
        bool ret = rule_function_call(parser, data); // init type in function call
        bool is_global = stack_lookup_var_in_global(parser->stack, data->name);
        gen_func_return_to_var(parser->gen, data->name, parser->in_function, is_global);
        return ret;
    }
    else{

        variable_type_t type = exp_parsing(parser);                                 // <expression>

        // ----------------- SEMANTICS START -----------------
        bool is_global = stack_lookup_var_in_global(parser->stack, data->name);
        gen_pop_value(parser->gen, data->name, parser->in_function, is_global);

        if (data->type == LET && (data->value.var_id.initialized || data->value.var_id.func_init)){
            error = ERR_SEM_OTHER;
            print_error_and_exit(error);
            return false;   // Attempt at assigning to a let variable
        }
        if (data->value.var_id.type == VAL_UNKNOWN){
            data->value.var_id.type = type;
        }
        if (type != data->value.var_id.type){
            if (type == VAL_INT && (data->value.var_id.type == VAL_DOUBLE || data->value.var_id.type == VAL_DOUBLEQ)) {
                bool is_global = stack_lookup_var_in_global(parser->stack, data->name);
                gen_push_var(parser->gen, data->name, parser->in_function, is_global);
                gen_call_convert(parser->gen); // evil int2float hacks
                gen_func_return_to_var(parser->gen, data->name, parser->in_function, is_global);
                return true;
            }

            if ((data->value.var_id.type == VAL_DOUBLEQ && type == VAL_DOUBLE) ||
                (data->value.var_id.type == VAL_INTQ && type == VAL_INT) ||
                (data->value.var_id.type == VAL_STRINGQ && type == VAL_STRING)){
                return true;
            }
            error = ERR_SEM_TYPE; // assignment type does not match the type of the variable
            print_error_and_exit(error);
            return false;
        }
        // ----------------- SEMANTICS END -----------------

        return true;
    }
}

// <conditional_statement> -> if <if_statement>

bool rule_conditional_statement(parser_t *parser){
    if (!is_type(parser, K_IF)){                    // "if"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    load_token(parser);
    return rule_if_statement(parser);               // <if_statement>
}

// <if_statement> -> <classical_statement> | <variable_statement>

bool rule_if_statement(parser_t *parser){
    if (is_type(parser, K_LET)){
        return rule_variable_statement(parser);     // <variable_statement>
    }
    return rule_classical_statement(parser);        // <classical_statement>
}

// <classical_statement> -> <expression> "{" <statement_list>  "}" "else" "{" <statement_list> "}"

bool rule_classical_statement(parser_t *parser){

    variable_type_t type = exp_parsing(parser);     // <expression>
    if (type != VAL_BOOL){
        error = ERR_SEM_TYPE;
        print_error_and_exit(error);
        return false;   // non-bool expression in if statement
    }

    int label = parser->gen->label_counter;
    gen_if(parser->gen, parser->in_function, label);

    if (!is_type(parser, TOK_LCURLYBRACKET)){       // "{"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    bool was_in_if = parser->in_if;
    parser->in_if = true;
    load_token(parser);
    stack_push_table(parser->stack);
    if (!rule_statement_list(parser)){              // <statement_list>
        return false;
    }
    stack_pop_table(parser->stack);
    if (!is_type(parser, TOK_RCURLYBRACKET)){       // "}"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    if (!was_in_if){
        parser->in_if = false;
    }
    load_token(parser);
    if (!is_type(parser, K_ELSE)){                  // "else"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }

    gen_else(parser->gen, parser->in_function, label);

    load_token(parser);
    if (!is_type(parser, TOK_LCURLYBRACKET)){       // "{"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    bool was_in_else = parser->in_else;
    parser->in_else = true;
    load_token(parser);
    stack_push_table(parser->stack);
    if (!rule_statement_list(parser)){              // <statement_list>
        return false;
    }
    stack_pop_table(parser->stack);
    if (!is_type(parser, TOK_RCURLYBRACKET)){       // "}"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    if (!was_in_else){
        parser->in_else = false;
    }
    gen_endif(parser->gen, parser->in_function, label);
    load_token(parser);
    return true;
}

// <variable_statement> -> let <identifier> "{" <statement_list> "}" "else" "{" <statement_list> "}" (pozn. identifier zastupuje drive def. nemodifikovatelnou promennou)

bool rule_variable_statement(parser_t *parser){
    if (!is_type(parser, K_LET)){               // "let"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    load_token(parser);
    if (!is_type(parser, TOK_IDENTIFIER)){      // <identifier>
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }

    // ----------------- SEMANTICS START -----------------
    data_t *data = stack_lookup_var(parser->stack, parser->token.attribute.string);
    if (data == NULL) {
        error = ERR_SEM_NDEF;
        print_error_and_exit(error);
        return false;   // Attempt at defining a variable that does not exist
    }

    if (data->value.var_id.type != VAL_INTQ && data->value.var_id.type != VAL_DOUBLEQ && data->value.var_id.type != VAL_STRINGQ) {
        error = ERR_SEM_OTHER;
        print_error_and_exit(error);
        return false;   // Attempt at defining a variable that is not of type ? (unknown)
    }

    int label = parser->gen->label_counter;
    bool is_global = stack_lookup_var_in_global(parser->stack, data->name);
    gen_if_let(parser->gen, data->name, parser->in_function, is_global, label);

    symbol_type_t prev_type = data->type;
    data->type = LET;
    // ----------------- SEMANTICS END -----------------

    load_token(parser);
    if (!is_type(parser, TOK_LCURLYBRACKET)){   // "{"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    bool was_in_if = parser->in_if;
    parser->in_if = true;
    load_token(parser);
    stack_push_table(parser->stack);
    if (!rule_statement_list(parser)){          // <statement_list>
        return false;
    }
    stack_pop_table(parser->stack);
    if (!is_type(parser, TOK_RCURLYBRACKET)){   // "}"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    data->type = prev_type;
    if (!was_in_if){
        parser->in_if = false;
    }
    load_token(parser);
    if (!is_type(parser, K_ELSE)){              // "else"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    gen_else(parser->gen, parser->in_function, label);

    load_token(parser);
    if (!is_type(parser, TOK_LCURLYBRACKET)){   // "{"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    bool was_in_else = parser->in_else;
    parser->in_else = true;
    load_token(parser);
    stack_push_table(parser->stack);
    if (!rule_statement_list(parser)){          // <statement_list>
        return false;
    }
    stack_pop_table(parser->stack);
    if (!is_type(parser, TOK_RCURLYBRACKET)){   // "}"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    if (!was_in_else){
        parser->in_else = false;
    }
    gen_endif(parser->gen, parser->in_function, label);
    load_token(parser);
    return true;
}

// <loop> -> "while" <expression> "{" <statement_list> "}"

bool rule_loop(parser_t *parser){

    int while_index = parser->gen->label_counter;
    gen_while(parser->gen, parser->in_function, while_index);

    if (!is_type(parser, K_WHILE)){                 // "while"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    load_token(parser);
    
    
    variable_type_t type = exp_parsing(parser);     // <expression>
    if (type != VAL_BOOL){
        error = ERR_SEM_TYPE;
        print_error_and_exit(error);
        return false;   // non-bool expression in while loop
    }

    gen_while_exit(parser->gen, parser->in_function, while_index);


    if (!is_type(parser, TOK_LCURLYBRACKET)){       // "{"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    bool was_in_cycle = parser->in_cycle;
    parser->in_cycle = true;
    load_token(parser);
    stack_push_table(parser->stack);
    if (!rule_statement_list(parser)){              // <statement_list>
        return false;
    }
    stack_pop_table(parser->stack);
    if (!is_type(parser, TOK_RCURLYBRACKET)){       // "}"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    if (!was_in_cycle){
        parser->in_cycle = false;
    }
    load_token(parser);
    gen_while_end(parser->gen, parser->in_function, while_index);
    return true;
}

// <function_call> -> <identifier> "(" <arguments> ")"

bool rule_function_call(parser_t *parser, data_t *var){
    if (!is_type(parser, TOK_IDENTIFIER)){                              // <identifier>
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }

    // ------------ SEMANTICS START ------------
    data_t *data;
    if (is_table_global(parser->stack)) {
        data = symbol_table_lookup_var(stack_top_table(parser->stack), parser->token.attribute.string);
        if (data != NULL){
            error = ERR_SEM_FUNCTION;
            print_error_and_exit(error);
            return false;   // Attempt at calling a function, that exists as a variable in the current context
        }
    }

    int argnum = 0;
    data = stack_lookup_func(parser->stack, parser->token.attribute.string);
    if (data == NULL){
        // calling a non-existing function
        data_t data;
        data.name = parser->token.attribute.string;
        data.type = FUNC;
        data.value.func_id.defined = false;
        data.value.func_id.arguments_defined = false;
        data.value.func_id.parameters = vector_init(5);
        data.value.func_id.return_type = VAL_UNKNOWN;
        if (var != NULL){
            data.value.func_id.return_type = var->value.var_id.type;
        }
        symbol_table_insert(stack_bottom_table(parser->stack), data.name, data);
    } else {
        if (var != NULL ){
            if (data->value.func_id.return_type == VAL_VOID) {
                error = ERR_SEM_TYPE;
                print_error_and_exit(error);
                return false;   // Attempt at calling a function with a return type that does not match the type of the variable
            }

            if (var->value.var_id.type == VAL_UNKNOWN){
                var->value.var_id.type = data->value.func_id.return_type;
            } else if (data->value.func_id.return_type != var->value.var_id.type){
                error = ERR_SEM_TYPE;
                print_error_and_exit(error);
                return false;   // Attempt at calling a function with a return type that does not match the type of the variable
            }
        }
        argnum = data->value.func_id.parameters->size;
    }

    data = stack_lookup_func(parser->stack, parser->token.attribute.string);
    if (!data->value.func_id.defined && var != NULL) {
        data_t *variable_assignent = stack_lookup_var(parser->stack, data->name);
        if (variable_assignent == NULL) {
            if (data->name == var->name) {
                error = ERR_SEM_FUNCTION;
                print_error_and_exit(error);
                return false;   // Attempt at calling a function that is not defined yet, but is being defined as a variable in the current context
            }
        }
    }
    // ------------ SEMANTICS END ------------

    load_token(parser);
    if (!is_type(parser, TOK_LBRACKET)){                                    // "("
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    load_token(parser);
    int argindex = 0;
    vector_t *call_args = vector_init(5);
    if (!rule_arguments(parser, argnum, &argindex, data, call_args)){       // <arguments>
        return false;
    }

    // ------------ SEMANTICS START ------------
    if (!data->value.func_id.arguments_defined) {
        data->value.func_id.arguments_defined = true;
    }

    if (argindex < argnum && strcmp(data->name, "write")){
        error = ERR_SEM_CALL;
        print_error_and_exit(error);
        return false;  // Attempt at calling a function with too few arguments
    }
    call_args->size = argindex;
    // gen funkce zde
    gen_arguments_start(parser->gen, parser->in_function);
    bool is_global = false;
    for(int i = call_args->size - 1; i >= 0; i--){
        if (call_args->data[i].parameter.type == VAL_ID) {
            is_global = stack_lookup_var_in_global(parser->stack, call_args->data[i].parameter.value.string);
        }
        gen_arguments(parser->gen, call_args->data[i], parser->in_function, is_global);
    }
    if(strcmp(data->name, "write") == 0){
        gen_push_int(parser->gen, call_args->size, parser->in_function);
    }
    // ------------ SEMANTICS END ------------

    gen_func_call(parser->gen, data->name, parser->in_function);
    vector_destroy(call_args);
    if (!is_type(parser, TOK_RBRACKET)){                                    // ")"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    load_token(parser);
    return true;
}

// <arguments> -> <argument> <more_arguments> | ε

bool rule_arguments(parser_t *parser, int argnum, int *argindex, data_t *data, vector_t *call_args){
    if (is_type(parser, TOK_RBRACKET)){                                     // ε
        return true;
    }

    // ------------ SEMANTICS START ------------
    if (data->value.func_id.arguments_defined && argnum == 0){
        error = ERR_SEM_CALL;
        print_error_and_exit(error);
        return false;  // Attempt at calling a function with too many arguments
    }
    // ------------ SEMANTICS END ------------

    if(!rule_argument(parser, argindex, data, call_args)){                  // <argument>
        return false;
    }

    return rule_more_arguments(parser, argnum, argindex, data, call_args);  // <more_arguments>
}

// <argument> -> <arg_name> <arg_value>

bool rule_argument(parser_t *parser, int *argindex, data_t *data, vector_t *call_args){
    // ------------ SEMANTICS START ------------
    if (!data->value.func_id.arguments_defined)  {// on funcion call before definition ve save the function params
        vector_push(data->value.func_id.parameters, (htab_func_param_t){});
    }
    vector_push(call_args, (htab_func_param_t){});
    // ------------ SEMANTICS END ------------

    if (!rule_arg_name(parser, argindex, data)){                        // <arg_name>
        return false;
    }
    return rule_arg_value(parser, argindex, data, call_args);           // <arg_value>
}

// <arg_name> -> <identifier> ":" | ε

bool rule_arg_name(parser_t *parser, int *argindex, data_t *data){
    switch(parser->token.type){
        case TOK_IDENTIFIER:               // <identifier>
            // ------------ SEMANTICS START ------------
            if (!data->value.func_id.arguments_defined) { // on funcion call before definition ve save the function params
                if (is_type_next(parser, TOK_COLON)) { // has call name
                    vector_top(data->value.func_id.parameters)->call_name = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1));
                    strcpy(vector_top(data->value.func_id.parameters)->call_name, parser->token.attribute.string);
                    load_token(parser);
                    load_token(parser);
                    return true;
                }
                vector_top(data->value.func_id.parameters)->call_name = malloc(sizeof(char) * 2);
                strcpy(vector_top(data->value.func_id.parameters)->call_name, "_");
                return true;
            }
            // argument is named
            if (strcmp(data->value.func_id.parameters->data[*argindex].call_name, "_")) {
                if (strcmp(parser->token.attribute.string, data->value.func_id.parameters->data[*argindex].call_name)) {
                    error = ERR_SEM_CALL;
                    print_error_and_exit(error);
                    return false; // argument should be named but doesnt match
                }
                load_token(parser);
                if (!is_type(parser, TOK_COLON)){
                    error = ERR_SYN;
                    print_error_and_exit(error);
                    return false;
                }
                load_token(parser);
                return true;
            } else {
                // is not named -> pass over to arg_value
                return true;
            }
            break;
            // ------------ SEMANTICS END ------------
        default:
            // ------------ SEMANTICS START ------------
            // argument is named
            if (strcmp(data->value.func_id.parameters->data[*argindex].call_name, "_")) {
                error = ERR_SEM_CALL;
                print_error_and_exit(error);
                return false; // argument should be named but isnt
            }
            // ------------ SEMANTICS END ------------
            // argument is not named
            return true;               // ε
            break;
    }
}

// <arg_value> -> <identifier> | <int> | <double> | <string> | <nil>

bool rule_arg_value(parser_t *parser, int *argindex, data_t *data, vector_t *call_args){
    // ------------ SEMANTICS START ------------
    variable_type_t param_type = data->value.func_id.parameters->data[*argindex].parameter.type;
    data_t *arg = NULL;
    if (param_type == VAL_TERM){           // built-in write function only
        switch (parser->token.type) {
            case TOK_IDENTIFIER:           // <identifier>
                // check if var exists
                arg = stack_lookup_var(parser->stack, parser->token.attribute.string);
                if (arg == NULL || (!arg->value.var_id.initialized && !arg->value.var_id.func_init)){
                    error = ERR_SEM_NDEF;
                    print_error_and_exit(error);
                    return false;   // Attempt at calling a function with a non-existing variable
                }
                vector_top(call_args)->parameter.type = VAL_ID;
                vector_top(call_args)->parameter.value.string = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1));
                strcpy(vector_top(call_args)->parameter.value.string, parser->token.attribute.string);
                break;
            case TOK_INT:                   // <int>
                vector_top(call_args)->parameter.type = VAL_INT;
                vector_top(call_args)->parameter.value.number = parser->token.attribute.number;
                break;
            case TOK_DOUBLE:                // <double>
                vector_top(call_args)->parameter.type = VAL_DOUBLE;
                vector_top(call_args)->parameter.value.decimal = parser->token.attribute.decimal;
                break;
            case TOK_STRING:                // <string>
                vector_top(call_args)->parameter.type = VAL_STRING;
                vector_top(call_args)->parameter.value.string = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1));
                strcpy(vector_top(call_args)->parameter.value.string, parser->token.attribute.string);
                break;
            case K_NIL:                     // <nil>
                vector_top(call_args)->parameter.type = VAL_NIL;
                break;
            default:
                error = ERR_SYN;
                print_error_and_exit(error);
                return false;
        }

        (*argindex)++;
        load_token(parser);
        return true;
    }
    arg = NULL;
    switch(parser->token.type){    // <identifier> | <int> | <double> | <string> | <nil>
        case TOK_IDENTIFIER:
            if (!data->value.func_id.arguments_defined) { // on funcion call before definition ve save the function params
                vector_top(data->value.func_id.parameters)->parameter.type = stack_lookup_var(parser->stack, parser->token.attribute.string)->value.var_id.type;
                break;
            }
            // check if var exists
            arg = stack_lookup_var(parser->stack, parser->token.attribute.string);
            if (arg == NULL){
                error = ERR_SEM_NDEF;
                print_error_and_exit(error);
                return false;   // Attempt at calling a function with a non-existing variable
            }
            // check correct type (ignore this code block please)
            if (arg->value.var_id.type != param_type &&
                !(arg->value.var_id.type == VAL_DOUBLE && param_type == VAL_DOUBLEQ) &&
                !(arg->value.var_id.type == VAL_INT && param_type == VAL_INTQ) &&
                !(arg->value.var_id.type == VAL_STRING && param_type == VAL_STRINGQ) &&
                !(arg->value.var_id.type == VAL_NIL && param_type == VAL_INTQ) &&
                !(arg->value.var_id.type == VAL_NIL && param_type == VAL_DOUBLEQ) &&
                !(arg->value.var_id.type == VAL_NIL && param_type == VAL_STRINGQ) &&
                !(arg->value.var_id.type == VAL_DOUBLEQ && param_type == VAL_DOUBLE) &&
                !(arg->value.var_id.type == VAL_INTQ && param_type == VAL_INT) &&
                !(arg->value.var_id.type == VAL_STRINGQ && param_type == VAL_STRING)){
                error = ERR_SEM_CALL;
                print_error_and_exit(error);
                return false;   // Attempt at calling a function with a variable of a different type than the function expects
            }
            vector_top(call_args)->parameter.type = VAL_ID;
            vector_top(call_args)->parameter.value.string = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1));
            strcpy(vector_top(call_args)->parameter.value.string, parser->token.attribute.string);
            break;
        case TOK_INT:
            if (param_type != VAL_INT && param_type != VAL_INTQ){
                error = ERR_SEM_CALL;
                print_error_and_exit(error);
                return false;   // Attempt at calling a function with an int argument, where the function expects a different type
            }
            vector_top(call_args)->parameter.type = VAL_INT;
            vector_top(call_args)->parameter.value.number = parser->token.attribute.number;
            break;
        case TOK_DOUBLE:
            if (param_type != VAL_DOUBLE && param_type != VAL_DOUBLEQ){
                error = ERR_SEM_CALL;
                print_error_and_exit(error);
                return false;   // Attempt at calling a function with a double argument, where the function expects a different type
            }
            vector_top(call_args)->parameter.type = VAL_DOUBLE;
            vector_top(call_args)->parameter.value.decimal = parser->token.attribute.decimal;
            break;
        case TOK_STRING:
            if (param_type != VAL_STRING && param_type != VAL_STRINGQ){
                error = ERR_SEM_CALL;
                print_error_and_exit(error);
                return false;   // Attempt at calling a function with a string argument, where the function expects a different type
            }
            vector_top(call_args)->parameter.type = VAL_STRING;
            vector_top(call_args)->parameter.value.string = malloc(sizeof(char) * (strlen(parser->token.attribute.string) + 1));
            strcpy(vector_top(call_args)->parameter.value.string, parser->token.attribute.string);
            break;
        case K_NIL:
            if (param_type != VAL_INTQ && param_type != VAL_DOUBLEQ && param_type != VAL_STRINGQ){
                error = ERR_SEM_CALL;
                print_error_and_exit(error);
                return false;   // Attempt at calling a function with a nil argument, where the function expects a different type
            }
            vector_top(call_args)->parameter.type = VAL_NIL;
            break;
        default:
            error = ERR_SYN;
            print_error_and_exit(error);
            return false;
    }
    

    if (!data->value.func_id.arguments_defined && parser->token.type != TOK_IDENTIFIER) {
        vector_top(data->value.func_id.parameters)->parameter.type = parser->token.type;
    }

    (*argindex)++;
    load_token(parser);
    // ------------ SEMANTICS END ------------
    return true;
}

// <more_arguments> -> "," <argument> <more_arguments> | ε

bool rule_more_arguments(parser_t *parser, int argnum, int *argindex, data_t *data, vector_t *call_args){
    if(is_type(parser, TOK_RBRACKET)){                                      // ε
        return true;
    }

    if (!is_type(parser, TOK_COMMA)){                                       // ","
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }
    load_token(parser);
    // ------------ SEMANTICS START ------------
    if (data->value.func_id.arguments_defined && *argindex >= argnum){
        error = ERR_SEM_CALL;
        print_error_and_exit(error);
        return false;  // Attempt at calling a function with too many arguments
    }
    // ------------ SEMANTICS END ------------
    if (!rule_argument(parser, argindex, data, call_args)){                 // <argument>
        return false;
    }
    if (!rule_more_arguments(parser, argnum, argindex, data, call_args)){   // <more_arguments>
        return false;
    }
    return true;
}

// <return_statement> -> "return" <returned_expression>

bool rule_return_statement(parser_t *parser){
    if (!is_type(parser, K_RETURN)){                // "return"
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;
    }

    if (!parser->in_function){
        error = ERR_SYN;
        print_error_and_exit(error);
        return false;   // Attempt at returning from outside of a function
    }

    // ------------ SEMANTICS START ------------
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
    // ------------ SEMANTICS END ------------

    load_token(parser);
    return rule_returned_expression(parser);    // <returned_expression>
}

// <returned_expression> -> <expression> | ε

bool rule_returned_expression(parser_t *parser){
    if (is_expression(parser)){
        // ------------ SEMANTICS START ------------
        if(parser->func_is_void){
            if(parser->token.eol){
                while(!is_type(parser, TOK_RCURLYBRACKET)){         // ε
                    load_token(parser);
                }
                gen_func_end(parser->gen, parser->func_is_void);
                return true;                                        
            }

            error = ERR_SEM_RETURN;
            print_error_and_exit(error);
            return false;
        }
        // ------------ SEMANTICS END ------------
        variable_type_t type = exp_parsing(parser);                 // <expression>

        if (type != parser->return_type){
            error = ERR_SEM_RETURN;
            print_error_and_exit(error);
            return false;   // Attempt at returning a value of a different type than the function expects
        }
        return true;
    }
    // ------------ SEMANTICS START ------------
    gen_func_end(parser->gen, parser->func_is_void);

    if (!parser->func_is_void){
        error = ERR_SEM_RETURN;
        print_error_and_exit(error);
        return false;
    }
    // ------------ SEMANTICS END ------------

    return true;                                                    // ε
}


//================= GRAMMAR RULES END HERE ================= //

//================= BUILT IN FUNCTIONS ================= //

/*
    func readString() -> String?
    func readInt() -> Int?
    func readDouble() -> Double?
    func write ( term1 , term2 , …, term𝑛 )
    func Int2Double(_ term ∶ Int) -> Double
    func Double2Int(_ term ∶ Double) -> Int
    func length(_ 𝑠 : String) -> Int
    func substring(of 𝑠 : String, startingAt 𝑖 : Int, endingBefore 𝑗 : Int) -> String?
    func ord(_ 𝑐 : String) -> Int
    func chr(_ 𝑖 : Int) -> String
*/
void insert_builtins_to_table(parser_t *parser) {
    data_t data;
    data.type = FUNC;
    data.name = malloc(sizeof(char) * 11);
    strcpy(data.name, "readString");
    data.value.func_id.defined = true;
    data.value.func_id.arguments_defined = true;
    data.value.func_id.return_type = VAL_STRINGQ;
    data.value.func_id.parameters = vector_init(0);
    symbol_table_insert(stack_top_table(parser->stack), "readString", data);

    data.name = malloc(sizeof(char) * 7);
    strcpy(data.name, "readInt");
    data.value.func_id.return_type = VAL_INTQ;
    symbol_table_insert(stack_top_table(parser->stack), "readInt", data);

    data.name  = malloc(sizeof(char) * 10);
    strcpy(data.name, "readDouble");
    data.value.func_id.return_type = VAL_DOUBLEQ;
    symbol_table_insert(stack_top_table(parser->stack), "readDouble", data);

    data_t write;
    write.type = FUNC;
    write.name = malloc(sizeof(char) * 6);
    strcpy(write.name, "write");
    write.value.func_id.defined = true;
    write.value.func_id.arguments_defined = true;
    write.value.func_id.return_type = VAL_VOID;
    write.value.func_id.parameters = vector_init(99);
    for (int i = 0; i < 99; i++) {
        htab_func_param_t p;
        vector_push(write.value.func_id.parameters, p);
        vector_top(write.value.func_id.parameters)->call_name = malloc(sizeof(char) * 2);
        strcpy(vector_top(write.value.func_id.parameters)->call_name, "_");
        vector_top(write.value.func_id.parameters)->parameter.type = VAL_TERM;
    }
    symbol_table_insert(stack_top_table(parser->stack), "write", write);

    data_t int2double;
    int2double.type = FUNC;
    int2double.name = malloc(sizeof(char) * 11);
    strcpy(int2double.name, "Int2Double");
    int2double.value.func_id.defined = true;
    int2double.value.func_id.arguments_defined = true;
    int2double.value.func_id.return_type = VAL_DOUBLE;
    int2double.value.func_id.parameters = vector_init(1);
    vector_push(int2double.value.func_id.parameters, (htab_func_param_t){});
    vector_top(int2double.value.func_id.parameters)->call_name = malloc(sizeof(char) * 2);
    strcpy(vector_top(int2double.value.func_id.parameters)->call_name, "_");
    vector_top(int2double.value.func_id.parameters)->parameter.type = VAL_INT;
    symbol_table_insert(stack_top_table(parser->stack), "Int2Double", int2double);

    data_t double2int;
    double2int.type = FUNC;
    double2int.name = malloc(sizeof(char) * 11);
    strcpy(double2int.name, "Double2Int");
    double2int.value.func_id.defined = true;
    double2int.value.func_id.arguments_defined = true;
    double2int.value.func_id.return_type = VAL_INT;
    double2int.value.func_id.parameters = vector_init(1);
    vector_push(double2int.value.func_id.parameters, (htab_func_param_t){});
    vector_top(double2int.value.func_id.parameters)->call_name = malloc(sizeof(char) * 2);
    strcpy(vector_top(double2int.value.func_id.parameters)->call_name, "_");
    vector_top(double2int.value.func_id.parameters)->parameter.type = VAL_DOUBLE;
    symbol_table_insert(stack_top_table(parser->stack), "Double2Int", double2int);

    data_t length;
    length.type = FUNC;
    length.name = malloc(sizeof(char) * 7);
    strcpy(length.name, "length");
    length.value.func_id.defined = true;
    length.value.func_id.arguments_defined = true;
    length.value.func_id.return_type = VAL_INT;
    length.value.func_id.parameters = vector_init(1);
    vector_push(length.value.func_id.parameters, (htab_func_param_t){});
    vector_top(length.value.func_id.parameters)->call_name = malloc(sizeof(char) * 2);
    strcpy(vector_top(length.value.func_id.parameters)->call_name, "_");
    vector_top(length.value.func_id.parameters)->parameter.type = VAL_STRING;
    symbol_table_insert(stack_top_table(parser->stack), "length", length);

    data_t substring;
    substring.type = FUNC;
    substring.name = malloc(sizeof(char) * 10);
    strcpy(substring.name, "substring");
    substring.value.func_id.defined = true;
    substring.value.func_id.arguments_defined = true;
    substring.value.func_id.return_type = VAL_STRINGQ;
    substring.value.func_id.parameters = vector_init(3);
    vector_push(substring.value.func_id.parameters, (htab_func_param_t){});
    vector_top(substring.value.func_id.parameters)->call_name = malloc(sizeof(char) * 3);
    strcpy(vector_top(substring.value.func_id.parameters)->call_name, "of");
    vector_top(substring.value.func_id.parameters)->parameter.type = VAL_STRING;

    vector_push(substring.value.func_id.parameters, (htab_func_param_t){});
    vector_top(substring.value.func_id.parameters)->call_name = malloc(sizeof(char) * 11);
    strcpy(vector_top(substring.value.func_id.parameters)->call_name, "startingAt");
    vector_top(substring.value.func_id.parameters)->parameter.type = VAL_INT;


    vector_push(substring.value.func_id.parameters, (htab_func_param_t){});
    vector_top(substring.value.func_id.parameters)->call_name = malloc(sizeof(char) * 13);
    strcpy(vector_top(substring.value.func_id.parameters)->call_name, "endingBefore");
    vector_top(substring.value.func_id.parameters)->parameter.type = VAL_INT;
    symbol_table_insert(stack_top_table(parser->stack), "substring", substring);

    data_t ord;
    ord.type = FUNC;
    ord.name = malloc(sizeof(char) * 4);
    strcpy(ord.name, "ord");
    ord.value.func_id.defined = true;
    ord.value.func_id.arguments_defined = true;
    ord.value.func_id.return_type = VAL_INT;
    ord.value.func_id.parameters = vector_init(1);
    vector_push(ord.value.func_id.parameters, (htab_func_param_t){});
    vector_top(ord.value.func_id.parameters)->call_name = malloc(sizeof(char) * 2);
    strcpy(vector_top(ord.value.func_id.parameters)->call_name, "_");
    vector_top(ord.value.func_id.parameters)->parameter.type = VAL_STRING;
    symbol_table_insert(stack_top_table(parser->stack), "ord", ord);

    data_t chr;
    chr.type = FUNC;
    chr.name = malloc(sizeof(char) * 4);
    strcpy(chr.name, "chr");
    chr.value.func_id.defined = true;
    chr.value.func_id.arguments_defined = true;
    chr.value.func_id.return_type = VAL_STRINGQ;
    chr.value.func_id.parameters = vector_init(1);
    vector_push(chr.value.func_id.parameters, (htab_func_param_t){});
    vector_top(chr.value.func_id.parameters)->call_name = malloc(sizeof(char) * 2);
    strcpy(vector_top(chr.value.func_id.parameters)->call_name, "_");
    vector_top(chr.value.func_id.parameters)->parameter.type = VAL_INT;
    symbol_table_insert(stack_top_table(parser->stack), "chr", chr);
}

//================= EOF ================= //
