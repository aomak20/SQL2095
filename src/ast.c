// File:        ast.c
// Author:      Andrei M
// Description: TODO

#include <stdlib.h>
#include "ast.h"


// Destructor functions for AST nodes

void free_program(ProgramNode* program) {
    if (program == NULL) return;

    StatementNode* s = program->first_stmt;
    while (s != NULL) {
        StatementNode* temp = s->next;
        free_statement(s);
        s = temp;
    }
    program->first_stmt = NULL;
}

void free_statement(StatementNode* stmt) {
    if (stmt == NULL) return;

    // TODO implement more logic later when StatementNode is fleshed out, e.g.
    // based on type it will have various types of child nodes it must free

    free(stmt);
}