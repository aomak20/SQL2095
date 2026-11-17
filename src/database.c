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

// Read a ColumnMeta struct from a given file.
static SqlError read_column_meta(ColumnMeta* col, FILE* fp) {
    if (col == NULL || fp == NULL) {
        return (SqlError) {
            .column = 0,
            .line = 0,
            .message = "ColumnMeta or file to be read is null",
            .type = SQL_ERROR_INTERNAL
        };
    }

    return SQL_ERROR_OK_MACRO;
}

// Write a ColumnMeta struct to a given file.
static SqlError write_column_meta(const ColumnMeta* col, FILE* fp) {
    // TODO add proper error handling to fwrite's and RETURN_IF_ERR calls
    if (col == NULL || fp == NULL) {
        return (SqlError) {
            .column = 0,
            .line = 0,
            .message = "ColumnMeta or file to be written is null",
            .type = SQL_ERROR_INTERNAL
        };
    }

    // Write name and its length
    fwrite(&col->name_len, sizeof(col->name_len), 1, fp);
    fwrite(col->name, sizeof(*col->name), col->name_len, fp);

    // TODO implement the rest when ColumnMeta is fleshed out more

    return SQL_ERROR_OK_MACRO;
}

// Read a TableMeta struct from a given file.
static SqlError read_table_meta(TableMeta* table, FILE* fp) {
    if (table == NULL || fp == NULL) {
        return (SqlError) {
            .column = 0,
            .line = 0,
            .message = "TableMeta or file to be read is null",
            .type = SQL_ERROR_INTERNAL
        };
    }

    return SQL_ERROR_OK_MACRO;
}

// Write a TableMeta struct to a given file.
static SqlError write_table_meta(const TableMeta* table, FILE* fp) {
    // TODO add proper error handling to fwrite's and RETURN_IF_ERR calls
    if (table == NULL || fp == NULL) {
        return (SqlError) {
            .column = 0,
            .line = 0,
            .message = "TableMeta or file to be written is null",
            .type = SQL_ERROR_INTERNAL
        };
    }

    // Write name and its length
    fwrite(&table->name_len, sizeof(table->name_len), 1, fp);
    fwrite(table->name, sizeof(*table->name), table->name_len, fp);

    // Write number of columns
    // NOTE: Prematurely write count from `table`, trusting that it's right.
    fwrite(&table->count, sizeof(table->count), 1, fp);

    // Write each column
    ColumnMeta* col = table->first;
    while (col != NULL) {
        RETURN_IF_ERR(write_column_meta(col, fp));
        col = col->next;
    }

    return SQL_ERROR_OK_MACRO;
}


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
    // TODO add proper error handling to fwrite's and RETURN_IF_ERR calls
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
    TableMeta* table = c->first;
    while (table != NULL) {
        RETURN_IF_ERR(write_table_meta(table, fp));
        table = table->next;
    }

    return SQL_ERROR_OK_MACRO;
}

SqlError free_catalog(Catalog* c) {
    // TODO implement
    return SQL_ERROR_OK_MACRO;
}