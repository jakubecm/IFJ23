/**
 *  token.h and shit
 *
 *
 * @authors Jiri Tesar (xtesar44)
 */

#include "token.h"
#include "str.h"
#include "scanner.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern error_t error;

void tokinit(token_t *token, int lenght){
    token->attribute.string = (char*)malloc(lenght * sizeof(char));
    if(token->attribute.string == NULL){
        error = ERR_INTERNAL;
    }
}

void tokdestroy(token_t *token){
    free(token->attribute.string);
    token->attribute.string = NULL;
}

bool iskeyw(token_t *token){
    char inchar;

    if(token->attribute.string != NULL){
            if(strcmp(token->attribute.string, "if") == 0){
            token->type = K_IF;
            return true;
        }

        else if(strcmp(token->attribute.string, "else") == 0){
            token->type = K_ELSE;
            return true;
        }

        else if(strcmp(token->attribute.string, "var") == 0){
            token->type = K_VAR;
            return true;
        }

        else if(strcmp(token->attribute.string, "while") == 0){
            token->type = K_WHILE;
            return true;
        }

        else if(strcmp(token->attribute.string, "let") == 0){
            token->type = K_LET;
            return true;
        }

        else if(strcmp(token->attribute.string, "nil") == 0){
            token->type = K_NIL;
            return true;
        }

        else if(strcmp(token->attribute.string, "func") == 0){
            token->type = K_FUNC;
            return true;
        }

        else if(strcmp(token->attribute.string, "Double") == 0){
            inchar = getchar();

            if(inchar == '?'){
                token->type = K_DOUBLEQ;
                return true;
            }

            else if(inchar == '!'){
                token->type = K_DOUBLEE;
                return true;
            }

            else{
                myungetc(inchar);
                token->type = K_DOUBLE;
                return true;
            }
        }

        else if(strcmp(token->attribute.string, "Int") == 0){
            inchar = getchar();

            if(inchar == '?'){
                token->type = K_INTQ;
                return true;
            }

            else if(inchar == '!'){
                token->type = K_INTE;
                return true;
            }

            else{
                myungetc(inchar);
                token->type = K_INT;
                return true;
            }
        }

        else if(strcmp(token->attribute.string, "String") == 0){
            inchar = getchar();

            if(inchar == '?'){
                token->type = K_STRINGQ;
                return true;
            }

            else if(inchar == '!'){
                token->type = K_STRINGE;
                return true;
            }

            else{
                myungetc(inchar);
                token->type = K_STRING;
                return true;
            }
        }

        else if(strcmp(token->attribute.string, "return") == 0){
            token->type = K_RETURN;
            return true;
        }

        else{
            return false;
        }
    }

    else{
        return false;
    }
}