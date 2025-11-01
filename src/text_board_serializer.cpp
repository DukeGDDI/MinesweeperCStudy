/*                                       
 *   _____ _                                       
 *  |     |_|___ ___ ___ _ _ _ ___ ___ ___ ___ ___ 
 *  | | | | |   | -_|_ -| | | | -_| -_| . | -_|  _|
 *  |_|_|_|_|_|_|___|___|_____|___|___|  _|___|_|  
 *                                  |_|          
 */

#include "text_board_serializer.hpp"
#include "minesweeper.hpp"

int TextBoardSerializer::save(const Board& board, ostream& out) {
    // Save rows, columns, mines
    out << board.getRows() << " " << board.getColumns() << " " << board.getMines() << "\n";
    // Save each tile's state
    for (int r = 0; r < board.getRows(); r++) {
        for (int c = 0; c < board.getColumns(); c++) {
            Tile tile = board.getTile(r, c);
            out << static_cast<int>(tile.state) << " " << tile.isMine << " " << tile.adjacentMines << "\n";
        }
    }
    return 0; // success
}

int TextBoardSerializer::load(Board& board, istream& in) {
    int r, c, m;
    in >> r >> c >> m;
    if (r <= 0 || c <= 0 || m < 0) {
        return -1; // invalid dimensions
    }
    Tile tile;
    board.reset(r, c, m);
    for (int row = 0; row < board.getRows(); row++) {
        for (int col = 0; col < board.getColumns(); col++) {
            int stateInt;
            bool isMine;
            unsigned int adjacentMines;
            in >> stateInt >> isMine >> adjacentMines;
            tile = board.getTile(row, col);
            tile.state = static_cast<TileState>(stateInt);
            tile.isMine = isMine;
            tile.adjacentMines = adjacentMines;
        }
    }
    return 0; // success
}