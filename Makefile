# Compiler/flags
CXX      := g++
CXXFLAGS := -Wall -Wextra -O2 -std=c++17 -Iinclude

# Output dirs
BIN_DIR := build/bin
OBJ_DIR := build/obj
LIB_DIR := build/lib

# Core lib
LIB_SRC := src/minesweeper.cpp
LIB_OBJ := $(LIB_SRC:%.cpp=$(OBJ_DIR)/%.o)
LIB_A   := $(LIB_DIR)/libminesweeper.a

# Optional components (existence-checked)
TEST_SRC := $(wildcard tests/main.cpp)
TUI_SRC  := $(wildcard tui/app.cpp)

# Derived
TEST_OBJ := $(TEST_SRC:%.cpp=$(OBJ_DIR)/%.o)
TEST_BIN := $(BIN_DIR)/ms_test

TUI_OBJ  := $(TUI_SRC:%.cpp=$(OBJ_DIR)/%.o)
TUI_BIN  := $(BIN_DIR)/ms_tui

# Detect ncurses (prefer pkg-config)
UNAME_S := $(shell uname -s)
PKG_NCURSESW := $(shell pkg-config --libs --cflags ncursesw 2>/dev/null)
PKG_NCURSES  := $(shell pkg-config --libs --cflags ncurses  2>/dev/null)

ifeq ($(strip $(PKG_NCURSESW)),)
  ifeq ($(strip $(PKG_NCURSES)),)
    ifeq ($(UNAME_S),Darwin)
      NCURSES_LIBS := -lncurses
      NCURSES_CFLAGS :=
    else
      NCURSES_LIBS := -lncursesw
      NCURSES_CFLAGS :=
    endif
  else
    NCURSES_LIBS   := $(shell pkg-config --libs ncurses)
    NCURSES_CFLAGS := $(shell pkg-config --cflags ncurses)
  endif
else
  NCURSES_LIBS   := $(shell pkg-config --libs ncursesw)
  NCURSES_CFLAGS := $(shell pkg-config --cflags ncursesw)
endif

TUI_DEFS := -D_XOPEN_SOURCE_EXTENDED

.PHONY: all clean run-test run-tui
all: $(LIB_A) maybe-test maybe-tui

maybe-test: $(LIB_A)
ifeq ($(TEST_SRC),)
	@echo "(tests/main.cpp not found) skipping ms_test"
else
	@$(MAKE) $(TEST_BIN)
endif

maybe-tui: $(LIB_A)
ifeq ($(TUI_SRC),)
	@echo "(tui/app.cpp not found) skipping ms_tui"
else
	@$(MAKE) $(TUI_BIN)
endif

# Library
$(LIB_A): $(LIB_OBJ)
	@mkdir -p $(LIB_DIR)
	ar rcs $@ $^

# Generic compile rule
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link test binary (if present)
$(TEST_BIN): $(LIB_A) $(TEST_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Link TUI binary (if present)
$(TUI_BIN): $(LIB_A) $(TUI_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(TUI_DEFS) $(NCURSES_CFLAGS) $^ -o $@ $(NCURSES_LIBS)

clean:
	rm -rf build

run-test: $(TEST_BIN)
	$(TEST_BIN)

run-tui: $(TUI_BIN)
	$(TUI_BIN)