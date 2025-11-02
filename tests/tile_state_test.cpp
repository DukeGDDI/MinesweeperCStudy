/*                                       
 *   _____ _                                       
 *  |     |_|___ ___ ___ _ _ _ ___ ___ ___ ___ ___ 
 *  | | | | |   | -_|_ -| | | | -_| -_| . | -_|  _|
 *  |_|_|_|_|_|_|___|___|_____|___|___|  _|___|_|  
 *                                  |_|          
 */
// tests/test_tile_state.cpp
#include <gtest/gtest.h>
#include <sstream>
#include "tile_state.hpp"

TEST(TileState_Stream, CoveredPrintsCovered) {
    std::ostringstream os;
    os << TileState::COVERED;
    EXPECT_EQ(os.str(), "COVERED");
}

TEST(TileState_Stream, RevealedPrintsRevealed) {
    std::ostringstream os;
    os << TileState::REVEALED;
    EXPECT_EQ(os.str(), "REVEALED");
}

TEST(TileState_Stream, FlaggedPrintsFlagged) {
    std::ostringstream os;
    os << TileState::FLAGGED;
    EXPECT_EQ(os.str(), "FLAGGED");
}

TEST(TileState_Stream, QuestionedPrintsQuestioned) {
    std::ostringstream os;
    os << TileState::QUESTIONED;
    EXPECT_EQ(os.str(), "QUESTIONED");
}

TEST(TileState_Stream, ExplodedPrintsExploded) {
    std::ostringstream os;
    os << TileState::EXPLODED;
    EXPECT_EQ(os.str(), "EXPLODED");
}

TEST(TileState_Stream, InvalidValuePrintsUnknown) {
    // Exercise the default branch in operator<<
    auto bogus = static_cast<TileState>(999);
    std::ostringstream os;
    os << bogus;
    EXPECT_EQ(os.str(), "UNKNOWN");
}
