// tests/test_text_board_serializer.cpp
#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <vector>
#include "board.hpp"
#include "tile.hpp"
#include "tile_state.hpp"
#include "text_board_serializer.hpp"

namespace {
    // Utility: parse first line "R C M"
    struct Header { int r{}, c{}, m{}; };
    Header parseHeader(const std::string& s) {
        std::istringstream ss(s);
        Header h{};
        ss >> h.r >> h.c >> h.m;
        return h;
    }

    // Utility: split into lines
    std::vector<std::string> splitLines(const std::string& s) {
        std::vector<std::string> lines;
        std::istringstream ss(s);
        std::string line;
        while (std::getline(ss, line)) lines.push_back(line);
        return lines;
    }
}

// --- 1) Round-trip should preserve the whole board (dimensions + tiles) ---

TEST(TextBoardSerializer, SaveThenLoad_RoundTripPreservesBoard) {
    // Start with a random board (constructor injects TextBoardSerializer for us)
    Board original(9, 9, 10);

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

    // Use the provided operator== on Board (present in your code)
    EXPECT_TRUE(restored == original) << "Round-trip mismatch: restored board != original";
}

// --- 2) Header must contain rows cols mines exactly as written ---

TEST(TextBoardSerializer, Save_WritesCorrectHeader) {
    const int R = 5, C = 6, M = 7;
    Board b(R, C, M);

    std::stringstream ss;
    ASSERT_EQ(b.save(ss), 0);

    const auto out = ss.str();
    auto lines = splitLines(out);
    ASSERT_FALSE(lines.empty()) << "Serialized output should have at least one line (header)";

    Header h = parseHeader(lines.front());
    EXPECT_EQ(h.r, R);
    EXPECT_EQ(h.c, C);
    EXPECT_EQ(h.m, M);
}

// --- 3) Payload should have exactly rows*cols*3 serialized scalars
// (TileState as int, bool isMine, unsigned adjacentMines) ---

TEST(TextBoardSerializer, Save_WritesTripletsPerTile) {
    const int R = 4, C = 3, M = 5;
    Board b(R, C, M);

    // Make a few state changes to ensure non-trivial values are written
    b.toggleTile(0, 0);
    (void)b.revealTile(1, 1);

    std::stringstream ss;
    ASSERT_EQ(b.save(ss), 0);

    // Pull the full text and discard header tokens; then count remaining tokens
    std::string all = ss.str();
    std::istringstream is(all);

    int rr, cc, mm;
    ASSERT_TRUE(is >> rr >> cc >> mm);
    ASSERT_EQ(rr, R);
    ASSERT_EQ(cc, C);
    ASSERT_EQ(mm, M);

    // Count the remaining scalars
    size_t count = 0;
    std::string tok;
    while (is >> tok) ++count;

    // Expect 3 scalars per tile
    EXPECT_EQ(count, static_cast<size_t>(R * C * 3));
}

// --- 4) Load should overwrite an existing board’s size and content ---

TEST(TextBoardSerializer, Load_OverwritesDimensionsAndState) {
    Board source(7, 8, 9);
    // Change some state so we're not just default COVERED everywhere
    source.toggleTile(2, 3);
    (void)source.revealTile(3, 4);

    std::stringstream ss;
    ASSERT_EQ(source.save(ss), 0);

    // Destination starts with totally different dimensions
    Board dest(2, 2, 1);
    ASSERT_NE(dest.getRows(), source.getRows());
    ASSERT_NE(dest.getColumns(), source.getColumns());

    ASSERT_EQ(dest.load(ss), 0);

    EXPECT_EQ(dest.getRows(), source.getRows());
    EXPECT_EQ(dest.getColumns(), source.getColumns());
    EXPECT_EQ(dest.getMines(), source.getMines());
    EXPECT_TRUE(dest == source);
}
