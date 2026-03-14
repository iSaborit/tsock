CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -Iinclude
BUILD_DIR := build
BIN_DIR := bin
TARGET := $(BIN_DIR)/tsock
SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRC))

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $(OBJ) -o $@

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

clean:
	/usr/bin/rm -rf -- $(BUILD_DIR) $(TARGET)
