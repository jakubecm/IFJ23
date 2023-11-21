/**
 *  @file buildin.h
 *
 *
 *  @authors Milan Jakubec (xjakub41)
 */

#include "gen.h"

/**
 * @brief Generate all buildin functions
 * Takes all buildin functions and generates them in gen header.
 * @param gen Generator
*/
void gen_buildin_func(gen_t *gen);

/**
 * @brief function readString() -> String?
 * Reads a string from the standard input without the trailing newline character.
 * Loaded string does not support escape sequences.
 * @param gen Generator
 */
void gen_func_readstring(gen_t *gen);

/**
 * @brief function readInt() -> Int?
 * Reads an integer from the standard input.
 * @param gen Generator
 */
void gen_func_readint(gen_t *gen);

/**
 * @brief function readDouble() -> Double?
 * Reads a double/float from the standard input.
 * @param gen Generator
 */
void gen_func_readdouble(gen_t *gen);

/**
 * @brief function write(term1, term2, ..., termN)
 * Goes through all terms from left to right and prints them to the standard output without any formatting.
 * Nothing is printed after the last term.
 * Int value will be printed with '%d' format specifier.
 * Double value will be printed with '%a' format specifier.
 * Nil value will be printed as empty string.
 * This function returns no value.
 * @param gen Generator
*/
void gen_func_write(gen_t *gen);

/**
 * @brief function Int2Double(_term: Int) -> Double
 * Returns a double/float representation of the given integer.
 * Need to use correct instruction from IFJcode23.
 * @param gen Generator
 */
void gen_func_Int2Double(gen_t *gen);

/**
 * @brief function Double2Int(_ term: Double) -> Int
 * Returns an integer representation of the given double/float by cutting the decimal part.
 * Need to use correct instruction from IFJcode23.
 * @param gen Generator
 */
void gen_func_Double2Int(gen_t *gen);

/**
 * @brief function Length(_ s: String) -> Int
 * Returns the length (the amount of characters) of the given string.
 * length("x/nz") returns 3
 * @param gen Generator
 */
void gen_func_length(gen_t *gen);

/**
 * @brief function substring(of s: String, startingAt i : Int, endingBefore j : Int) -> String?
 * Returns a substring of the given string "s".
 * Second parameter "i" is the starting index of the substring (inclusive).
 * Third parameter "j" is the ending index of the substring (exclusive).
 * Function return nil in the following cases:
 * i < 0
 * j < 0
 * i > j
 * i >= length(s)
 * j > length(s) 
*/
void gen_func_substr(gen_t *gen);

/**
 * @brief function ord(_ c: String) -> Int
 * Returns ordinal value (ASCII) of the first character of the given string.
 * If the string is empty, the function returns 0
*/
void gen_func_ord(gen_t *gen);

/**
 * @brief function chr(_ i: Int) -> String
 * Returns a string of length 1, which contains the character with the given ordinal value (ASCII) in parameter i.
 * Value i outside of the boundaries 0..255 is solved by an instruction of IFJCODE23.
*/
void gen_func_chr(gen_t *gen);