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
    int lastchar;
    int whitespace = 0;
    int blockcomm;
    
    token.attribute.string = NULL;
    token.eol = false;

    // Main loop for reading characters and determining tokens
    while(true){
        if(state == COMMENT || state == START){
            inchar = getchar();
        }

        whitespace--;         
        
        // Check the end of input
        if(inchar == EOF){
            token.type = TOK_EOF;
            return token;
        }

        // Check if token is on the new line
        if(inchar == '\n'){
            token.eol = true;
        }

        // Switch for recognizing tokens
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
            
            // If no matching case is found, it's an error
            else{
                error = ERR_LEX;
                print_error_and_exit(error);
            }
            break;


        // State for singleline comment
        case(COMMENT):
            {
                while((inchar != '\n') && (inchar != EOF)){
                    inchar = getchar();
                }
    
                state = START;
                break;
            }


        // State for multiline comment
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


        // State for number
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


        // State for decimal number 
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

        
        // State for number with exponent
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


        // State for identifier
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


        // State for singleline string
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


        // State for multiline string
        case(MSTRING):
            initstr(&mstr);
            inchar = getchar();

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

                    if(inchar == '"' && (preinchar == 10 || preinchar == 32)){
                        
                        lastchar = preinchar;
                        preinchar = inchar;
                        inchar = getchar();
                        if(inchar == '"'){
                            
                            preinchar = inchar;
                            inchar = getchar();
                            if(inchar == '"'){
                                makestr(&mstr, lastchar);
                                spaceignor(&mstr);
                                tokinit(&token,mstr.lenght);
                                token.attribute.string = mstr.string;
                                token.type = TOK_MLSTRING;
                                return token;
                            }

                            else{
                                makestr(&mstr,lastchar);
                                makestr(&mstr,preinchar);
                                makestr(&mstr,preinchar);
                            }
                        }

                        else{
                            makestr(&mstr,lastchar);
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

            error = ERR_LEX;
            print_error_and_exit(error);

        default:

            break;
        }
    }
}


void backslash(mystring_t *str){
    mystring_t hexanum;
    int hexachar;

    initstr(&hexanum);
    int inchar = getchar();

    if(inchar == '\\' || inchar == 'n' ||inchar == '"' ||inchar == 't' ||inchar == 'r'){
        removechar(str,str->lenght);
        switch(inchar){
            case('\\'):
                makestr(str,'\\');
                break;
            
            case('n'):
                makestr(str,'\n');
                break;

            case('"'):
                makestr(str,'\"');
                break;

            case('t'):
                makestr(str,'\t');
                break;

            case('r'):
                makestr(str,'\r');
                break;
        }

    }

    else if(inchar == 'u'){
        removechar(str,str->lenght);
        inchar = getchar();

        if(inchar == '{'){
            int count = 0;
            makestr(&hexanum,'0');
            makestr(&hexanum,'x');
            inchar = getchar();
            
            // Read and validate hexadecimal characters
            if((inchar >= '0' && inchar <= '9') || (inchar >= 'a' && inchar <= 'z') || (inchar >= 'A' && inchar <= 'Z')){
                do{
                    if(count > 8){
                        break;
                    }
                    makestr(&hexanum,inchar); 
                    inchar = getchar();
                    count++;
                }while(((inchar >= 'a' && inchar <= 'f') || (inchar >= 'A' && inchar <= 'F') || (inchar >= '0' && inchar <= '9')) && (inchar != EOF));

                if(inchar == '}'){
                    hexachar = strtol(hexanum.string, NULL, 16);

                    makestr(str,(char)hexachar); 
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

void spaceignor(mystring_t *str){
    int lasteol;
    int space = 0;

    // Find the last end-of-line character in the string
    for(int i = 0; i < str->lenght; i++){
        if(str->string[i]== 10){
            lasteol = i;
        }
    }

    // Count spaces after the last end-of-line character
    for(int i = lasteol + 1; i < str->lenght;i++){
        if(str->string[i] == 32){
            space++; 
        }

        else{
            error = ERR_LEX;
            print_error_and_exit(error);
        }
    }

    // Remove space after end of line character
    for(int i = 0; i < str->lenght; i++){
        if(str->string[i]== 10 && space != 0){
            for(int j = space; j > 0; j--){
                if(str->string[i + 1] == 32){
                    removechar(str,i+1);
                }

                else if(str->string[i + 1] == 10){
                    continue;
                }

                else{
                    error = ERR_LEX;
                    print_error_and_exit(error);
                }
            }
            
        }
    }

    removechar(str,str->lenght);
}

/* end of file scanner.c */
