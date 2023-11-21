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

extern error_t error;

void myungetc(int inchar){
    ungetc(inchar,stdin);
}


token_t get_next_token(){  
    token_t token;
    mystring_t mstr; 
    mystring_t str;   
    mystring_t id;
    mystring_t number;
    
    int state = START;
    int inchar;
    int preinchar;
    int whitespace = 0;
    int blockcomm;
    
    token.attribute.string = NULL;
    token.eol = false;

    while(true){
        if(state == COMMENT || state == START){
            inchar = getchar();
        }

        whitespace--;         
        
        if(inchar == EOF){
            token.type = TOK_EOF;
            return token;
        }

        if(inchar == '\n'){
            token.eol = true;
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
                whitespace = 2;
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
                    if(whitespace != 1){
                        myungetc(inchar);
                        token.type = TOK_NOT;
                        return token;
                    }
                    else{
                        error = ERR_LEX;
                        print_error_and_exit(error);
                    }
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

                    if(inchar == EOF){
                        error = ERR_LEX;
                        print_error_and_exit(error);
                        break;
                    }
                    
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
                        preinchar = inchar;
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
                print_error_and_exit(error);
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
            blockcomm = 1;

            while(inchar != EOF){
                if(inchar == '/'){
                    inchar = getchar();

                    if(inchar == '*'){
                        blockcomm++;
                        inchar = getchar();
                    }
                }

                if(inchar == '*'){
                    inchar = getchar();
                    
                    if(inchar == '*'){
                        continue;
                    }

                    if(inchar == '/'){
                        blockcomm--;

                        if(blockcomm == 0){
                            state = START;
                            break;
                        }
                    }
                }

                inchar = getchar();
            }

            if(blockcomm == 0){
                state = START;
                break;
            }
            
            else{
                error = ERR_LEX;
                print_error_and_exit(error);
                break;
            }      


        case(NUM):
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
                    print_error_and_exit(error);
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
                    print_error_and_exit(error);
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
                    print_error_and_exit(error);
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

            if(inchar >= '0' && inchar <= '9'){
                do{
                    makestr(&number,inchar);
                    inchar = getchar();
                }while(inchar >= '0' && inchar <= '9');
            }

            else{
                error = ERR_LEX;
                print_error_and_exit(error);
            }
            
      
            myungetc(inchar);

            double dnum = atof(number.string);
            int inum;

            inum = (int) dnum;

            if((dnum - inum) == 0){
                token.attribute.number = inum;
                token.type = TOK_INT;
                
                return token;
            }

            else{
                token.attribute.decimal = dnum;
                token.type = TOK_DOUBLE;

                return token;
            }


        case(ID):
            initstr(&id);

            if(preinchar == '_'){
                makestr(&id,preinchar);
            }

            do{
                makestr(&id,inchar);
                inchar = getchar();
            }while(inchar != EOF && ((inchar >= 'a' && inchar <= 'z') || (inchar >= 'A' && inchar <= 'Z') || (inchar >= '0' && inchar <= '9') || inchar == '_'));
            
            myungetc(inchar);

            tokinit(&token,id.lenght);
            token.type = TOK_IDENTIFIER;
            token.attribute.string = id.string;

            iskeyw(&token);
            return token;


        case(STRING):
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
                print_error_and_exit(error);
                break;
            }


        case(MSTRING):
            initstr(&mstr);
            inchar = getchar();
            char newline = '\n';

            if(inchar == '\n'){
                do{
                    preinchar = inchar;
                    inchar = getchar();

                    if(inchar == '\\'){                       
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
                print_error_and_exit(error);
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
                    if(count > 8){
                        break;
                    }
                    makestr(str,inchar); 
                    inchar = getchar();
                    count++;
                }while(((inchar >= 'a' && inchar <= 'f') || (inchar >= 'A' && inchar <= 'F') || (inchar >= '0' && inchar <= '9')) && (inchar != EOF));

                if(inchar == '}'){
                    makestr(str,inchar); 
                }

                else{
                    error = ERR_LEX;
                    print_error_and_exit(error);
                }
            }

            else{
                error = ERR_LEX;
                print_error_and_exit(error);
            }
         
        }

        else{
            error = ERR_LEX;
            print_error_and_exit(error);
        }
        
    }

    else{
        error = ERR_LEX;
        print_error_and_exit(error);
    }
}

/* end of file scanner.c */