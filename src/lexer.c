// File:        lexer.c
// Author:      Andrei M
// Description: TODO

#include <stdlib.h>
#include "lexer.h"

// Constants
const size_t INIT_TOKARR_CAP = 64; // start capacity for a TokenArray

// Lexer API
TokenArray lexer_tokenize(const char* data, size_t len) {
    // Initialize scanner state, TokenArray
    size_t pos = 0;
    size_t line = 1;
    size_t col = 1;
    
    TokenArray tokens = lexer_new_token_array();

    // TODO implement lexer, main loop of scanning input, creating tokens, and
    // appending tokens to `tokens.start`. also malloc/realloc `.start` if need

    return tokens;
}

// TokenArray helpers
TokenArray lexer_new_token_array() {
    TokenArray tokens = {
        .start = NULL,
        .count = 0,
        .capacity = 0
    };
    return tokens;
}

void lexer_grow_token_array(TokenArray* arr) {
    if (arr->capacity == 0) {
        arr->capacity = INIT_TOKARR_CAP;
        arr->start = malloc(sizeof(Token) * arr->capacity);
    } else if (arr->count == arr->capacity) {
        arr->capacity *= 2;
        arr->start = realloc(arr->start, sizeof(Token) * arr->capacity);
    }
}

void lexer_free_token_array(TokenArray* arr) {
    free(arr->start);
    arr->start = NULL;
    arr->count = 0;
}