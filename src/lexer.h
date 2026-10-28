// File:        lexer.h
// Author:      Andrei M
// Description: TODO

#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

////
// Token definitions
////
typedef enum {
    // Statements
    TOK_CREATE,
    TOK_DROP,
    TOK_TABLE,
    TOK_SELECT,
    TOK_INSERT,
    TOK_INTO,
    TOK_VALUES,
    TOK_UPDATE,
    TOK_SET,
    TOK_DELETE,
    TOK_FROM,
    TOK_WHERE,
    // Constraints
    TOK_PRIMARY,
    TOK_KEY,
    TOK_NOT,
    TOK_NULL,
    // Data types
    TOK_INT_TYPE,
    TOK_FLOAT_TYPE,
    TOK_CHAR_TYPE,
    TOK_VARCHAR_TYPE,
    TOK_BOOL_TYPE,
    // Boolean/logic
    TOK_TRUE,
    TOK_FALSE,
    TOK_AND,
    TOK_OR,
    // Literals
    TOK_IDENTIFIER,     // table_name
    TOK_INT_LITERAL,    // 74
    TOK_FLOAT_LITERAL,  // 1.23
    TOK_STRING_LITERAL, // 'example'
    // Operators
    TOK_EQ,             // =
    TOK_NE,             // <>
    TOK_LT,             // <
    TOK_LTE,            // <=
    TOK_GT,             // >
    TOK_GTE,            // >=
    // Symbols/punctuation
    TOK_LPAREN,         // (
    TOK_RPAREN,         // )
    TOK_COMMA,          // ,
    TOK_SEMICOLON,      // ;
    TOK_STAR,           // *
    TOK_PLUS,           // +
    TOK_MINUS,          // -
    TOK_SLASH,          // /
    // Other/special
    TOK_EOF,
    TOK_ERROR
} TokenType;

typedef struct {
    TokenType type;
    const char *start;  // Points at source in-mem
    size_t length;
    size_t line;
    size_t column;
} Token;

#endif//LEXER_H