// File:        lexer.c
// Author:      Andrei M
// Description: TODO

#include <stdlib.h>
#include "lexer.h"

TokenArray lexer_tokenize(const char* data, size_t len) {
    //TODO implement
    TokenArray tokens = {
        .start = NULL,
        .count = 0
    };
    return tokens;
}

void lexer_free_token_array(TokenArray* arr) {
    free(arr->start);
    arr->start = NULL;
    arr->count = 0;
}