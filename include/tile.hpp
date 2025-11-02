#include <iostream>
#include "tile_state.hpp"

using namespace std;

#ifndef TILE
#define TILE
// Define the tile structure
struct Tile {
    TileState state = TileState::COVERED;
    bool isMine = false;
    unsigned int adjacentMines = 0;

    // Overload output operator for Tile for debugging only
    // Shows the tile content regardless of state
    friend ostream& operator<<(ostream& out, const Tile& tile);

    // Overload the equality operator for testing purposes
    friend bool operator==(const Tile& t1, const Tile& t2);
};
#endif