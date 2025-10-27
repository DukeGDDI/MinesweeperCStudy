#include "minesweeper.hpp"
#include <cassert>


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

// Overload the equality operator for testing purposes
bool operator==(const Board& b1, const Board& b2) {
    if (b1.rows != b2.rows || b1.columns != b2.columns || b1.mines != b2.mines) {
        return false;
    }
    for (int r = 0; r < b1.rows; r++) {
        for (int c = 0; c < b1.columns; c++) {
            if (!(b1.tiles[r][c] == b2.tiles[r][c])) {
                return false;
            }
        }
    }
    return true;
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

// @return number of rows
int Board::getRows() {
    return this->rows;
}

// @return number of columns
int Board::getColumns() {
    return this->columns;
}

// Get tile state at (row,col)
Tile Board::getTile(int row, int col) {
    // Assert is in bounds
    assert(inBounds(row, col) && "getTile: (row,col) out of bounds");
    return this->tiles[row][col];
}

bool Board::revealTile(int row, int col) {
    // Assert is in bounds
    assert(inBounds(row, col) && "revealTile: (row,col) out of bounds");
    
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
    // Assert is in bounds
    assert(inBounds(row, col) && "toggleTile: (row,col) out of bounds");

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

int Board::save(ostream& out) {
    // Save rows, columns, mines
    out << this->rows << " " << this->columns << " " << this->mines << "\n";
    // Save each tile's state
    for (int r = 0; r < this->rows; r++) {
        for (int c = 0; c < this->columns; c++) {
            Tile& tile = this->tiles[r][c];
            out << static_cast<int>(tile.state) << " " << tile.isMine << " " << tile.adjacentMines << "\n";
        }
    }
    return 0; // success
}

int Board::load(istream& in) {
    int r, c, m;
    in >> r >> c >> m;
    if (r <= 0 || c <= 0 || m < 0) {
        return -1; // invalid dimensions
    }
    this->rows = r;
    this->columns = c;
    this->mines = m;
    this->tiles.resize(this->rows, vector<Tile>(this->columns));
    for (int row = 0; row < this->rows; row++) {
        for (int col = 0; col < this->columns; col++) {
            int stateInt;
            bool isMine;
            unsigned int adjacentMines;
            in >> stateInt >> isMine >> adjacentMines;
            this->tiles[row][col].state = static_cast<TileState>(stateInt);
            this->tiles[row][col].isMine = isMine;
            this->tiles[row][col].adjacentMines = adjacentMines;
        }
    }
    return 0; // success
}
