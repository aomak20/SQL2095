// File:        database.h
// Author:      Andrei M
// Description: TODO

#ifndef DATABASE_H
#define DATABASE_H

#include "sql_error.h"

////
// Database API
////

typedef struct Database {
    // TODO add fields
} Database;

// TODO add other stuff, will probably need enum for data types, struct for
// records, struct for WHERE clause, etc.


// Database operations

// Create a new database with `name`, where `db` is a pointer to a Database*.
SqlError db_create(const char* name, Database** db);

// Open the database at `name`, where `db` is a pointer to a Database*.
SqlError db_open(const char* name, Database** db);

// Close the database, where `db` is the database pointer.
SqlError db_close(Database* db);


// Table operations

// Create an empty table with `name`
SqlError db_create_table(Database* db, const char* name);

// Drop the table at `name`
SqlError db_drop_table(Database* db, const char* name);

// TODO add alter_table, which will include the logic to actually add/remove
// columns


// Record operations

// TODO add (select, insert, update, delete)

#endif//DATABASE_H