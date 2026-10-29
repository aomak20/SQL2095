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
static SqlError process_left_arrow(Scanner* s) {
    const char* temp = s->data + s->pos;    // Start of token pointer
    size_t line = s->line;
    size_t col = s->column;
    scanner_advance(s);

    char next_char = scanner_peek(s);
    switch (next_char) {
        case '>':   // <>
            RETURN_IF_ERR(lexer_append_token_array_arg(&s->tokens, TOKEN_TYPE_NE,
                temp, 2, line, col));
            scanner_advance(s);
            break;
        case '=':   // <=
            RETURN_IF_ERR(lexer_append_token_array_arg(&s->tokens, TOKEN_TYPE_LTE,
                temp, 2, line, col));
            scanner_advance(s);
            break;
        default:    // <
            RETURN_IF_ERR(lexer_append_token_array_arg(&s->tokens, TOKEN_TYPE_LT,
                temp, 1, line, col));
            // Next char was not part of < token, so don't advance
    }

    return SQL_ERROR_OK_MACRO;
}

// Process a '>' with scanner
static SqlError process_right_arrow(Scanner* s) {
    const char* temp = s->data + s->pos;    // Start of token pointer
    size_t line = s->line;
    size_t col = s->column;
    scanner_advance(s);

    char next_char = scanner_peek(s);
    switch (next_char) {
        case '=':   // >=
            RETURN_IF_ERR(lexer_append_token_array_arg(&s->tokens, TOKEN_TYPE_GTE,
                temp, 2, line, col));
            scanner_advance(s);
            break;
        default:    // >
            RETURN_IF_ERR(lexer_append_token_array_arg(&s->tokens, TOKEN_TYPE_GT,
                temp, 1, line, col));
            // Next char was not part of > token, so don't advance
    }

    return SQL_ERROR_OK_MACRO;
}

// Process a '"' with scanner
static SqlError process_quoted_string(Scanner* s) {
    SqlError result;
    // Line and column will point to start of string, including double quotes,
    // but the actual token will just store the string itself without quotes.
    size_t line = s->line;
    size_t col = s->column;
    scanner_advance(s);                     // Skip the double quote
    const char* temp = s->data + s->pos;    // Start of token pointer
    size_t len = 0;                         // Track length of quoted string

    char next_char = scanner_peek(s);
    while (next_char != '"') {
        if (isalnum(next_char) || ispunct(next_char) || isspace(next_char)) {
            // Allow alphanum, punctuation, and whitespace in strings. This is
            // a somewhat arbitrary restriction which I may change later.
            scanner_advance(s);
            len++;
        } else if (next_char == '\0') {
            // scanner_peek returns null-byte at end of input
            result = (SqlError) {
                .type=SQL_ERROR_LEXICAL, .line=line, .column=col,
                .message="Quoted string was not closed" };
            return result;
        } else {
            result = (SqlError) {
                .type=SQL_ERROR_LEXICAL, .line=s->line, .column=s->column };
            // Cast to char* here because the length of string is known
            // to be under the max length of result.message
            sprintf((char*)&result.message,
                "Unrecognized character '%c' in quoted string", next_char);
            return result;
        }
        next_char = scanner_peek(s);
    }
    RETURN_IF_ERR(lexer_append_token_array_arg(&s->tokens, TOKEN_TYPE_STRING_LITERAL,
        temp, len, line, col));
    scanner_advance(s);

    return SQL_ERROR_OK_MACRO;
}

// Process a letter with scanner
static SqlError process_id_keyword(Scanner* s) {
    SqlError result;
    const char* temp = s->data + s->pos;    // Start of token pointer
    size_t line = s->line;
    size_t col = s->column;
    size_t len = 0;                         // Track length of id/keyword

    char next_char = scanner_peek(s);
    while (true) {
        if (isalnum(next_char)) {
            // Allow only alphanumerics in identifiers
            scanner_advance(s);
            len++;
        } else if (isspace(next_char) || next_char == '\0') {
            // Whitespace or EOF ends the identifier
            break;
        } else {
            result = (SqlError) {
                .type=SQL_ERROR_LEXICAL, .line=s->line, .column=s->column };
            // Cast to char* here because the length of string is known
            // to be under the max length of result.message
            sprintf((char*)&result.message,
                "Unrecognized character '%c' in identifier/keyword", next_char);
            return result;
        }
        next_char = scanner_peek(s);
    }

    // Determine if this token matches a keyword (case-insensitive), otherwise
    // it's an identifier
    TokenType type = check_keyword_match(temp, len);
    RETURN_IF_ERR(lexer_append_token_array_arg(&s->tokens, type,
        temp, len, line, col));

    return SQL_ERROR_OK_MACRO;
}

// Process a digit with scanner
static SqlError process_number(Scanner* s) {
    SqlError result;
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
                // If we find another period before any non-digit character,
                // there's a nigh 100% chance the number is malformed.
                result = (SqlError) {
                    .type=SQL_ERROR_LEXICAL, .line=s->line, .column=s->column,
                    .message="Float literals can't have more than one period" };
                return result;
            } else {
                found_period = true;
                type = TOKEN_TYPE_FLOAT_LITERAL;
                scanner_advance(s);
                len++;
            }
        } else {
            // TODO this currently allows any char to signal the end of a num,
            // allowing legal syntax like "123abc" or "4.56;". Figure out if I
            // want to put any further restrictions on this later.
            break;
        }
    }

    // Make sure float didn't end with a period
    if (temp[len-1] == '.') {
        result = (SqlError) {
            .type=SQL_ERROR_LEXICAL, .line=s->line, .column=s->column,
            .message="Float can't end with a trailing period" };
        return result;
    }

    RETURN_IF_ERR(lexer_append_token_array_arg(&s->tokens, type,
        temp, len, line, col));
    
    return SQL_ERROR_OK_MACRO;
}


