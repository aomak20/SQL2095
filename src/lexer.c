// File:        lexer.c
// Author:      Andrei M
// Description: TODO

#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "sql_error.h"

// Constants
const char* TOKEN_TYPE_STR[] = {
    FOREACH_TOKEN_TYPE(STRING_GEN)
};
const size_t INIT_TOKARR_CAP = 64; // start capacity for a TokenArray

// Keywords array to match const char* to token type
typedef struct {
    const char *keyword;
    TokenType type;
} Keyword;

static const Keyword keywords[] = {
    {"CREATE",  TOKEN_TYPE_CREATE},
    {"DROP",    TOKEN_TYPE_DROP},
    {"TABLE",   TOKEN_TYPE_TABLE},
    {"SELECT",  TOKEN_TYPE_SELECT},
    {"INSERT",  TOKEN_TYPE_INSERT},
    {"INTO",    TOKEN_TYPE_INTO},
    {"VALUES",  TOKEN_TYPE_VALUES},
    {"UPDATE",  TOKEN_TYPE_UPDATE},
    {"SET",     TOKEN_TYPE_SET},
    {"DELETE",  TOKEN_TYPE_DELETE},
    {"FROM",    TOKEN_TYPE_FROM},
    {"WHERE",   TOKEN_TYPE_WHERE},
    {"PRIMARY", TOKEN_TYPE_PRIMARY},
    {"KEY",     TOKEN_TYPE_KEY},
    {"NOT",     TOKEN_TYPE_NOT},
    {"NULL",    TOKEN_TYPE_NULL},
    {"INT",     TOKEN_TYPE_INT_TYPE},
    {"FLOAT",   TOKEN_TYPE_FLOAT_TYPE},
    {"CHAR",    TOKEN_TYPE_CHAR_TYPE},
    {"VARCHAR", TOKEN_TYPE_VARCHAR_TYPE},
    {"BOOL",    TOKEN_TYPE_BOOL_TYPE},
    {"TRUE",    TOKEN_TYPE_TRUE},
    {"FALSE",   TOKEN_TYPE_FALSE},
    {"AND",     TOKEN_TYPE_AND},
    {"OR",      TOKEN_TYPE_OR}
};

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

// Check if string matches a keyword, case-insensitive, and returns either the
// corresponding keyword or identifier token type
static TokenType check_keyword_match(const char* data, size_t len) {
    for (size_t i = 0; i < sizeof(keywords) / sizeof(keywords[0]); ++i) {
        if (strlen(keywords[i].keyword) == len &&
            strncasecmp(data, keywords[i].keyword, len) == 0) {
            return keywords[i].type;
        }
    }
    return TOKEN_TYPE_IDENTIFIER;
}

// Get current character from scanner
static char scanner_peek(const Scanner* s) {
    return (s->pos < s->len ? s->data[s->pos] : '\0');
}

