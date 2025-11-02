/*                                       
 *   _____ _                                       
 *  |     |_|___ ___ ___ _ _ _ ___ ___ ___ ___ ___ 
 *  | | | | |   | -_|_ -| | | | -_| -_| . | -_|  _|
 *  |_|_|_|_|_|_|___|___|_____|___|___|  _|___|_|  
 *                                  |_|          
 */
// tests/test_board.cpp
#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include "board.hpp"
#include "tile.hpp"
#include "tile_state.hpp"

// Fixed test board (5 rows, 6 cols, 4 mines)
static const std::string kTestBoard = R"(
5 6 4
. . * . . .
. * . . . .
. . . * . .
. . . . . .
. . . . . *
)";

namespace {
    // Expected mine positions for the board above (0-based row,col)
    const std::vector<std::pair<int,int>> kMines {
        {0,2}, {1,1}, {2,3}, {4,5}
    };

    // Convenience to check membership in kMines
    bool isExpectedMine(int r, int c) {
        for (auto [mr, mc] : kMines) {
            if (mr == r && mc == c) return true;
        }
        return false;
    }
}

// ---------- Construction / shape ----------

TEST(Board_FromText, DimensionsAndMineCountMatch) {
    std::istringstream iss(kTestBoard);
    Board board(iss);

    EXPECT_EQ(board.getRows(), 5);
    EXPECT_EQ(board.getColumns(), 6);

    int actualMines = 0;
    for (int r = 0; r < board.getRows(); ++r) {
        for (int c = 0; c < board.getColumns(); ++c) {
            if (board.getTile(r, c).isMine) ++actualMines;
        }
    }
    EXPECT_EQ(actualMines, 4);
}

TEST(Board_FromText, InBoundsBehavior) {
    std::istringstream iss(kTestBoard);
    Board board(iss);

    // Corners in-bounds
    EXPECT_TRUE(board.inBounds(0, 0));
    EXPECT_TRUE(board.inBounds(0, board.getColumns()-1));
    EXPECT_TRUE(board.inBounds(board.getRows()-1, 0));
    EXPECT_TRUE(board.inBounds(board.getRows()-1, board.getColumns()-1));

    // Out-of-bounds
    EXPECT_FALSE(board.inBounds(-1, 0));
    EXPECT_FALSE(board.inBounds(0, -1));
    EXPECT_FALSE(board.inBounds(board.getRows(), 0));
    EXPECT_FALSE(board.inBounds(0, board.getColumns()));
}

TEST(Board_FromText, MineLocationsMatchLayout) {
    std::istringstream iss(kTestBoard);
    Board board(iss);

    for (int r = 0; r < board.getRows(); ++r) {
        for (int c = 0; c < board.getColumns(); ++c) {
            bool expectMine = isExpectedMine(r, c);
            EXPECT_EQ(board.getTile(r, c).isMine, expectMine) << "at (" << r << "," << c << ")";
        }
    }
}

// ---------- Adjacency counts ----------
//
// Validate a handful of known adjacency counts derived from kTestBoard.
// (We don't assert every cell to keep the test readable.)
TEST(Board_Adjacency, KnownCountsAreCorrect) {
    std::istringstream iss(kTestBoard);
    Board board(iss);

    // (0,1) touches mines at (0,2) and (1,1) => 2
    EXPECT_EQ(board.getTile(0,1).adjacentMines, 2u);

    // (1,2) touches (0,2) and (1,1) => 2
    EXPECT_EQ(board.getTile(1,2).adjacentMines, 3u);

    // (2,2) touches (2,3) and (1,1) => 2
    EXPECT_EQ(board.getTile(2,2).adjacentMines, 2u);

    // (3,4) touches (2,3) and (4,5) => 2
    EXPECT_EQ(board.getTile(3,4).adjacentMines, 2u);

    // (4,4) touches only (4,5) => 1
    EXPECT_EQ(board.getTile(4,4).adjacentMines, 1u);
}

// ---------- Reveal behavior ----------

