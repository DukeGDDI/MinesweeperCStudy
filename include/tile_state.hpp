/*                                       
 *   _____ _                                       
 *  |     |_|___ ___ ___ _ _ _ ___ ___ ___ ___ ___ 
 *  | | | | |   | -_|_ -| | | | -_| -_| . | -_|  _|
 *  |_|_|_|_|_|_|___|___|_____|___|___|  _|___|_|  
 *                                  |_|          
 */
#include <iostream>
using namespace std;

#ifndef TILE_STATE
#define TILE_STATE
// Define the tile states
 enum TileState {
    COVERED,
    REVEALED,
    FLAGGED,
    QUESTIONED,
    EXPLODED
};

// Overload output operator for TileState for debugging only
ostream& operator<<(ostream& out, const TileState& state);

#endif