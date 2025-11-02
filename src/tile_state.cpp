/*                                       
 *   _____ _                                       
 *  |     |_|___ ___ ___ _ _ _ ___ ___ ___ ___ ___ 
 *  | | | | |   | -_|_ -| | | | -_| -_| . | -_|  _|
 *  |_|_|_|_|_|_|___|___|_____|___|___|  _|___|_|  
 *                                  |_|          
 */
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

