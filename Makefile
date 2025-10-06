# === Compiler and flags ===
CC      := gcc
CFLAGS  := -Wall -Wextra -pedantic -std=c11
BUILD   := build
TARGET  := minesweeper

# === Source and object files ===
SRC_DIR := src
OBJ_DIR := $(BUILD)
SRC     := $(wildcard $(SRC_DIR)/*.c)
OBJ     := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

# === Default target ===
all: $(BUILD) $(TARGET)

# === Link final executable ===
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# === Compile each .c into .o ===
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/minesweeper.h
	$(CC) $(CFLAGS) -c $< -o $@

# === Ensure build directory exists ===
$(BUILD):
	mkdir -p $(BUILD)

# === Run the program ===
run: all
	./$(TARGET)

# === Debug build ===
debug: CFLAGS += -g -O0
debug: clean all

# === Clean build artifacts ===
clean:
	rm -rf $(BUILD) $(TARGET)

# === Phony targets ===
.PHONY: all run clean debug
