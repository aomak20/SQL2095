// File:        parser.c
// Author:      Andrei M
// Description: TODO

#include "parser.h"
#include "lexer.h"
#include "sql_error.h"

// TODO parser struct, similar to scanner in lexer, track state and has peek(),
// advance(), etc. functions


// Parser API

SqlError parser_generate(const TokenArray* tokens/*, ASTNode* out_tree */) {
    // TODO fix signature after figuring out data structure for AST nodes
    return SQL_ERROR_OK_MACRO;
}