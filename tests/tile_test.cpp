// tests/test_tile.cpp
#include <gtest/gtest.h>
#include <sstream>
#include "tile.hpp"
#include "tile_state.hpp"

// NOTE: Tile::adjacentMines is not initialized in the header.
// Tests avoid asserting its default value and set it explicitly
// before any checks that depend on it.

TEST(Tile_Defaults, DefaultStateAndMineFlag) {
    Tile t;
    EXPECT_EQ(t.state, TileState::COVERED);
    EXPECT_FALSE(t.isMine);
    // Do NOT assert on t.adjacentMines; it is uninitialized by default.
}

TEST(Tile_Stream, MinePrintsAsteriskRegardlessOfStateOrAdjCount) {
    Tile t;
    t.isMine = true;
    t.adjacentMines = 0;
    t.state = TileState::COVERED;

    std::ostringstream os1;
    os1 << t;
    EXPECT_EQ(os1.str(), "*");

    // Change state & adjacentMines; still prints "*"
    t.state = TileState::FLAGGED;
    t.adjacentMines = 7;

    std::ostringstream os2;
    os2 << t;
    EXPECT_EQ(os2.str(), "*");
}

TEST(Tile_Stream, ZeroAdjacentNonMinePrintsDotRegardlessOfState) {
    Tile t;
    t.isMine = false;
    t.adjacentMines = 0;

    // Try multiple states; output should remain "."
    for (TileState s : {TileState::COVERED, TileState::REVEALED,
                        TileState::FLAGGED, TileState::QUESTIONED}) {
        t.state = s;
        std::ostringstream os;
        os << t;
        EXPECT_EQ(os.str(), ".");
    }
}

TEST(Tile_Stream, NonZeroAdjacentNonMinePrintsNumber) {
    Tile t;
    t.isMine = false;

    for (unsigned adj : {1u, 2u, 3u, 8u}) {
        t.adjacentMines = adj;
        t.state = TileState::REVEALED; // state is ignored by operator<<
        std::ostringstream os;
        os << t;
        EXPECT_EQ(os.str(), std::to_string(adj));
    }
}

TEST(Tile_Equality, EqualWhenAllFieldsMatch) {
    Tile a, b;
    a.state = TileState::QUESTIONED;
    a.isMine = false;
    a.adjacentMines = 4;

    b.state = TileState::QUESTIONED;
    b.isMine = false;
    b.adjacentMines = 4;

    EXPECT_TRUE(a == b);
}

TEST(Tile_Equality, NotEqualWhenStateDiffers) {
    Tile a, b;
    a.state = TileState::COVERED;
    b.state = TileState::FLAGGED;
    a.isMine = b.isMine = false;
    a.adjacentMines = b.adjacentMines = 0;

    EXPECT_FALSE(a == b);
}

TEST(Tile_Equality, NotEqualWhenMineFlagDiffers) {
    Tile a, b;
    a.state = b.state = TileState::REVEALED;
    a.isMine = true;
    b.isMine = false;
    a.adjacentMines = b.adjacentMines = 2;

    EXPECT_FALSE(a == b);
}

TEST(Tile_Equality, NotEqualWhenAdjacentCountDiffers) {
    Tile a, b;
    a.state = b.state = TileState::REVEALED;
    a.isMine = b.isMine = false;
    a.adjacentMines = 1;
    b.adjacentMines = 2;

    EXPECT_FALSE(a == b);
}