TEST(Board_Reveal, RevealingSafeTileShowsRevealedAndNotGameOver) {
    std::istringstream iss(kTestBoard);
    Board board(iss);

    // Choose a safe tile known to be safe from the layout, e.g., (0,0)
    ASSERT_FALSE(board.getTile(0,0).isMine);
    EXPECT_EQ(board.getTile(0,0).state, TileState::COVERED);

    bool isGameOver = board.revealTile(0,0);

    EXPECT_NE(board.getTile(0,0).state, TileState::COVERED);
    EXPECT_NE(board.getTile(0,0).state, TileState::FLAGGED);
    EXPECT_FALSE(isGameOver);
}

TEST(Board_Reveal, RevealingMineExplodesAndEndsGame) {
    std::istringstream iss(kTestBoard);
    Board board(iss);

    // Reveal a known mine, e.g., (0,2)
    ASSERT_TRUE(board.getTile(0,2).isMine);

    bool isGameOver = board.revealTile(0,2);

    EXPECT_TRUE(isGameOver);
    EXPECT_EQ(board.getTile(0,2).state, TileState::EXPLODED);
}

// ---------- Flag/mark cycle ----------

TEST(Board_Flagging, ToggleTileCyclesMarkStatesFromCovered) {
    std::istringstream iss(kTestBoard);
    Board board(iss);

    // Use a safe, covered tile
    int r = 0, c = 0;
    ASSERT_EQ(board.getTile(r,c).state, TileState::COVERED);

    // 1st toggle -> FLAGGED
    board.toggleTile(r,c);
    EXPECT_EQ(board.getTile(r,c).state, TileState::FLAGGED);

    // 2nd toggle -> QUESTIONED (common convention)
    board.toggleTile(r,c);
    EXPECT_EQ(board.getTile(r,c).state, TileState::QUESTIONED);

    // 3rd toggle -> back to COVERED
    board.toggleTile(r,c);
    EXPECT_EQ(board.getTile(r,c).state, TileState::COVERED);
}

// ---------- Save & Load ---------------

TEST(Board_SaveLoad, SaveThenLoad_RoundTripPreservesBoard) {
    // Start with a random board (constructor injects TextBoardSerializer for us)
    Board original(9, 9, 10);
    ASSERT_EQ(original.getRows(), 9);
    ASSERT_EQ(original.getColumns(), 9);
    ASSERT_EQ(original.getMines(), 10);

    // Do a couple of state changes so we capture more than initial COVERED:
    // Toggle a few flags (in-bounds by construction).
    original.toggleTile(0, 0);
    original.toggleTile(0, 1);
    // Reveal one tile (may explode or reveal—either way should be preserved)
    (void)original.revealTile(1, 1);

    std::stringstream buffer;
    ASSERT_EQ(original.save(buffer), 0) << "Save should return 0 on success";

    // Load into a different board shape to ensure load actually overwrites
    Board restored(5, 4, 3);
    ASSERT_EQ(restored.load(buffer), 0) << "Load should return 0 on success";
    ASSERT_EQ(restored.getRows(), original.getRows());
    ASSERT_EQ(restored.getColumns(), original.getColumns());
    ASSERT_EQ(restored.getMines(), original.getMines());

    // Use the provided operator== on Board (present in your code)
    // EXPECT_TRUE(restored == original) << "Round-trip mismatch: restored board != original";
    for (int r=0; r<9; r++) {
        for (int c=0; c<9; c++) {
            Tile ot = original.getTile(r, c);
            Tile rt = restored.getTile(r, c);
            ASSERT_EQ(ot.isMine, rt.isMine) << "Tile.isMine at [" << r << "," << c << "] Not Equal";;
            ASSERT_EQ(ot.adjacentMines, rt.adjacentMines) << "Tile.adjacentMines at [" << r << "," << c << "] Not Equal";;
            ASSERT_EQ(ot.state, rt.state) << "Tile.state at [" << r << "," << c << "] Not Equal";;
            //ASSERT_EQ(original.getTile(r, c), restored.getTile(r, c)) << "Tiles at [" << r << "," << c << "] Not Equal";
        }
    }
}