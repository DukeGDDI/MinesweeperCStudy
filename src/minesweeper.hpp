#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

// Define the tile states
 enum TileState {
    COVERED,
    REVEALED,
    FLAGGED,
    QUESTIONED,
    EXPLODED
};

// Define the tile structure
struct Tile {
    TileState state = COVERED;
    bool isMine = false;
    unsigned int adjacentMines;

    // Overload output operator for Tile for debugging only
    // Shows the tile content regardless of state
    friend ostream& operator<<(ostream& out, const Tile& tile);
};

class Board {
    int rows;
    int columns;
    int mines;

    vector<vector<Tile>> tiles;

    public:
    // Empty Constructor
    Board() : Board(10, 10, 10) {};

    // Main Constructor
    Board(int rows, int columns, int mines);

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
    Board(istream& in);

    // Reveal logic:
    // - If tile is FLAGGED/QUESTIONED/REVEALED: do nothing
    // - If tile is a mine: returns 1 to indicate explosion (the caller can handle game over
    //   (NOTE: function is also used for auto-spread after a safe click).
    // - If tile has adjacentMines > 0: reveal it and stop
    // - If tile has adjacentMines == 0: reveal it and recursively reveal neighbors
    // @return true if a mine was revealed (explosion), 0 otherwise
    bool revealTile(int row, int col);
    
    // Toggles flatg state: COVERED -> FLAGGED -> QUESTIONED -> COVERED
    // @returns The TileState after toggle
    TileState toggleTile(int row, int col);

    
    
    // Save game state to a stream
    // int save(ostream& in);

    // Load game state from a stream
    // int load(istream& out);

    private:
    // @return  true if (row,col) is within bounds of the board, false otherwise
    bool inBounds(int row, int col);

    // Randomly place mines and calculate adjacent mine counts
    void layMines();

    // Calculate adjacent mine counts for all tiles
    void calculateAdjacents();

    // Overload output operator for Board for debugging only
    // Shows all tiles regardless of state (e.g., covered tiles are shown)
    friend ostream& operator<<(ostream& out, const Board& board);   
};