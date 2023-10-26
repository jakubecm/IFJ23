/**
 *  @file scanner.c
 * 
 * 
 *  @authors Jiri Tesar (xtesar44)
 */

#include "token.h"
#include "scanner.h"
#include "error.h"
#include "str.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>


int myungetc(int inchar){
    ungetc(inchar,stdin);
}


token_t get_next_token(){  
    token_t token;    
    
    int state = START;
    int inchar;

    while(true){
        if(state == COMMENT || state == START){
            inchar = getchar();
        }
        
        
        if(inchar == EOF){
            token.type = TOK_EOF;
            return token;
        }

        switch(state){

        case(START):

            if(inchar == '+'){
                token.type = TOK_PLUS;
                return token;
            }

            else if(inchar == ':'){
                token.type = TOK_COLON;
                return token;
            }
            
            else if(inchar == '('){
                token.type = TOK_LBRACKET;
                return token;
            }

            else if(inchar == ')'){
                token.type = TOK_RBRACKET;
                return token;
            }

            else if(inchar == '*'){
                token.type = TOK_MUL;
                return token;
            }

            else if(inchar == ','){
                token.type = TOK_COMMA;
                return token;
            }

            else if(inchar == '{'){
                token.type = TOK_LCURLYBRACKET;
                return token;
            }

            else if(inchar == '}'){
                token.type = TOK_RCURLYBRACKET;
                return token;
            }

            else if(isspace(inchar)){
                state = START;
            }

            else if(inchar == '-'){
                inchar = getchar();

                if(inchar == '>'){
                    token.type = TOK_ARROW;
                    return token;
                }

                else{
                    myungetc(inchar);
                    token.type = TOK_MINUS;
                    return token;
                }
            }

            else if(inchar == '<'){
                inchar = getchar();

                if(inchar == '='){
                    token.type = TOK_LESSEQ;
                    return token;
                }

                else{
                    myungetc(inchar);
                    token.type = TOK_LESS;
                    return token;
                }
            }

            else if(inchar == '>'){
                inchar = getchar();

                if(inchar == '='){
                    token.type = TOK_GREATEREQ;
                    return token;
                }

                else{
                    myungetc(inchar);
                    token.type = TOK_GREATER;
                    return token;
                }
            }

            else if(inchar == '='){
                inchar = getchar();

                if(inchar == '='){
                    token.type = TOK_EQUAL;
                    return token;
                }

                else{
                    myungetc(inchar);
                    token.type = TOK_ASSIGNMENT;
                    return token;
                }
            }

            else if(inchar == '!'){
                inchar = getchar();

                if(inchar == '='){
                    token.type = TOK_NOTEQUAL;
                    return token;
                }

                else{
                    myungetc(inchar);
                    token.type = TOK_NOT;
                    return token;
                }
            }

            else if(inchar == '?'){
                inchar = getchar();

                if(inchar == '?'){
                    token.type = TOK_DQUESTMK;
                    return token;
                }

                else{
                    myungetc(inchar);
                    token.type = TOK_QUESTMK;
                    return token;
                }
            }

            else if(inchar == '/'){
                inchar = getchar();

                if(inchar == '/'){
                    state = COMMENT;
                    printf("prechod do comment\n");
                }

                else if(inchar == '*'){
                    inchar = getchar();
                    state = MCOMMENT;
                }

                else{
                    myungetc(inchar);
                    token.type = TOK_DIV;
                    return token;
                }
            }

            
            else if(inchar >= '0' && inchar <= '9'){
                state = NUM;
            }
            
            break;
            
        
        case(COMMENT):
            {
                while((inchar != '\n') && (inchar != EOF)){
                    inchar = getchar();
                }
                printf("konec komentare\n");
                state = START;
                break;
            }

        case(MCOMMENT):
            //TODO zanorene komentare a neukoncene komentare
            while(inchar != '*'){
                inchar = getchar();
            }

            inchar = getchar();

            if(inchar != '/'){
                state = MCOMMENT;
            }

            else{
                state = START;
            }
            break;

        case(NUM):
            mystring_t number;
            initstr(&number);
            
            do{
                makestr(&number,inchar);
                inchar = getchar();
            }
            while(inchar >= '0' && inchar <= '9');

            if(inchar == '.'){
                makestr(&number,inchar);
                inchar = getchar();
                state = DNUM;
            }

            else if((inchar == 'e') || (inchar == 'E')){
                makestr(&number,inchar);
                inchar = getchar();
                state = ENUM;
            }

            else{
                myungetc(inchar);
                token.attribute.number = atoi(number.string);
                token.type = TOK_INT;
                return token;
            }
            break;
            
        case(DNUM):
            do{
                makestr(&number,inchar);
                inchar = getchar();
            }
            while(inchar >= '0' && inchar <= '9');
            
            if((inchar == 'e') || (inchar == 'E')){
                makestr(&number,inchar);
                inchar = getchar();
                state = ENUM;
            }

            else{
                myungetc(inchar);
                token.attribute.decimal = atof(number.string);
                token.type = TOK_DOUBLE;
                return token;
            }
            break;
        
        case(ENUM):
            if((inchar == '+') || (inchar == '-')){
                makestr(&number,inchar);
                inchar = getchar();
            }

            do{
                makestr(&number,inchar);
                inchar = getchar();
            }
            while(inchar >= '0' && inchar <= '9');
      
            myungetc(inchar);
            token.attribute.decimal = atof(number.string);
            token.type = TOK_DOUBLE;
            return token;

            break;

        default:    
            break;
        }
    }
}