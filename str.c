/**
 *  @file str.c
 * 
 * 
 *  
 *  @authors Jiri Tesar (xtesar44)
 */

#include "str.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern error_t error;

const int STR_CHUNK = 10;

void makestr(mystring_t *str, char input){
    if(str->lenght == str->capacity){
        addcap(str);
    }

    str->string[str->lenght] = input;
    str->lenght++;
    str->string[str->lenght] = '\0';
}


void initstr(mystring_t *str){
    str->string = NULL;
    str->lenght = 0;
    str->capacity = STR_CHUNK;

    str->string = (char*)malloc(str->capacity * sizeof(char));
    if(str->string == NULL){
        error = ERR_INTERNAL;
        print_error_and_exit(error);
    }
}

void addcap(mystring_t *str){
    str->capacity += STR_CHUNK;

    str->string = realloc(str->string,str->capacity * sizeof(char));
    if(str->string == NULL){
        error = ERR_INTERNAL;
        print_error_and_exit(error);
    }
}

void destroy(mystring_t *str){
    free(str->string);

    str->lenght = 0;
    str->capacity = 0;
    str->string = NULL;
}

void mergestr(mystring_t *str1, char *str2){
    int lenght;
    
    lenght = strlen(str2);
    str1->capacity += lenght;
    str1->lenght += lenght;
    str1->string = realloc(str1->string,str1->capacity * sizeof(char));

    if(str1->string == NULL){
        error = ERR_INTERNAL;
        print_error_and_exit(error);
    }

    strcat(str1->string,str2);
    str1->string[str1->lenght] = '\0';
}

void mergestr_int(mystring_t *str1, int input){
    char str2[21];
    sprintf(str2, "%d", input);
    mergestr(str1, str2);
}

void mergestr_float(mystring_t *str1, float input){
    char str2[35];
    sprintf(str2, "%f", input);
    mergestr(str1, str2);
}

void printstr(mystring_t *str){
    printf("%s", str->string);
}
/* end of file str.c */