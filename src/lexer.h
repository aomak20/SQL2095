// File:        lexer.h
// Author:      Andrei M
// Description: TODO

#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>
#include "sql_error.h"

////
// Token definitions
////

// Macros to generate both TokenType enum and const strings from one list
#define ENUM_GEN(ENUM) ENUM,
#define STRING_GEN(STRING) #STRING,

#define FOREACH_TOKEN_TYPE(TOKEN_TYPE) \
    TOKEN_TYPE(TOKEN_TYPE_CREATE) \
    TOKEN_TYPE(TOKEN_TYPE_DROP) \
    TOKEN_TYPE(TOKEN_TYPE_ALTER) \
    TOKEN_TYPE(TOKEN_TYPE_ADD) \
    TOKEN_TYPE(TOKEN_TYPE_RENAME) \
    TOKEN_TYPE(TOKEN_TYPE_TO) \
    TOKEN_TYPE(TOKEN_TYPE_TABLE) \
    TOKEN_TYPE(TOKEN_TYPE_SELECT) \
    TOKEN_TYPE(TOKEN_TYPE_INSERT) \
    TOKEN_TYPE(TOKEN_TYPE_INTO) \
    TOKEN_TYPE(TOKEN_TYPE_VALUES) \
    TOKEN_TYPE(TOKEN_TYPE_UPDATE) \
    TOKEN_TYPE(TOKEN_TYPE_SET) \
    TOKEN_TYPE(TOKEN_TYPE_DELETE) \
    TOKEN_TYPE(TOKEN_TYPE_FROM) \
    TOKEN_TYPE(TOKEN_TYPE_WHERE) \
    TOKEN_TYPE(TOKEN_TYPE_PRIMARY) \
    TOKEN_TYPE(TOKEN_TYPE_KEY) \
    TOKEN_TYPE(TOKEN_TYPE_DEFAULT) \
    TOKEN_TYPE(TOKEN_TYPE_NOT) \
    TOKEN_TYPE(TOKEN_TYPE_NULL) \
    TOKEN_TYPE(TOKEN_TYPE_INT_TYPE) \
    TOKEN_TYPE(TOKEN_TYPE_FLOAT_TYPE) \
    TOKEN_TYPE(TOKEN_TYPE_CHAR_TYPE) \
    TOKEN_TYPE(TOKEN_TYPE_VARCHAR_TYPE) \
    TOKEN_TYPE(TOKEN_TYPE_BOOL_TYPE) \
    TOKEN_TYPE(TOKEN_TYPE_TRUE) \
    TOKEN_TYPE(TOKEN_TYPE_FALSE) \
    TOKEN_TYPE(TOKEN_TYPE_AND) \
    TOKEN_TYPE(TOKEN_TYPE_OR) \
    TOKEN_TYPE(TOKEN_TYPE_IS) \
    TOKEN_TYPE(TOKEN_TYPE_IDENTIFIER) \
    TOKEN_TYPE(TOKEN_TYPE_INT_LITERAL) \
    TOKEN_TYPE(TOKEN_TYPE_FLOAT_LITERAL) \
    TOKEN_TYPE(TOKEN_TYPE_STRING_LITERAL) \
    TOKEN_TYPE(TOKEN_TYPE_EQ) \
    TOKEN_TYPE(TOKEN_TYPE_NE) \
    TOKEN_TYPE(TOKEN_TYPE_LT) \
    TOKEN_TYPE(TOKEN_TYPE_LTE) \
    TOKEN_TYPE(TOKEN_TYPE_GT) \
    TOKEN_TYPE(TOKEN_TYPE_GTE) \
    TOKEN_TYPE(TOKEN_TYPE_LPAREN) \
    TOKEN_TYPE(TOKEN_TYPE_RPAREN) \
    TOKEN_TYPE(TOKEN_TYPE_COMMA) \
    TOKEN_TYPE(TOKEN_TYPE_SEMICOLON) \
    TOKEN_TYPE(TOKEN_TYPE_STAR) \
    TOKEN_TYPE(TOKEN_TYPE_PLUS) \
    TOKEN_TYPE(TOKEN_TYPE_MINUS) \
    TOKEN_TYPE(TOKEN_TYPE_SLASH) \
    TOKEN_TYPE(TOKEN_TYPE_EOF) \
    TOKEN_TYPE(TOKEN_TYPE_ERROR) \
    TOKEN_TYPE(TOKEN_TYPE_MAX)

typedef enum TokenType {
    FOREACH_TOKEN_TYPE(ENUM_GEN)
} TokenType;

extern const char* TOKEN_TYPE_STR[];

// Individual token
typedef struct {
    TokenType type;
    const char *start;  // Points at source in-mem
    size_t length;
    size_t line;
    size_t column;
} Token;

// Dynamically allocated token array, gets returned by main tokenize function.
// `start` is a pointer to heap allocation, `count` is number of tokens,
// `capacity` is max number of tokens.
typedef struct {
    Token* start;
    size_t count;
    size_t capacity;
} TokenArray;

////
// Lexer API
////

// Main tokenize function. Reads input from memory, returns a TokenArray, which
// points to a heap allocated array of Token structs.
// NOTE: Remember to free the TokenArray after you're done with it using
// `lexer_free_token_array`.
SqlError lexer_tokenize(const char* data, size_t len, TokenArray* out_arr);

////
// TokenArray helpers
////

// Create an empty TokenArray.
TokenArray lexer_new_token_array();

// Check count of a TokenArray and grow if needed.
SqlError lexer_grow_token_array(TokenArray* arr);

// Free the heap allocation under a TokenArray.
SqlError lexer_free_token_array(TokenArray* arr);

// Append a Token to a TokenArray, growing if needed.
SqlError lexer_append_token_array_tok(TokenArray* arr, const Token token);
// Append a new Token to a TokenArray based on given args, growing if needed.
SqlError lexer_append_token_array_arg(TokenArray* arr, TokenType type,
    const char* start, size_t length, size_t line, size_t column);

// Print a Token struct's info.
void lexer_print_token(const Token* token);

// Print each Token in a TokenArray.
void lexer_print_token_array(const TokenArray* arr);

#endif//LEXER_H