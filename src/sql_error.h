// File:        sql_error.h
// Author:      Andrei M
// Description: TODO

#ifndef SQL_ERROR_H
#define SQL_ERROR_H

#include <stddef.h>

////
// Error definitions
////

// Error types
// NOTE: Since ERROR_OK = 0, `if (error_type)` returns true on error, else false
typedef enum ErrorType {
    SQL_ERROR_OK = 0,
    SQL_ERROR_LEXICAL,
    SQL_ERROR_SYNTAX,
    SQL_ERROR_SEMANTIC,
    SQL_ERROR_RUNTIME,
    SQL_ERROR_GENERIC
} ErrorType;

// List of strings representing error types. I'd use Xmacros like for token
// type but I want the strings to be formatted naturally, not LIKE_THIS.
extern const char* ERROR_TYPE_STR[];

// Error result
typedef struct Error {
    ErrorType type;
    size_t line;
    size_t column;
    char message[256];
} Error;

////
// Error functions
////

// Print an error's info.
void print_error_info(Error* e);

#endif//SQL_ERROR_H