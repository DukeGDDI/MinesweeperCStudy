#include "minesweeper.hpp"

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

// Overload output operator for Board for debugging only
// Shows all tiles regardless of state (e.g., covered tiles are shown)
ostream& operator<<(ostream& out, const Board& board) {
    for (int r = 0; r < board.rows; r++) {
        for (int c = 0; c < board.columns; c++) {
            out << board.tiles[r][c] << " ";
        }
        out << "\n";
    }
    return out;
}

Board::Board(int rows, int columns, int mines) {
    this->rows = rows;
    this->columns = columns;
    this->mines = mines;
    this->tiles.resize(this->rows, vector<Tile>(this->columns));
    this->layMines();
    this->calculateAdjacents();
}

// Create Board from a stream (file).  This not the same as restoring a game 
// from a file (see load() method).  This is used to create repeatable starting
// boards that make testing simpler.
//
// Example format:
// --
// 5 5 4
// . . * . . .
// . * . . . .
// . . . * . .
// . . . . . .
// . . . . . *
Board::Board(istream& in) {
    in >> this->rows >> this->columns >> this->mines;
    this->tiles.resize(this->rows, vector<Tile>(this->columns));
    for (int r = 0; r < this->rows; r++) {
        for (int c = 0; c < this->columns; c++) {
            char ch;
            in >> ch;
            this->tiles[r][c].state = COVERED; // redundant but explicit
            this->tiles[r][c].isMine = (ch == '*') ? true : false;
            this->tiles[r][c].adjacentMines = 0; // redundant but explicit
        }
    }
    this->calculateAdjacents();
}

bool Board::revealTile(int row, int col) {
    Tile& tile = this->tiles[row][col];
    if (tile.state == REVEALED || tile.state == FLAGGED || tile.state == QUESTIONED) {
        return false; // do nothing
    }
    if (tile.isMine) {
        tile.state = EXPLODED;
        return true; // mine revealed
    }
    // Reveal this tile
    tile.state = REVEALED;
    // If no adjacent mines, reveal neighbors
    if (tile.adjacentMines == 0) {
        for (int dr = -1; dr <= 1; dr++) {
            for (int dc = -1; dc <= 1; dc++) {
                if (dr == 0 && dc == 0) continue; // skip self
                int nr = row + dr;
                int nc = col + dc;
                if (inBounds(nr, nc)) {
                    revealTile(nr, nc);
                }
            }
        }
    }
    return false; // no mine revealed
}

TileState Board::toggleTile(int row, int col) {
    Tile& tile = this->tiles[row][col];
    switch (tile.state) {
        case COVERED:
            tile.state = FLAGGED;
            break;
        case FLAGGED:
            tile.state = QUESTIONED;
            break;
        case QUESTIONED:
            tile.state = COVERED;
            break;
        default:
            // Do nothing for REVEALED or EXPLODED
            // should not happen
            break;
    }
    return tile.state;
}

bool Board::inBounds(int row, int col) {
    return (row >= 0 && row < this->rows && col >= 0 && col < this->columns);
}

// Randomly place mines and calculate adjacent mine counts
void Board::layMines() {
    // Randomly place mines and calculate adjacent mine counts
    srand(time(NULL));
    int placed = 0;
    while (placed < mines) {
        int r = rand() % this->rows;
        int c = rand() % this->columns;
        if (!this->tiles[r][c].isMine) {
            this->tiles[r][c].isMine = 1;
            placed++;
            // Update adjacent mine counts
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    if (inBounds(r + dr, c + dc)) {
                        this->tiles[r + dr][c + dc].adjacentMines++;
                    }
                }
            }
        }
    }
}

// Calculate adjacent mine counts for all tiles
void Board::calculateAdjacents() {
    for (int r = 0; r < this->rows; r++) {
        for (int c = 0; c < this->columns; c++) {
            // Skip mines
            if (this->tiles[r][c].isMine) continue;

            unsigned int count = 0;
            // Check all neighbors
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    if (dr == 0 && dc == 0) continue; // skip self
                    int nr = r + dr;
                    int nc = c + dc;
                    if (inBounds(nr, nc) && this->tiles[nr][nc].isMine) {
                        count++;
                    }
                }
            }
            this->tiles[r][c].adjacentMines = count;
        }
    }
}
