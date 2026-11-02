// File:        parser.h
// Author:      Andrei M
// Description: TODO

#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "sql_error.h"

////
// AST Node Definitions/Destructors
////

// Statement node
typedef struct StatementNode StatementNode;
struct StatementNode {
    StatementNode* next;        // Next statement in linked list
    // TODO add union or enum or something to support different statement types
    // also add child nodes as needed when more AST nodes are defined
};

void free_statement(StatementNode* stmt);

// Program node, root of AST
typedef struct ProgramNode {
    StatementNode* first_stmt;  // Start of linked list
} ProgramNode;

// NOTE: free_program does not free(program). While all other nodes in the AST
// are created and allocated by helper functions, the ProgramNode at the root
// is expected to be handled by the caller.
void free_program(ProgramNode* program);

////
// Parser API
////

// Main parser function. Generates an AST based on an input list of tokens.
SqlError parser_generate(const TokenArray* tokens, ProgramNode* out_tree);

#endif//PARSER_H