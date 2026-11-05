// File:        parser_test.c
// Author:      Andrei M
// Description: TODO

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "parser.h"
#include "lexer.h"
#include "sql_error.h"

static char *read_all_input(FILE* stream, size_t* out_len)
{
    size_t len = 0;
    size_t capacity = 4096;
    char* buffer = malloc(capacity);
    if (buffer == NULL) return NULL;

    char c;
    while ((c = fgetc(stream)) != EOF) {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char* tmp = realloc(buffer, capacity);
            if (tmp == NULL) {
                free(buffer);
                return NULL;
            }
            buffer = tmp;
        }
        buffer[len] = (char)c;
        len++;
    }

    if (ferror(stream)) {
        free(buffer);
        return NULL;
    }

    buffer[len] = '\0';
    *out_len = len;
    return buffer;
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        fprintf(stderr, "Usage: parser_test\n");
        fprintf(stderr, "       parser_test (filename)\n");
        return 1;
    }

    FILE *input = stdin;

    if (argc == 2) {
        input = fopen(argv[1], "r");
        if (input == NULL) {
            perror(argv[1]);
            return 1;
        }
    }

    size_t data_len;
    char* data = read_all_input(input, &data_len);

    if (input != stdin)
        fclose(input);

    if (data == NULL) {
        fprintf(stderr, "Failed to read input\n");
        return 1;
    }

    TokenArray tokens;
    SqlError result = lexer_tokenize(data, data_len, &tokens);
    if (result.type != SQL_ERROR_OK) {
        print_error_info(&result);
        goto lexer_cleanup;
    }

    ProgramNode program;
    result = parser_generate(&tokens, &program);
    if (result.type != SQL_ERROR_OK) {
        print_error_info(&result);
        goto parser_cleanup;
    }
    // TODO print the AST starting from program
    
parser_cleanup:
    free_program(&program);

lexer_cleanup:
    lexer_free_token_array(&tokens);
    free(data);
    if (result.type == SQL_ERROR_OK) return 0;
    else return 1;
}