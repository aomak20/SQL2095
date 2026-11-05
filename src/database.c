// File:        database.c
// Author:      Andrei M
// Description: TODO

#include "database.h"
#include "sql_error.h"


// Database API

// Create empty DB
SqlError db_create(const char* name, Database** db) {
    // TODO implement
    return SQL_ERROR_OK_MACRO;
}

// Open DB
SqlError db_open(const char* name, Database** db) {
    // TODO implement
    return SQL_ERROR_OK_MACRO;
}

// Close DB
SqlError db_close(Database* db) {
    // TODO implement
    return SQL_ERROR_OK_MACRO;
}

// Create empty table
SqlError db_create_table(Database* db, const char* name) {
    // TODO implement
    return SQL_ERROR_OK_MACRO;
}

// Drop table
SqlError db_drop_table(Database* db, const char* name) {
    // TODO implement
    return SQL_ERROR_OK_MACRO;
}