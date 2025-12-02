cmake_minimum_required(VERSION 3.16)
project(Minesweeper LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------
# 1. Locations
# ------------------------------------------------------------

set(MS_ROOT_DIR    ${CMAKE_SOURCE_DIR})
set(MS_INCLUDE_DIR ${MS_ROOT_DIR}/include)
set(MS_SRC_DIR     ${MS_ROOT_DIR}/src)
set(MS_TEST_DIR    ${MS_ROOT_DIR}/tests)

# Make sure EVERY target sees include/
# This is global, so even if we mess up per-target includes, it still works.
include_directories(${MS_INCLUDE_DIR})

# ------------------------------------------------------------
# 2. Library: minesweeperlib (game logic)
#    -> all src/*.cpp EXCEPT main.cpp
# ------------------------------------------------------------

file(GLOB MS_LIB_SOURCES
    "${MS_SRC_DIR}/*.cpp"
)

list(REMOVE_ITEM MS_LIB_SOURCES "${MS_SRC_DIR}/main.cpp")

add_library(minesweeperlib ${MS_LIB_SOURCES})

# Public include directory for consumers (tests, app)
target_include_directories(minesweeperlib
    PUBLIC
        ${MS_INCLUDE_DIR}
)

# ------------------------------------------------------------
# 3. App: minesweeper (ncurses ASCII UI)
# ------------------------------------------------------------

add_executable(minesweeper
    ${MS_SRC_DIR}/main.cpp
)

find_package(Curses REQUIRED)

target_link_libraries(minesweeper
    PRIVATE
        minesweeperlib
        ${CURSES_LIBRARIES}
)

target_include_directories(minesweeper
    PRIVATE
        ${MS_INCLUDE_DIR}
        ${CURSES_INCLUDE_DIR}
)

# ------------------------------------------------------------
# 4. Tests: minesweeper_tests (googletest via FetchContent)
# ------------------------------------------------------------

include(CTest)
enable_testing()
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
)
FetchContent_MakeAvailable(googletest)

file(GLOB MS_TEST_SOURCES
    "${MS_TEST_DIR}/*.cpp"
)

add_executable(minesweeper_tests
    ${MS_TEST_SOURCES}
)

target_link_libraries(minesweeper_tests
    PRIVATE
        minesweeperlib
        gtest_main
)

target_include_directories(minesweeper_tests
    PRIVATE
        ${MS_INCLUDE_DIR}
)

include(GoogleTest)
gtest_discover_tests(minesweeper_tests)
