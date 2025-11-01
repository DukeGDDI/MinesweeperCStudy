/*                                       
 *   _____ _                                       
 *  |     |_|___ ___ ___ _ _ _ ___ ___ ___ ___ ___ 
 *  | | | | |   | -_|_ -| | | | -_| -_| . | -_|  _|
 *  |_|_|_|_|_|_|___|___|_____|___|___|  _|___|_|  
 *                                  |_|          
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include "tile_state.hpp"
#include "tile.hpp"

using namespace std;

#ifndef BOARD
#define BOARD
// Forward-declare Board so the interface can reference it
class Board;

// Serializer interface: DI target
struct ISerializable {
    virtual ~ISerializable() = default;

    // Save/load the Board through the interface (Board delegates to this)
    virtual int save(const Board& board, std::ostream& out) = 0;
    virtual int load(Board& board, std::istream& in) = 0;
};

class Board {
    public:
        // Empty Constructor
        Board();

        Board(int rows, int columns, int mines);

        // Main Constructor
        Board(int rows, int columns, int mines, std::shared_ptr<ISerializable> serializer);

        // Create Board from a stream (file).  This not the same as restoring a game 
        // from a file (see load() method).  This is used to create repeatable starting
        // boards that make testing simpler.
        Board(istream& in);

        // @return number of rows
        int getRows() const;

        // @return number of columns
        int getColumns() const;

        // @return number of mines
        int getMines() const;

        // @return tag state for tile at (row,col)
        const Tile& getTile(int row, int col) const;

        // Reveal logic:
        // - If tile is FLAGGED/QUESTIONED/REVEALED: do nothing
        // - If tile is a mine: returns 1 to indicate explosion (the caller can handle game over
        //   (NOTE: function is also used for auto-spread after a safe click).
        // - If tile has adjacentMines > 0: reveal it and stop
        // - If tile has adjacentMines == 0: reveal it and recursively reveal neighbors
        // @return true if a mine was revealed (explosion), 0 otherwise
        bool revealTile(int row, int col);

        // Toggles tile state: COVERED -> FLAGGED -> QUESTIONED -> COVERED
        // @return The TileState after toggle
        TileState toggleTile(int row, int col);
        
        // Save game state to a stream
        int save(ostream& in);

        // Load game state from a stream
        int load(istream& out);
    
        // @return  true if (row,col) is within bounds of the board, false otherwise
        bool inBounds(int row, int col) const;
    
        // Reset the board to initial state (with mines laid out)
        void reset(int rows, int cols, int mines);

        // Overload output operator for Board for debugging only
        // Shows all tiles regardless of state (e.g., covered tiles are shown)
        friend ostream& operator<<(ostream& out, const Board& board);
        
        // Overload the equality operator for testing purposes
        friend bool operator==(const Board& b1, const Board& b2);

    
    private:
        int rows;
        int columns;
        int mines;

        vector<vector<Tile>> tiles;

        // Injected dependency (shared_ptr lets you reuse a stateless singleton)
        std::shared_ptr<ISerializable> serializer;

        // Randomly place mines and calculate adjacent mine counts
        void layMines();

        // Calculate adjacent mine counts for all tiles
        void calculateAdjacents();
};

#endif // BOARD