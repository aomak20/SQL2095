// File:        lexer.c
// Author:      Andrei M
// Description: TODO

#include <stdio.h>
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

void lexer_print_token(const Token* token) {
    printf("Type: %s\n", TOKEN_TYPE_STR[token->type]);
    printf("Data: \"%.*s\"\n", (int)token->length, token->start);
    printf("Line, column: %zu, %zu\n", token->line, token->column);
}

void lexer_print_token_array(const TokenArray* arr) {
    printf("Token count: %zu\n\n", arr->count);
    for (size_t i = 0; i < arr->count; ++i) {
        printf("%zu\n", i);
        lexer_print_token(&arr->start[i]);
        printf("\n");
    }
}