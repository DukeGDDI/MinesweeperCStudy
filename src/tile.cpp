/*                                       
 *   _____ _                                       
 *  |     |_|___ ___ ___ _ _ _ ___ ___ ___ ___ ___ 
 *  | | | | |   | -_|_ -| | | | -_| -_| . | -_|  _|
 *  |_|_|_|_|_|_|___|___|_____|___|___|  _|___|_|  
 *                                  |_|          
 */
#include <iostream>
#include "tile.hpp"

using namespace std;

// Overload output operator for Tile
ostream& operator<<(ostream& out, const Tile& tile) {
    if (tile.isMine) {
        out << "*";
        return out;
    }
    if (tile.adjacentMines == 0) {
        out << ".";
        return out;
    }
    out << tile.adjacentMines;
    return out;
}

// Overload the equality operator for testing purposes
bool operator==(const Tile& t1, const Tile& t2) {
    return (t1.state == t2.state &&
            t1.isMine == t2.isMine &&
            t1.adjacentMines == t2.adjacentMines);
}