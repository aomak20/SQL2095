// File:        sql_error.c
// Author:      Andrei M
// Description: TODO

#include <stdio.h>
#include "sql_error.h"

// Error type strings
const char* SQL_ERROR_TYPE_STR[] = {
    "",                 // SQL_ERROR_OK
    "Lexical error",    // SQL_ERROR_LEXICAL
    "Syntax error",     // SQL_ERROR_SYNTAX
    "Semantic error",   // SQL_ERROR_SEMANTIC
    "Database error",   // SQL_ERROR_DATABASE
    "Runtime error",    // SQL_ERROR_RUNTIME
    "Internal error",   // SQL_ERROR_INTERNAL
    "Error"             // SQL_ERROR_GENERIC
};


// Error functions
void print_error_info(const SqlError* e) {
    if (e->type == SQL_ERROR_OK) return;
    printf("%s", SQL_ERROR_TYPE_STR[e->type]);
    if (e->line > 0 || e->column > 0)
        printf(" at %zu:%zu", e->line, e->column);
    printf(": %s\n", e->message);
}