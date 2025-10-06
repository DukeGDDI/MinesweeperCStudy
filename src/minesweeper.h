/**
 * A simple Minesweeper game implementation in C.
 * This code initializes a game board, allows for basic operations (eventually to be implemented),
 * and prints the board state to the console.
 * 
 * The primary objective of this code is to utilize all the features of the C language that we have learned so far.
 * @author Rodney Aiglstorfer
 */
#define DEFAULT_BOARD_SIZE 10
#define DEFAULT_MINE_COUNT 4

// Define the tile states
typedef enum _tile_state {
    COVERED,
    REVEALED,
    FLAGGED,
    QUESTIONED,
    EXPLODED
} TileState;

// Define the tile structure
typedef struct _tile {
    TileState state;
    int isMine;
    int adjacentMines;
} Tile;

// Creates the game board and initializes all tiles to COVERED state
// TODO: Add mine placement and adjacent mine counting
void initBoard(Tile ***board, int board_size, int mine_count);

// Frees the allocated memory for the board
void freeBoard(Tile ***board, int board_size);

// Reveal logic:
// - If tile is FLAGGED/QUESTIONED/REVEALED: do nothing
// - If tile is a mine: returns 1 to indicate explosion (the caller can handle game over
//   (NOTE: function is also used for auto-spread after a safe click).
// - If tile has adjacentMines > 0: reveal it and stop
// - If tile has adjacentMines == 0: reveal it and recursively reveal neighbors
// @return 1 if a mine was revealed (explosion), 0 otherwise
int revealAt(Tile **board, int n, int r, int c);

// @return 1 if (r,c) is within bounds of an n x n board, 0 otherwise
int inBounds(int n, int r, int c);

// Toggles flatg state: COVERED -> FLAGGED -> QUESTIONED -> COVERED
void toggleFlagAt(Tile **board, int n, int r, int c);