/**
 *  @file str.h
 * 
 * 
 *  
 *  @authors Jiri Tesar (xtesar44)
 */

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

/* end of file str.h */