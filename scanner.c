/**
 *  @file scanner.c
 * 
 * 
 *  @authors Jiri Tesar (xtesar44)
 */

#include "token.h"
#include "scanner.h"
#include <stdio.h>


int myungetc(int inchar){
    ungetc(inchar,stdin);
}


token_t get_next_token(){  
    token_t token;    
    int inchar = getchar();

    if(inchar == EOF){
        token.type = TOK_EOF;
        return token;
    }
    
    int state = START;

    switch(state){

        case(START):

            if(inchar == '+'){
                token.type = TOK_PLUS;
                return token;
            }

            else if(inchar == ':'){
                token.type = TOK_COLON;
                return token;

            //TODO
            }
    }
}