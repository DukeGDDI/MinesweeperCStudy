# =========================================
# Compiler setup
# =========================================
CXX      := g++
CXXFLAGS := -Wall -Wextra -O2 -std=c++17 -Iinclude -MMD -MP

# =========================================
# Directory structure
# =========================================
SRC_DIR   := src
INC_DIR   := include
TEST_DIR  := tests
TUI_DIR   := tui
OBJ_DIR   := build/obj
BIN_DIR   := build/bin

# =========================================
# Source files
# =========================================
SRC_FILES   := $(wildcard $(SRC_DIR)/*.cpp)
TEST_FILES  := $(wildcard $(TEST_DIR)/*.cpp)
TUI_FILES   := $(wildcard $(TUI_DIR)/*.cpp)

# =========================================
# Object and dependency files
# =========================================
OBJ_FILES   := $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES) $(TEST_FILES) $(TUI_FILES))
DEP_FILES   := $(OBJ_FILES:.o=.d)

# =========================================
# Executables
# =========================================
TEST_BIN := $(BIN_DIR)/ms_test
TUI_BIN  := $(BIN_DIR)/minesweeper

# =========================================
# Default target
# =========================================
all: $(TEST_BIN) $(TUI_BIN)

# =========================================
# Link rules
# =========================================
$(TEST_BIN): $(filter $(OBJ_DIR)/$(SRC_DIR)/%.o $(OBJ_DIR)/$(TEST_DIR)/%.o,$(OBJ_FILES))
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(TUI_BIN): $(filter $(OBJ_DIR)/$(SRC_DIR)/%.o $(OBJ_DIR)/$(TUI_DIR)/%.o,$(OBJ_FILES))
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ -lncursesw   # link ncursesw if using curses UI

# =========================================
# Compile rule (generic)
# =========================================
$(OBJ_DIR)/%.o: %.cpp $(INC_DIR)/minesweeper.hpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# =========================================
# Utility targets
# =========================================
run-test: $(TEST_BIN)
	$(TEST_BIN)

run-tui: $(TUI_BIN)
	$(TUI_BIN)

clean:
	rm -rf build

.PHONY: all clean run-test run-tui

# Auto-include header dependencies
#-include $(DEP_FILES)
