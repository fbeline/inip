CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Wno-unused-parameter \
         -Wno-unused-function -Wno-gnu-zero-variadic-macro-arguments
SANITIZE_FLAGS = -fsanitize=undefined,address

DEBUG_DIR = build/debug

# Automatically find all .c files in src/ directory
SRC_DIR = src
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRC:$(SRC_DIR)/%.c=%.o)

DEBUG_OBJS = $(addprefix $(DEBUG_DIR)/, $(OBJS))

TARGET = inip

all: test

debug: CFLAGS += $(SANITIZE_FLAGS) -g
debug: $(DEBUG_DIR)/$(TARGET)

$(DEBUG_DIR)/$(TARGET): $(DEBUG_OBJS)
	$(CC) $(CFLAGS) -o $@ $(DEBUG_OBJS)

$(DEBUG_DIR)/%.o: $(SRC_DIR)/%.c | $(DEBUG_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DEBUG_DIR):
	mkdir -p $(DEBUG_DIR)

test: debug
	./$(DEBUG_DIR)/$(TARGET)

clean:
	rm -rf build/

.PHONY: all clean test
