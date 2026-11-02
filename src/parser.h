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

// Program node, root of AST
typedef struct ProgramNode {
    StatementNode* first_stmt;  // Start of linked list
} ProgramNode;

void free_program(ProgramNode* program);

// Statement node
typedef struct StatementNode {
    StatementNode* next;        // Next statement in linked list
    // TODO add union or enum or something to support different statement types
    // also add child nodes as needed when more AST nodes are defined
} StatementNode;

void free_statement(StatementNode* stmt);

////
// Parser API
////

// Main parser function. Generates an AST based on an input list of tokens.
SqlError parser_generate(const TokenArray* tokens, ProgramNode* out_tree);

#endif//PARSER_H