#ifndef IOTOOLS_H
#define IOTOOLS_H

#include <stdio.h>

/**
 * Returns the size in bytes of a file.
*/
size_t fsize(FILE* stream);

/**
 * Reads a single line from a text file or stream into a heap-allocated string.
 * Returns a pointer to the string on success or NULL on failure. On success,
 * the returned string is guaranteed to be null-terminated.
 * 
 * Note: will only return NULL if heap allocation fails; as yet does not check
 * for I/O errors.
*/
char* fgets_dup(FILE* stream);

/**
 * Reads the entire contents of a text file into a heap-allocated string.
 * Returns a pointer to the string on success or NULL on failure. On success,
 * the returned string is guaranteed to be null-terminated.
 * 
 * Note: binary files that contain null bytes will read correctly, but will be
 * difficult to parse because the returned string will appear shorter.
 * 
 * Note: will only return NULL if heap allocation fails; as yet does not check
 * for I/O errors.
*/
char* fread_dup(FILE* stream);

#endif