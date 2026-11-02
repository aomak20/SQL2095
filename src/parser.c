// File:        parser.c
// Author:      Andrei M
// Description: TODO

#include <stdlib.h>
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

static SqlError parse_program(Parser* p, ProgramNode* out_node) {
    // TODO implement, while current token is not EOF, keep calling parse_stmt
    // parse_stmt should do the advancing so each time this func checks current
    // token it should see either the starting token of a new statement, or EOF
    return SQL_ERROR_OK_MACRO;
}

static SqlError parse_statement(Parser* p, StatementNode* out_node) {
    // TODO implement
    return SQL_ERROR_OK_MACRO;
}


// Parser API

SqlError parser_generate(const TokenArray* tokens, ProgramNode* out_tree) {
    // Initialization
    Parser parser = { .tokens=tokens, .index=0 };
    // Parsing
    SqlError result = parse_program(&parser, out_tree);
    if (result.type != SQL_ERROR_OK) {
        free_program(out_tree);     // Recursive top-down cleanup AST
        return result;
    }
    return SQL_ERROR_OK_MACRO;
}


// Destructor functions for AST nodes

void free_program(ProgramNode* program) {
    if (program == NULL) return;

    StatementNode* s = program->first_stmt;
    while (s != NULL) {
        StatementNode* temp = s->next;
        free_statement(s);
        s = temp;
    }

    // Don't free(program) since the lifetime and allocation of `program`
    // should be handled by the caller
}

void free_statement(StatementNode* stmt) {
    if (stmt == NULL) return;

    // TODO implement more logic later when StatementNode is fleshed out, e.g.
    // based on type it will have various types of child nodes it must free

    free(stmt);
}