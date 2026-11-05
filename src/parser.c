// File:        parser.c
// Author:      Andrei M
// Description: TODO

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "parser.h"
#include "lexer.h"
#include "sql_error.h"


// Parser struct to keep track of state, pass between functions
typedef struct Parser {
    // Tokens input
    const TokenArray* tokens;
    size_t index;
} Parser;


// Helper functions

// Get current token from parser
static Token* parser_peek(const Parser* p) {
    return (p->index < p->tokens->count ? &p->tokens->start[p->index] : NULL);
}

// Advance one token in parser
static void parser_advance(Parser* p) {
    if (p->index >= p->tokens->count) return;
    p->index++;
}

static SqlError parse_statement(Parser* p, StatementNode* out_node) {
    (void)out_node; // TEMPORARY silence warning
    // TODO implement further, currently just consumes all tokens until next ';'
    Token* curr_token = parser_peek(p);
    if (curr_token == NULL) 
        return (SqlError) {
            .type = SQL_ERROR_INTERNAL,
            .line = 0,
            .column = 0,
            .message = "Unexpected end of token list"            
        };

    while (curr_token->type != TOKEN_TYPE_SEMICOLON) {
        if (curr_token->type == TOKEN_TYPE_EOF)
            return (SqlError) {
                .type = SQL_ERROR_SEMANTIC,
                .line = 0,
                .column = 0,
                .message = "Unexpected end of file"
            };

        parser_advance(p);
        curr_token = parser_peek(p);
        if (curr_token == NULL) 
            return (SqlError) {
                .type = SQL_ERROR_INTERNAL,
                .line = 0,
                .column = 0,
                .message = "Unexpected end of token list"            
            };
    }

    parser_advance(p);  // Consume the semicolon
    return SQL_ERROR_OK_MACRO;
}

static SqlError parse_program(Parser* p, ProgramNode* out_node) {
    SqlError result;
    Token* curr_token = parser_peek(p);
    if (curr_token == NULL)
        return (SqlError) {
            .type = SQL_ERROR_INTERNAL,
            .line = 0,
            .column = 0,
            .message = "Unexpected end of token list"            
        };

    // Parse statement nodes until EOF reached
    StatementNode* last_node = NULL;            // Last node in out_node's list
    while (curr_token->type != TOKEN_TYPE_EOF) {
        // Create new statement_node, parse it, and add to out_node
        StatementNode* new_node = malloc(sizeof(StatementNode));
        if (new_node == NULL) 
            return (SqlError) {
                .type = SQL_ERROR_INTERNAL,
                .line = 0,
                .column = 0,
                .message = "Failed to allocate memory"
            };
        *new_node = (StatementNode){0};
        result = parse_statement(p, new_node);
        if (result.type != SQL_ERROR_OK) {
            free(new_node);
            return result;
        }

        if (out_node->first_stmt == NULL) {     // out_node's list empty
            out_node->first_stmt = new_node;
            last_node = out_node->first_stmt;
        } else {                                // out_node's list not empty
            last_node->next = new_node;
            last_node = last_node->next;
        }

        curr_token = parser_peek(p);
        // Don't need to check if curr_token is NULL, since parse_statement
        // would have thrown up an error.
    }
    return SQL_ERROR_OK_MACRO;
}


// Parser API

SqlError parser_generate(const TokenArray* tokens, ProgramNode* out_tree) {
    ProgramNode node = parser_new_program_node();
    Parser parser = { .tokens=tokens, .index=0 };
    SqlError result = parse_program(&parser, &node);
    if (result.type != SQL_ERROR_OK) {
        free_program(&node);
    }
    *out_tree = node;
    return SQL_ERROR_OK_MACRO;
}


// AST Helpers

ProgramNode parser_new_program_node() {
    ProgramNode node = {
        .first_stmt = NULL
    };
    return node;
}