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
void initBoard(Tile ***board, int board_size);

// Frees the allocated memory for the board
void freeBoard(Tile ***board, int board_size);