CC = gcc
CFLAGS = -Wall -Iinclude
SRC = src/inip.c
OBJ = $(SRC:.c=.o)
TEST_SRC = tests/inip_test.c
TEST_OBJ = $(TEST_SRC:.c=.o)
TARGET = inip.a

all: $(TARGET)

$(TARGET): $(OBJ)
	ar rcs $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

test: $(TEST_OBJ) $(TARGET)
	$(CC) $(CFLAGS) -o inip_test $^
	./inip_test

clean:
	rm -f $(OBJ) $(TEST_OBJ) $(TARGET) inip_test

.PHONY: all test clean
