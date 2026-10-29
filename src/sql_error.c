// File:        sql_error.c
// Author:      Andrei M
// Description: TODO

#include "sql_error.h"

// Error type strings
const char* ERROR_TYPE_STR[] = {
    "",                 // SQL_ERROR_OK
    "Lexical error",    // SQL_ERROR_LEXICAL
    "Syntax error",     // SQL_ERROR_SYNTAX
    "Semantic error",   // SQL_ERROR_SEMANTIC
    "Runtime error",    // SQL_ERROR_RUNTIME
    "Error"             // SQL_ERROR_GENERIC
};


// Error functions
void print_error_info(Error* e) {
    printf("%s at %zu:%zu: %s\n",
        ERROR_TYPE_STR[e->type], e->line, e->column, e->message);
}