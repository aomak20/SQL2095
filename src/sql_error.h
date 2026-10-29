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
typedef enum SqlErrorType {
    SQL_ERROR_OK = 0,
    SQL_ERROR_LEXICAL,
    SQL_ERROR_SYNTAX,
    SQL_ERROR_SEMANTIC,
    SQL_ERROR_RUNTIME,
    SQL_ERROR_INTERNAL,
    SQL_ERROR_GENERIC
} SqlErrorType;

// List of strings representing error types. I'd use Xmacros like for token
// type but I want the strings to be formatted naturally, not LIKE_THIS.
extern const char* SQL_ERROR_TYPE_STR[];

// Error result
typedef struct SqlError {
    SqlErrorType type;
    size_t line;
    size_t column;
    char message[256];
} SqlError;

// Useful macros
#define SQL_ERROR_OK_MACRO ((SqlError){.type=SQL_ERROR_OK})
#define RETURN_IF_ERR(E) do { \
    SqlError ERR_RESULT = (E); \
    if (ERR_RESULT.type != SQL_ERROR_OK) return ERR_RESULT; \
} while (0) // Ending in while (0) means you can do RETURN_IF_ERR(..); cleanly

////
// Error functions
////

// Print an error's info.
void print_error_info(const SqlError* e);

#endif//SQL_ERROR_H