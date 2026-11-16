// File:        database.c
// Author:      Andrei M
// Description: TODO

#include <stdio.h>
#include "database.h"
#include "sql_error.h"


// Constants

const uint32_t CATALOG_MAGIC        = 0xCA7A1060;   // Hex val to mark catalog
const uint8_t DB_TABLE_NAME_MAX_LEN  = UINT8_MAX;
const uint8_t DB_COLUMN_NAME_MAX_LEN = UINT8_MAX;


// Helper functions

// TODO should be relatively simple to write helper functions to read/write
// TableMeta and ColumnMeta from/to a FILE* with a TableMeta/ColumnMeta*


// Catalog operations

SqlError init_catalog(Catalog* c) {
    if (c == NULL) {
        return (SqlError) {
            .column = 0,
            .line = 0,
            .message = "Cannot initialize null catalog",
            .type = SQL_ERROR_INTERNAL
        };
    }

    c->count = 0;
    c->first = NULL;
    c->last = NULL;
    return SQL_ERROR_OK_MACRO;
}

SqlError read_catalog(Catalog* c, FILE* fp) {
    if (c == NULL || fp == NULL) {
        return (SqlError) {
            .column = 0,
            .line = 0,
            .message = "Catalog or file to be read is null",
            .type = SQL_ERROR_INTERNAL
        };
    }

    // TODO implement

    return SQL_ERROR_OK_MACRO;
}

SqlError write_catalog(const Catalog* c, FILE* fp) {
    if (c == NULL || fp == NULL) {
        return (SqlError) {
            .column = 0,
            .line = 0,
            .message = "Catalog or file to be written is null",
            .type = SQL_ERROR_INTERNAL
        };
    }

    rewind(fp);

    // Write magic value
    fwrite(&CATALOG_MAGIC, sizeof(CATALOG_MAGIC), 1, fp);

    // Write num tables
    // NOTE: We're writing count straight from the catalog instead of the actual
    // count of tables written. We trust that the catalog is right.
    fwrite(&c->count, sizeof(c->count), 1, fp);

    // Write each table
    // TODO implement, make sure to add write_tablemeta & write_columnmeta funcs

    return SQL_ERROR_OK_MACRO;
}

SqlError free_catalog(Catalog* c) {
    // TODO implement
    return SQL_ERROR_OK_MACRO;
}