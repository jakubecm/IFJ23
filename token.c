/**
 *  token.h and shit
 *
 *
 * @authors Jiri Tesar (xtesar44)
 */

#include "token.h"
#include "str.h"
#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void tokinit(token_t *token, int lenght){
    token->attribute.string = (char*)malloc(lenght * sizeof(char));
    if(token->attribute.string == NULL){
        fprintf(stderr,"memory_error");
        exit(1);
    }
}

void tokdestroy(token_t *token){
    free(token->attribute.string);
    token->attribute.string = NULL;
}

void iskeyw(token_t *token){
    char inchar;

    if(strcmp(token->attribute.string, "if") == 0){
        token->type = K_IF;
    }

    else if(strcmp(token->attribute.string, "else") == 0){
        token->type = K_ELSE;
    }

    else if(strcmp(token->attribute.string, "var") == 0){
        token->type = K_VAR;
    }

    else if(strcmp(token->attribute.string, "while") == 0){
        token->type = K_WHILE;
    }

    else if(strcmp(token->attribute.string, "let") == 0){
        token->type = K_LET;
    }

    else if(strcmp(token->attribute.string, "nil") == 0){
        token->type = K_NIL;
    }

    else if(strcmp(token->attribute.string, "func") == 0){
        token->type = K_FUNC;
    }

    else if(strcmp(token->attribute.string, "Double") == 0){
        inchar = getchar();

        if(inchar == '?'){
            token->type = K_DOUBLEQ;
        }

        else if(inchar == '!'){
            token->type = K_DOUBLEE;
        }

        else{
            myungetc(inchar);
            token->type = K_DOUBLE;
        }
    }

    else if(strcmp(token->attribute.string, "Int") == 0){
        inchar = getchar();

        if(inchar == '?'){
            token->type = K_INTQ;
        }

        else if(inchar == '!'){
            token->type = K_INTE;
        }

        else{
            myungetc(inchar);
            token->type = K_INT;
        }
    }

    else if(strcmp(token->attribute.string, "String") == 0){
        inchar = getchar();

        if(inchar == '?'){
            token->type = K_STRINGQ;
        }

        else if(inchar == '!'){
            token->type = K_STRINGE;
        }

        else{
            myungetc(inchar);
            token->type = K_STRING;
        }
    }

    else if(strcmp(token->attribute.string, "return") == 0){
        token->type = K_RETURN;
    }
}