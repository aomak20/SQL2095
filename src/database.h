// File:        database.h
// Author:      Andrei M
// Description: TODO

#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <stdint.h>
#include "sql_error.h"

////
// Database definitions
////

// Constants
extern const uint32_t CATALOG_MAGIC;
extern const uint8_t DB_TABLE_NAME_MAX_LEN;
extern const uint8_t DB_COLUMN_NAME_MAX_LEN;

// Column metadata, containing information about a column's name, type, and
// other information.
typedef struct ColumnMeta ColumnMeta;
struct ColumnMeta {
    // TODO add relevant stuff like type, len (for varchar), ...
    uint8_t name_len;    // Length of name
    char* name;         // Name char data
    ColumnMeta* next;   // Next col metadata in linked list
};

// Table metadata, containing information about a table's name.
typedef struct TableMeta TableMeta;
struct TableMeta {
    uint16_t count;       // Number of columns
    uint8_t name_len;    // Length of name
    char* name;         // Name char data
    ColumnMeta* first;  // First col metadata in linked list
    ColumnMeta* last;   // Last col metadata in linked list
    TableMeta* next;    // Next table metadata in linked list
};

// Catalog, containing meta information about database like number of tables.
// In binary, the catalog looks like:
// CATALOG_MAGIC:u32, NUM_TABLES:u16
// For each table, (NAME_LEN:u8, NAME:u8[], NUM_COLUMNS:u16)
//   For each column, (NAME_LEN:u8, NAME:u8[], ... )
typedef struct Catalog {
    uint16_t count;       // Number of tables
    TableMeta* first;   // First table metadata in linked list
    TableMeta* last;    // Last table metadata in linked list, for easy append
} Catalog;

// TODO add other stuff, will probably need enum for data types, struct for
// records, struct for WHERE clause, etc.

////
// Catalog operations
////

// Initialize a catalog with default empty values.
SqlError init_catalog(Catalog* c);

// Read in a catalog from the given file. `fp` must be open in 'rb' mode.
SqlError read_catalog(Catalog* c, FILE* fp);

// Write out a catalog to the given file. `fp` must be open in 'wb' mode.
SqlError write_catalog(const Catalog* c, FILE* fp);

// Free all dynamically allocated memory associated with a catalog.
SqlError free_catalog(Catalog* c);

////
// Database operations
////
// TODO add create, delete, open, close db instance

////
// Table operations
////
// TODO add create, drop, alter table

////
// Record operations
////
// TODO add (select, insert, update, delete)

#endif//DATABASE_H