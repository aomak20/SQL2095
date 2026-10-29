SRC_DIR = src
BUILD_DIR = build

CC ?= cc
CFLAGS ?= -Wall -Wextra -I$(SRC_DIR)

LEXER_TEST_SOURCES = testing/lexer_test.c \
	$(SRC_DIR)/lexer.c \
	$(SRC_DIR)/sql_error.c 

lexer_test: $(LEXER_TEST_SOURCES)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LEXER_TEST_SOURCES) -o $(BUILD_DIR)/$@

PARSER_TEST_SOURCES = testing/parser_test.c \
	$(SRC_DIR)/parser.c \
	$(SRC_DIR)/lexer.c \
	$(SRC_DIR)/sql_error.c 

parser_test: $(PARSER_TEST_SOURCES)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(PARSER_TEST_SOURCES) -o $(BUILD_DIR)/$@

.PHONY: all clean lexer_test parser_test
all: lexer_test parser_test
clean:
	@rm -rf build/