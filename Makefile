# ====== Minesweeper Makefile (C++) ======

# Compiler and flags
CXX      := g++
CXXFLAGS := -Wall -Wextra -O2 -std=c++17 -MMD -MP

# Directories
SRC_DIR := src
BUILD_DIR := build

# Source and header files
SRCS    := $(SRC_DIR)/main.cpp $(SRC_DIR)/minesweeper.cpp
OBJS    := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
EXEC    := $(BUILD_DIR)/minesweeper

# Libraries
LIBS    := -lncurses

# ====== Build Rules ======

# Default target
all: $(EXEC)

# Link final binary
$(EXEC): $(OBJS) | $(BUILD_DIR)
	@echo "Linking $@ ..."
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LIBS)

# Compile source files (.cpp -> .o)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@echo "Compiling $< ..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create necessary folders
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean up build artifacts
clean:
	@echo "Cleaning..."
	rm -rf $(BUILD_DIR)

# Run the game
run: all
	./$(EXEC)

# Auto-generated header dependencies
DEPS := $(OBJS:.o=.d)
-include $(DEPS)

# ====== Convenience ======
.PHONY: all clean run