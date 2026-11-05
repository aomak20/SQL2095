// File:        parser.h
// Author:      Andrei M
// Description: TODO

#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include "sql_error.h"

////
// Parser API
////

// Main parser function. Generates an AST based on an input list of tokens.
// Initializes `out_tree` and generates an AST from the input tokens. If this
// function fails, cleanup is automatic and `out_tree` remains uninitialized.
SqlError parser_generate(const TokenArray* tokens, ProgramNode* out_tree);

////
// AST Helpers
////

// Create an empty ProgramNode.
ProgramNode parser_new_program_node();

#endif//PARSER_H