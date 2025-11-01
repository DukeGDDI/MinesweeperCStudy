#include <iostream>
#include "tile_state.hpp"

using namespace std;


// Overload output operator for TileState for debugging only
ostream& operator<<(ostream& out, const TileState& state) {
    switch (state) {
        case COVERED:
            out << "COVERED";
            break;
        case REVEALED:
            out << "REVEALED";
            break;
        case FLAGGED:
            out << "FLAGGED";
            break;
        case QUESTIONED:
            out << "QUESTIONED";
            break;
        case EXPLODED:
            out << "EXPLODED";
            break;
        default:
            out << "UNKNOWN";
            break;
    }
    return out;
}

