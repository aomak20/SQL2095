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

.PHONY: all clean lexer_test
all: lexer_test
clean:
	@rm -rf build/