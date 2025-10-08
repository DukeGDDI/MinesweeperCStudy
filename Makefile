# ====== Minesweeper Makefile ======

# Compiler and flags
CC      := gcc
CFLAGS  := -Wall -Wextra -O2

# Directories and file lists
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

SRCS    := $(SRC_DIR)/main.c $(SRC_DIR)/minesweeper.c
OBJS    := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
EXEC    := $(BIN_DIR)/minesweeper

# Libraries
LIBS    := -lncursesw     # ncursesw for wide-character support

# ====== Build Rules ======

# Default target
all: $(EXEC)

# Link final binary
$(EXEC): $(OBJS) | $(BIN_DIR)
	@echo "Linking $@ ..."
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $< ..."
	$(CC) $(CFLAGS) -c $< -o $@

# Create necessary folders
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean up build artifacts
clean:
	@echo "Cleaning..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the game
run: all
	./$(EXEC)

# ====== Convenience ======
.PHONY: all clean run
