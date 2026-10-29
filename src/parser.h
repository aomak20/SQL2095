// File:        parser.h
// Author:      Andrei M
// Description: TODO

#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "sql_error.h"

// TODO figure out data structure for AST nodes and stuff

////
// Parser API
////

// Main parser function. Generates an AST based on an input list of tokens.
// TODO fix signature after figuring out data structure for AST nodes
SqlError parser_generate(const TokenArray* tokens/*, ASTNode* out_tree */);

#endif//PARSER_H