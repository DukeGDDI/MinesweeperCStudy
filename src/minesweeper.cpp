/*                                       
 *   _____ _                                       
 *  |     |_|___ ___ ___ _ _ _ ___ ___ ___ ___ ___ 
 *  | | | | |   | -_|_ -| | | | -_| -_| . | -_|  _|
 *  |_|_|_|_|_|_|___|___|_____|___|___|  _|___|_|  
 *                                  |_|          
 */

#include "minesweeper.hpp"
#include "text_board_serializer.hpp"
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

Board::Board() : Board(16, 30, 99, std::make_shared<TextBoardSerializer>()) {}

Board::Board(int rows, int columns, int mines) : 
    Board(rows, columns, mines, std::make_shared<TextBoardSerializer>()) {}

Board::Board(int rows, int columns, int mines, std::shared_ptr<ISerializable> serializer) : 
    rows(rows), columns(columns), mines(mines), serializer(serializer) {
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
    Tile t;
    for (int r = 0; r < this->rows; r++) {
        for (int c = 0; c < this->columns; c++) {
            char ch;
            in >> ch;
            t = this->tiles[r][c];
            t.state = TileState::COVERED; // redundant but explicit
            t.isMine = (ch == '*') ? true : false;
            t.adjacentMines = 0; // redundant but explicit
        }
    }
    this->calculateAdjacents();
}

// @return number of rows
int Board::getRows() const {
    return this->rows;
}

// @return number of columns
int Board::getColumns() const {
    return this->columns;
}

// @return number of mines
int Board::getMines() const {
    return this->mines;
}

// Get tile state at (row,col)
const Tile& Board::getTile(int row, int col) const {
    // Assert is in bounds
    assert(inBounds(row, col) && "getTile: (row,col) out of bounds");
    return this->tiles[row][col];
}

bool Board::revealTile(int row, int col) {
    // Assert is in bounds
    assert(inBounds(row, col) && "revealTile: (row,col) out of bounds");
    
    Tile& tile = this->tiles[row][col];
    if (tile.state == TileState::REVEALED || tile.state == TileState::FLAGGED || tile.state == TileState::QUESTIONED) {
        return false; // do nothing
    }
    if (tile.isMine) {
        tile.state = TileState::EXPLODED;
        return true; // mine revealed
    }
    // Reveal this tile
    tile.state = TileState::REVEALED;
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
        case TileState::COVERED:
            tile.state = TileState::FLAGGED;
            break;
        case TileState::FLAGGED:
            tile.state = TileState::QUESTIONED;
            break;
        case TileState::QUESTIONED:
            tile.state = TileState::COVERED;
            break;
        default:
            // Do nothing for REVEALED or EXPLODED
            // should not happen
            break;
    }
    return tile.state;
}

bool Board::inBounds(int row, int col) const {
    return (row >= 0 && row < this->rows && col >= 0 && col < this->columns);
}

void Board::reset(int rows, int cols, int mines) {
    this->rows = rows;
    this->columns = cols;
    this->mines = mines;
    this->tiles.clear();
    this->tiles.resize(this->rows, vector<Tile>(this->columns));
    this->layMines();
    this->calculateAdjacents();
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
    return serializer->save(*this, out);
}

int Board::load(istream& in) {
    return serializer->load(*this, in);
}

