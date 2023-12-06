/**
 *  @file str.h
 * 
 * 
 *  
 *  @authors Jiri Tesar (xtesar44)
 */

#ifndef STR_H
#define STR_H

extern const int STR_CHUNK; 

/**
 *  @brief Struct for dynamic string.
 */
typedef struct mystring
{
    char *string;
    int lenght; 
    int capacity;
}mystring_t;

/**
 *   @brief Add one char to string, if lenght is equal as capacity,
 *          call func addcap.
 *   @param str structer for dynamic string 
 *   @param input char who add to dynamic string
 */ 
void makestr(mystring_t *str, char input);

/**
 *   @brief inicialization structere 
 *   @param str structer for dynamic string 
 */ 
void initstr(mystring_t *str);

/**
 *   @brief destroy structere 
 *   @param str structer for dynamic string 
 */ 
void destroy(mystring_t *str);

/**
 *   @brief increases capacity of the strucure 
 *   @param str structer for dynamic string 
 */ 
void addcap(mystring_t *str);


/**
 *   @brief merge two strings 
 *   @param str1 structer for dynamic string 
 *   @param str2 string we want to merge
 */ 
void mergestr(mystring_t *str1, char *str2);

/**
 *   @brief merge string with int
 *   @param str1 structer for dynamic string 
 *   @param input int we want to merge
 */
void mergestr_int(mystring_t *str1, int input);

/**
 *   @brief merge string with float
 *   @param str1 structer for dynamic string 
 *   @param input float we want to merge
 */
void mergestr_float(mystring_t *str1, double input);


/**
 *   @brief print the final string
 *   @param str string to printout
 */
void printstr(mystring_t *str);


/**
 *   @brief Remove char from string
 *   @param str string from which the char will be removed
 *   @param index index of the removed character 
 */
void removechar(mystring_t *str, int index);

#endif

/* end of file str.h */
