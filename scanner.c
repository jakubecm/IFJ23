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

error_t error;

int myungetc(int inchar){
    ungetc(inchar,stdin);
}


token_t get_next_token(){  
    token_t token;    
    
    int state = START;
    int inchar;
    int preinchar;

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

            else if(inchar == '_' || (inchar >= 'a' && inchar <= 'z')|| (inchar >= 'A' && inchar <= 'Z')){
                
                if(inchar == '_'){
                    preinchar = inchar;
                    inchar = getchar();

                    if(inchar == '_' || (inchar >= 'a' && inchar <= 'z') || (inchar >= 'A' && inchar <= 'Z') || (inchar >= '0' && inchar <= '9')){            
                        state = ID;
                    }

                    else{
                        myungetc(inchar);
                        token.type = TOK_UNDERSCORE;
                        return token;
                    }
                }

                else{
                    state = ID;
                }
            }

            else if(inchar == '"'){
                state = STRING;
            }
            
            else{
                error = ERR_LEX;
            }
            break;


        case(COMMENT):
            {
                while((inchar != '\n') && (inchar != EOF)){
                    inchar = getchar();
                }
    
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
                if(!(inchar >= '0' && inchar <= '9')){
                    error = ERR_LEX;
                }
            }

            else if((inchar == 'e') || (inchar == 'E')){
                makestr(&number,inchar);
                inchar = getchar();

                if((inchar >= '0' && inchar <= '9') || (inchar == '+' || inchar == '-')){
                    state = ENUM;
                }

                else{
                    error = ERR_LEX;
                }
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
            }while(inchar >= '0' && inchar <= '9');      

            if((inchar == 'e') || (inchar == 'E')){
                makestr(&number,inchar);
                inchar = getchar();

                if((inchar >= '0' && inchar <= '9') || (inchar == '+' || inchar == '-')){
                    state = ENUM;
                }

                else{
                    error = ERR_LEX;
                }
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
            }while(inchar >= '0' && inchar <= '9');
      
            myungetc(inchar);
            token.attribute.decimal = atof(number.string);
            token.type = TOK_DOUBLE;
            return token;

            break;

        case(ID):
            mystring_t id;
            initstr(&id);

            if(preinchar == '_'){
                makestr(&id,preinchar);
            }

            do{
                makestr(&id,inchar);
                inchar = getchar();
            }while(inchar == '_' || (inchar >= 'a' && inchar <= 'z')|| (inchar >= 'A' && inchar <= 'Z') || (inchar >= '0' && inchar <= '9') && inchar == EOF);
            
            myungetc(inchar);

            tokinit(&token,id.lenght);
            token.type = TOK_IDENTIFIER;
            token.attribute.string = id.string;

            iskeyw(&token);
            return token;


        case(STRING):
            mystring_t str;
            initstr(&str);

            do{
                if(inchar != '"'){
                    makestr(&str,inchar);
                }
                
                if(inchar == '\\'){
                    backslash(&str);
                }
                
                inchar = getchar();

                if(inchar == '"'){
                    inchar = getchar();

                    if(inchar == '"'){
                        state = MSTRING;
                        break;
                    }

                    else{
                        myungetc(inchar);
                        tokinit(&token,str.lenght);
                        token.attribute.string = str.string;
                        token.type = TOK_STRING;
                        return token;
                    }
                    
                }

            }while(inchar >= 32 && inchar <= 255);

            if(inchar == '"' && state != MSTRING){
                tokinit(&token,str.lenght);
                token.attribute.string = str.string;
                token.type = TOK_STRING;
                return token;
            }

            else if(state == MSTRING){
                break;
            }

            else{
                error = ERR_LEX;
            }


        case(MSTRING):
            mystring_t mstr;
            initstr(&mstr);
            inchar = getchar();
            char newline = '\n';

            if(inchar == '\n'){
                do{
                    //makestr(&mstr,inchar);
                    preinchar = inchar;
                    inchar = getchar();

                    if(inchar == '\\'){                       
                        //myungetc(inchar);
                        if(preinchar != '\\'){
                            makestr(&mstr,preinchar);
                        }
                        
                        makestr(&mstr,inchar);
                        backslash(&mstr);
                        
                        continue;
                    }

                    if(inchar == '"' && preinchar == 10){
                        
                        preinchar = inchar;
                        inchar = getchar();
                        if(inchar == '"'){
                            
                            preinchar = inchar;
                            inchar = getchar();
                            if(inchar == '"'){
                                tokinit(&token,mstr.lenght);
                                token.attribute.string = mstr.string;
                                token.type = TOK_MLSTRING;
                                return token;
                            }

                            else{
                                makestr(&mstr,newline);
                                makestr(&mstr,preinchar);
                                makestr(&mstr,preinchar);
                            }
                        }

                        else{
                            makestr(&mstr,newline);
                            makestr(&mstr,preinchar);
                        }
                    }    
                    
                    else{
                        
                        if(preinchar == '\\'){
                            continue;
                        }
                        
                        makestr(&mstr,preinchar);
                    }

                                     
                }while((inchar >= 32 && inchar <= 255)|| inchar == '\n');
            }

            else{
                error = ERR_LEX;
            }

        default:

            break;
        }
    }
}

void backslash(mystring_t *str){
    int inchar = getchar();

    if(inchar == '\\' || inchar == 'n' ||inchar == '"' ||inchar == 't' ||inchar == 'r'){
        makestr(str,inchar);
    }

    else if(inchar == 'u'){
        makestr(str,inchar);
        inchar = getchar();

        if(inchar == '{'){
            int count = 0;
            makestr(str,inchar); 
            inchar = getchar();
            if((inchar >= '0' && inchar <= '9') || (inchar >= 'a' && inchar <= 'z') || (inchar >= 'A' && inchar <= 'Z')){
                do{
                    makestr(str,inchar); 
                    inchar = getchar();
                    count++;
                }while(count < 8 && ((inchar >= 'a' && inchar <= 'f') || (inchar >= 'A' && inchar <= 'F') || (inchar >= '0' && inchar <= '9') && inchar != EOF));

                if(inchar == '}'){
                    makestr(str,inchar); 
                }

                else{
                    error = ERR_LEX;
                }
            }

            else{
                error = ERR_LEX;
            }
         
        }

        else{
            error = ERR_LEX;
        }
        
    }

    else{
        error = ERR_LEX;
    }
}

/* end of file scanner.c */