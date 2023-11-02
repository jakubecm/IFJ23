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

extern error_t error;

const int STR_CHUNK = 10;

void makestr(mystring_t *str, char input){
    if(str->lenght == str->capacity){
        addcap(str);
    }

    str->string[str->lenght] = input;
    str->lenght++;
}


void initstr(mystring_t *str){
    str->string = NULL;
    str->lenght = 0;
    str->capacity = STR_CHUNK;

    str->string = (char*)malloc(str->capacity * sizeof(char));
    if(str->string == NULL){
        error = ERR_INTERNAL;
    }
}

void addcap(mystring_t *str){
    str->capacity += STR_CHUNK;

    str->string = realloc(str->string,str->capacity * sizeof(char));
    if(str->string == NULL){
        error = ERR_INTERNAL;
    }
}

void destroy(mystring_t *str){
    free(str->string);

    str->lenght = 0;
    str->capacity = 0;
    str->string = NULL;
}

/* end of file str.c */