// Lexer API

SqlError lexer_tokenize(const char* data, size_t len, TokenArray* out_arr) {
    // Initialize scanner state
    SqlError result;
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
            // Single-character tokens which can't be scanned further
            case '(':
                RETURN_IF_ERR(lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_LPAREN, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column));
                scanner_advance(&scanner);
                break;
            case ')':
                RETURN_IF_ERR(lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_RPAREN, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column));
                scanner_advance(&scanner);
                break;
            case ',':
                RETURN_IF_ERR(lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_COMMA, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column));
                scanner_advance(&scanner);
                break;
            case ';':
                RETURN_IF_ERR(lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_SEMICOLON, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column));
                scanner_advance(&scanner);
                break;
            case '*':
                RETURN_IF_ERR(lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_STAR, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column));
                scanner_advance(&scanner);
                break;
            case '+':
                RETURN_IF_ERR(lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_PLUS, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column));
                scanner_advance(&scanner);
                break;
            case '-':
                RETURN_IF_ERR(lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_MINUS, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column));
                scanner_advance(&scanner);
                break;
            case '/':
                RETURN_IF_ERR(lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_SLASH, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column));
                scanner_advance(&scanner);
                break;
            case '=':
                RETURN_IF_ERR(lexer_append_token_array_arg(&scanner.tokens,
                    TOKEN_TYPE_EQ, scanner.data + scanner.pos, 1,
                    scanner.line, scanner.column));
                scanner_advance(&scanner);
                break;
            case '<':
                RETURN_IF_ERR(process_left_arrow(&scanner));
                break;
            case '>':
                RETURN_IF_ERR(process_right_arrow(&scanner));
                break;
            case '"':
                RETURN_IF_ERR(process_quoted_string(&scanner));
                break;
            default:
                if (isspace(c)) {
                    scanner_advance(&scanner);
                } else if (isalpha(c)) {
                    RETURN_IF_ERR(process_id_keyword(&scanner));
                } else if (isdigit(c)) {
                    RETURN_IF_ERR(process_number(&scanner));
                } else {
                    result = (SqlError) {
                        .type = SQL_ERROR_LEXICAL,
                        .line = scanner.line,
                        .column = scanner.column
                    };
                    // Cast to char* here because the length of string is known
                    // to be under the max length of result.message
                    sprintf((char*)&result.message,
                        "Unrecognized character '%c'", c);
                    return result;
                }
        }
    }

    RETURN_IF_ERR(lexer_append_token_array_arg(out_arr, TOKEN_TYPE_EOF,
        scanner.data + scanner.pos, 0, scanner.line, scanner.column));
    *out_arr = scanner.tokens;
    return SQL_ERROR_OK_MACRO;
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

SqlError lexer_grow_token_array(TokenArray* arr) {
    if (arr->capacity == 0) {
        arr->capacity = INIT_TOKARR_CAP;
        arr->start = malloc(sizeof(Token) * arr->capacity);
    } else if (arr->count == arr->capacity) {
        arr->capacity *= 2;
        char* temp = realloc(arr->start, sizeof(Token) * arr->capacity);
        if (temp == NULL) {
            free(arr->start);
            arr->start = NULL;
        }
    }

    if (arr->start != NULL) {
        return SQL_ERROR_OK_MACRO;
    } else {
        return (SqlError) {
            .type = SQL_ERROR_INTERNAL,
            .line = 0,
            .column = 0,
            .message = "Failed to allocate memory"
        };
    }
}

SqlError lexer_free_token_array(TokenArray* arr) {
    free(arr->start);
    arr->start = NULL;
    arr->count = 0;
    return SQL_ERROR_OK_MACRO;
}

SqlError lexer_append_token_array_tok(TokenArray* arr, const Token token) {
    RETURN_IF_ERR(lexer_grow_token_array(arr));
    arr->start[arr->count] = token;
    arr->count++;
    return SQL_ERROR_OK_MACRO;
}

SqlError lexer_append_token_array_arg(TokenArray* arr, TokenType type,
    const char* start, size_t length, size_t line, size_t column
) {
    RETURN_IF_ERR(lexer_grow_token_array(arr));
    Token token = {
        .type = type,
        .start = start,
        .length = length,
        .line = line,
        .column = column
    };
    arr->start[arr->count] = token;
    arr->count++;
    return SQL_ERROR_OK_MACRO;
}

void lexer_print_token(const Token* token) {
    printf("%s | %zu:%zu\n", TOKEN_TYPE_STR[token->type],
        token->line, token->column);
    printf("\"%.*s\"\n", (int)token->length, token->start);
}

void lexer_print_token_array(const TokenArray* arr) {
    printf("Token count: %zu\n\n", arr->count);
    for (size_t i = 0; i < arr->count; ++i) {
        printf("%zu | ", i);
        lexer_print_token(&arr->start[i]);
        printf("\n");
    }
}