// Advance one character in scanner
static void scanner_advance(Scanner* s) {
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

// Process a '<' with scanner
static void process_left_arrow(Scanner* s) {
    const char* temp = s->data + s->pos;    // Start of token pointer
    size_t line = s->line;
    size_t col = s->column;
    scanner_advance(s);

    char next_char = scanner_peek(s);
    switch (next_char) {
        case '>':   // <>
            lexer_append_token_array_arg(&s->tokens, TOKEN_TYPE_NE,
                temp, 2, line, col);
            scanner_advance(s);
            break;
        case '=':   // <=
            lexer_append_token_array_arg(&s->tokens, TOKEN_TYPE_LTE,
                temp, 2, line, col);
            scanner_advance(s);
            break;
        default:    // <
            lexer_append_token_array_arg(&s->tokens, TOKEN_TYPE_LT,
                temp, 1, line, col);
            scanner_advance(s);
    }
}

// Process a '>' with scanner
static void process_right_arrow(Scanner* s) {
    // TODO process > to get either > or >= token
    const char* temp = s->data + s->pos;    // Start of token pointer
    size_t line = s->line;
    size_t col = s->column;
    scanner_advance(s);

    char next_char = scanner_peek(s);
    switch (next_char) {
        case '=':   // >=
            lexer_append_token_array_arg(&s->tokens, TOKEN_TYPE_GTE,
                temp, 2, line, col);
            scanner_advance(s);
            break;
        default:    // >
            lexer_append_token_array_arg(&s->tokens, TOKEN_TYPE_GT,
                temp, 1, line, col);
            scanner_advance(s);
    }
}

// Process a '"' with scanner
static void process_quoted_string(Scanner* s) {
    // Line and column will point to start of string, including double quotes,
    // but the actual token will just store the string itself without quotes.
    size_t line = s->line;
    size_t col = s->column;
    scanner_advance(s);                     // Skip the double quote
    const char* temp = s->data + s->pos;    // Start of token pointer
    size_t len = 0;                         // Track length of quoted string

    while (scanner_peek(s) != '"') {
        if (isalnum(scanner_peek(s))) {
            scanner_advance(s);
            len++;
        } else {
            // TODO handle a non-alphanumeric or EOF inside string literal.
            // This is an arbitrary restriction, figure out what I want to do
            // about it later.
            scanner_advance(s);
            len++;
        }
    }
    lexer_append_token_array_arg(&s->tokens, TOKEN_TYPE_STRING_LITERAL,
        temp, len, line, col);
    scanner_advance(s);
}

// Process a letter with scanner
static void process_id_keyword(Scanner* s) {
    const char* temp = s->data + s->pos;    // Start of token pointer
    size_t line = s->line;
    size_t col = s->column;
    size_t len = 0;                         // Track length of id/keyword

    while (!isspace(scanner_peek(s))) {
        // TODO currently this allows any non-whitespace char in an identifier,
        // figure this out later and add check & error handling if needed
        scanner_advance(s);
        len++;
    }

    // Determine if this token matches a keyword (case-insensitive), otherwise
    // it's an identifier
    TokenType type = check_keyword_match(temp, len);
    lexer_append_token_array_arg(&s->tokens, type,
        temp, len, line, col);
}

// Process a digit with scanner
static void process_number(Scanner* s) {
    // TODO process to get a int or float literal token
    const char* temp = s->data + s->pos;    // Start of token pointer
    size_t line = s->line;
    size_t col = s->column;
    size_t len = 0;                         // Track length of number
    bool found_period = false;
    TokenType type = TOKEN_TYPE_INT_LITERAL;

    while (true) {
        char next_char = scanner_peek(s);
        if (isdigit(next_char)) {
            scanner_advance(s);
            len++;
        } else if (next_char == '.') {
            if (found_period) {
                // TODO handle error, multiple periods in number
                scanner_advance(s);
                len++;
            } else {
                found_period = true;
                type = TOKEN_TYPE_FLOAT_LITERAL;
                scanner_advance(s);
                len++;
            }
        } else {
            // TODO could put a check here to make sure number is followed by
            // whitespace, not sure if that's what I wanna do yet
            break;
        }
    }

    // Make sure float didn't end with a period
    if (temp[len-1] == '.') {
        // TODO handle error for float ending with '.'
    }

    lexer_append_token_array_arg(&s->tokens, type,
        temp, len, line, col);
}


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

    // Main scanner loop
    while (scanner.pos < scanner.len) {
        char c = scanner_peek(&scanner);

        switch (c) {
            // Skip whitespace
            case ' ':
            case '\t':
            case '\v':
            case '\n':
            case '\r':
            case '\f':
                scanner_advance(&scanner);
                break;
            // Single-character tokens which can't be scanned further
            case '(':
                lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_LPAREN, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column);
                scanner_advance(&scanner);
                break;
            case ')':
                lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_RPAREN, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column);
                scanner_advance(&scanner);
                break;
            case ',':
                lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_COMMA, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column);
                scanner_advance(&scanner);
                break;
            case ';':
                lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_SEMICOLON, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column);
                scanner_advance(&scanner);
                break;
            case '*':
                lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_STAR, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column);
                scanner_advance(&scanner);
                break;
            case '+':
                lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_PLUS, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column);
                scanner_advance(&scanner);
                break;
            case '-':
                lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_MINUS, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column);
                scanner_advance(&scanner);
                break;
            case '/':
                lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_SLASH, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column);
                scanner_advance(&scanner);
                break;
            case '=':
                lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_EQ, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column);
                scanner_advance(&scanner);
                break;
            case '<':
                process_left_arrow(&scanner);
                break;
            case '>':
                process_right_arrow(&scanner);
                break;
            case '"':
                process_quoted_string(&scanner);
                break;
            default:
                if (isalpha(c)) {
                    process_id_keyword(&scanner);
                } else if (isdigit(c)) {
                    process_number(&scanner);
                } else {
                    // TODO unrecognized character, error handle it
                    scanner_advance(&scanner);
                }
        }
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

void lexer_append_token_array_tok(TokenArray* arr, const Token token) {
    lexer_grow_token_array(arr);
    arr->start[arr->count] = token;
    arr->count++;
}

void lexer_append_token_array_arg(TokenArray* arr, TokenType type,
    const char* start, size_t length, size_t line, size_t column
) {
    lexer_grow_token_array(arr);
    Token token = {
        .type = type,
        .start = start,
        .length = length,
        .line = line,
        .column = column
    };
    arr->start[arr->count] = token;
    arr->count++;
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