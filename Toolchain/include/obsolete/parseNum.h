#ifndef PARSE_NUM_H
#define PARSE_NUM_H
#include <stdbool.h>

extern const char* pnum_parse_err;
extern const char* PNUM_ERR_MISMATCHED_OPEN;
extern const char* PNUM_ERR_MISMATCHED_CLOSE;
extern const char* PNUM_ERR_UNPARSEABLE;

/**
 * parseInt()
 * Takes a string representing a mathematical equation and returns an integer representing the result.
 * Upon failure, a failure message is placed in pnum_parse_err.
 * 
 * Parameters:
 * int* to: Pointer to a location in which to store the result. Not modified on failure.
 * const char* str: Source string to be parsed.
 * 
 * Returns: (bool) true if the string was parsed successfully, false otherwise
*/
bool parseInt(int* to, const char* str);

#endif