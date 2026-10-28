// File:        lexer.c
// Author:      Andrei M
// Description: TODO

#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

// Constants
const size_t INIT_TOKARR_CAP = 64; // start capacity for a TokenArray

// Scanner struct to keep track of state, pass between functions
typedef struct Scanner {
    // Source input
    const char* data;
    size_t len;
    // Current position
    size_t pos;
    size_t line;
    size_t column;
    // TokenArray
    TokenArray tokens;
} Scanner;


// Helper functions

static char scanner_peek(const Scanner* s) {
    return (s->pos < s->len ? s->data[s->pos] : '\0');
}

static void advance(Scanner* s) {
    if (s->pos >= s->len) return;

    // Handle lines
    if (scanner_peek(s) == '\n') {
        s->line++;
        s->column = 1;
    } else {
        s->column++;
    }

    s->pos++;
}

// TODO add helper functions to tokenize identifiers, numbers, etc.


// Lexer API

TokenArray lexer_tokenize(const char* data, size_t len) {
    // Initialize scanner state
    TokenArray tokens = lexer_new_token_array();
    Scanner scanner = {
        .data = data,
        .len = len,
        .pos = 0,
        .line = 1,
        .column = 1,
        .tokens = tokens
    };

    // TODO implement lexer, main loop of scanning input, creating tokens, and
    // appending tokens to `tokens.start`. also malloc/realloc `.start` if need
    // Main scanner loop
    while (scanner.pos < scanner.len) {
        // TODO figure out the scanning DFA, switch statements based on current char
        // Delegate to helper function as soon as possible, there it can do all the Token and TokenArray handling
    }

    return scanner.tokens;
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