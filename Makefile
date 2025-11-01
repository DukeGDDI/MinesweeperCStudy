# =========================
# Minesweeper Makefile
# Layout assumed:
# include/*.hpp
# src/*.cpp                 -> core library
# tui/app.cpp               -> ncurses app
# tests/*.cpp               -> gtest unit tests
# build/bin/*               -> executables
# build/lib/*               -> static libs
# build/obj/*               -> object files
# =========================

# --- Compilers & Flags ---
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude -MMD -MP

# Try pkg-config first, then fall back to common linker flags
NCURSES_LIBS := $(shell pkg-config --libs ncurses 2>/dev/null || echo -lncurses)
GTEST_LIBS   := $(shell pkg-config --libs gtest 2>/dev/null || echo -lgtest -lgtest_main -lpthread)

# --- Paths ---
BUILD_DIR := build
BIN_DIR   := $(BUILD_DIR)/bin
LIB_DIR   := $(BUILD_DIR)/lib
OBJ_DIR   := $(BUILD_DIR)/obj

SRC_DIR   := src
TUI_DIR   := tui
TEST_DIR  := tests

# --- Targets (files) ---
APP_EXE   := $(BIN_DIR)/minesweeper
TEST_EXE  := $(BIN_DIR)/minesweeper_tests
LIB_FILE  := $(LIB_DIR)/minesweeperlib.a   # static lib to link app & tests

# --- Sources & Objects ---
# All core sources become part of the static library
SRCS_CORE := $(wildcard $(SRC_DIR)/*.cpp)
OBJS_CORE := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/$(SRC_DIR)/%.o,$(SRCS_CORE))

# App (ncurses)
SRC_APP   := $(TUI_DIR)/app.cpp
OBJ_APP   := $(OBJ_DIR)/$(TUI_DIR)/app.o

# Tests
SRCS_TEST := $(wildcard $(TEST_DIR)/*.cpp)
OBJS_TEST := $(patsubst $(TEST_DIR)/%.cpp,$(OBJ_DIR)/$(TEST_DIR)/%.o,$(SRCS_TEST))

# --- Phony targets ---
.PHONY: all clean buildlib buildapp buildtests test

# all: clean, then build everything
all: clean buildlib buildapp buildtests

# clean: blow away build dir
clean:
	@echo ">> Cleaning build directory"
	@rm -rf $(BUILD_DIR)

# buildlib: archive core sources into a static library
buildlib: $(LIB_FILE)

# buildapp: build ncurses app (depends on lib)
buildapp: $(APP_EXE)

# buildtests: compile all tests & link (depends on lib)
buildtests: $(TEST_EXE)

# test: run the compiled tests
test: buildtests
	@echo ">> Running tests"
	@$(TEST_EXE)

# --- Link rules ---
$(APP_EXE): $(OBJ_APP) $(LIB_FILE) | $(BIN_DIR)
	@echo ">> Linking $@"
	@$(CXX) $(CXXFLAGS) -o $@ $(OBJ_APP) $(LIB_FILE) $(NCURSES_LIBS)

$(TEST_EXE): $(OBJS_TEST) $(LIB_FILE) | $(BIN_DIR)
	@echo ">> Linking $@"
	@$(CXX) $(CXXFLAGS) -o $@ $(OBJS_TEST) $(LIB_FILE) $(GTEST_LIBS)

# --- Library archive rule ---
$(LIB_FILE): $(OBJS_CORE) | $(LIB_DIR)
	@echo ">> Archiving $@"
	@ar rcs $@ $(OBJS_CORE)

# --- Compile rules (pattern) ---
# Core objects
$(OBJ_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)/$(SRC_DIR)
	@echo ">> Compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# App object
$(OBJ_DIR)/$(TUI_DIR)/%.o: $(TUI_DIR)/%.cpp | $(OBJ_DIR)/$(TUI_DIR)
	@echo ">> Compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Test objects
$(OBJ_DIR)/$(TEST_DIR)/%.o: $(TEST_DIR)/%.cpp | $(OBJ_DIR)/$(TEST_DIR)
	@echo ">> Compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Auto-create directories ---
$(BIN_DIR) $(LIB_DIR) $(OBJ_DIR) $(OBJ_DIR)/$(SRC_DIR) $(OBJ_DIR)/$(TUI_DIR) $(OBJ_DIR)/$(TEST_DIR):
	@mkdir -p $@

# --- Dependencies ---
# Include auto-generated .d files if they exist
DEP_FILES := \
  $(OBJS_CORE:.o=.d) \
  $(OBJ_APP:.o=.d)  \
  $(OBJS_TEST:.o=.d)

-include $(DEP_FILES)
