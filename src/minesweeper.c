#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "minesweeper.h"
/**
 * A simple Minesweeper game implementation in C.
 * This code initializes a game board, allows for basic operations (eventually to be implemented),
 * and prints the board state to the console.
 * 
 * The primary objective of this code is to utilize all the features of the C language that we have learned so far.
 * @author Rodney Aiglstorfer
 */

// Randomly place mines and calculate adjacent mine counts
void layMines(Tile **board, int board_size, int mine_count) {
    // Randomly place mines and calculate adjacent mine counts
    srand(time(NULL));
    int placed = 0;
    while (placed < mine_count) {
        int r = rand() % board_size;
        int c = rand() % board_size;
        if (!board[r][c].isMine) {
            board[r][c].isMine = 1;
            placed++;
            // Update adjacent mine counts
            for (int dr = -1; dr <= 1; dr++) {
                for (int dc = -1; dc <= 1; dc++) {
                    if (inBounds(board_size, r + dr, c + dc)) {
                        board[r + dr][c + dc].adjacentMines++;
                    }
                }
            }
        }
    }
}

// Creates the game board and initializes all tiles to COVERED state
// and no mines (you can add mine placement by calling layMines or 
// loading from a file).
//
// Mental model: 2D array as array of pointers
// +----------------+-----------+-------------------------------+
// | Expression     | Type      | Meaning                       |
// +----------------+-----------+-------------------------------+
// | board          | Tile ***  | pointer to 2D array pointer   |
// | *board         | Tile **   | the 2D array itself           |
// | (*board)[i]    | Tile *    | the i-th row                  |
// | (*board)[i][j] | Tile      | the tile at (i,j)             |
// +----------------+-----------+-------------------------------+
void initBoard(Tile ***board, int board_size) {
    // Allocate memory for the rows (array of pointers)
    // This allocates an array of Tile *, i.e., rows of your 2D grid.
    size_t bytes = board_size * sizeof(Tile *); // refactored to use size_t
    *board = malloc(bytes);
    // Allocate memory for each row (array of Tile structs)
    // Each (*board)[i] is a pointer to an array of Tile structs (the columns).
    for (int i = 0; i < board_size; i++) {
        bytes = board_size * sizeof(Tile); // refactored to use size_t
        (*board)[i] = malloc(bytes);
    }
    // Initialize all tiles
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            // Aproach 0: Direct field assignments (verbose)
            // (*board)[i][j].state = COVERED;
            // (*board)[i][j].isMine = 0;
            // (*board)[i][j].adjacentMines = 0;

            // Approach 1: Direct using -> operator (verbose)
            // Tile *t = &(*board)[i][j];
            // t->state = COVERED;
            // t->isMine = 0;
            // t->adjacentMines = 0;    

            // Approach 2: Direct assignment (commented out)
            // Tile tile = (*board)[i][j];
            // tile.state = COVERED;
            // tile.isMine = 0;
            // tile.adjacentMines = 0;
            
            // Approach 3: Temporary variable (clearer for beginners)
            // Tile t = {COVERED, 0, 0}; // C99 struct literal syntax
            // (*board)[i][j] = t;

            // Approach 4: Struct literal (preferred)
            (*board)[i][j] = (Tile){COVERED, 0, 0}; // C99 struct literal syntax
        }
    }
}

// Toggles flag state: COVERED -> FLAGGED -> QUESTIONED -> COVERED
void toggleFlagAt(Tile **board, int n, int r, int c) {
    assert(!inBounds(n, r, c));

    Tile *t = &board[r][c];
    if (t->state == COVERED) {
        t->state = FLAGGED;
    } else if (t->state == FLAGGED) {
        t->state = QUESTIONED;
    } else if (t->state == QUESTIONED) {
        t->state = COVERED;
    }
}

// Frees the allocated memory for the board
void freeBoard(Tile ***board, int board_size) {
    for (int i = 0; i < board_size; i++) {
        free((*board)[i]);
    }
    free(*board);
}

// Returns 1 if inside bounds, 0 otherwise
int inBounds(int n, int r, int c) {
    return r >= 0 && r < n && c >= 0 && c < n;
}

// Reveal logic:
// - If tile is FLAGGED/QUESTIONED/REVEALED: do nothing
// - If tile is a mine: mark EXPLODED (you can choose to early-return without exploding if this
//   function is also used for auto-spread after a safe click).
// - If tile has adjacentMines > 0: reveal it and stop
// - If tile has adjacentMines == 0: reveal it and recursively reveal neighbors
int revealAt(Tile **board, int n, int r, int c) {
    if (!inBounds(n, r, c)) return 0;

    Tile *t = &board[r][c];

    // Already visible or intentionally hidden by player
    if (t->state == REVEALED || t->state == FLAGGED || t->state == QUESTIONED) return 0;

    // Clicked a mine → explode (alternatively, return without changing if using separate "click" handler)
    if (t->isMine) {
        t->state = EXPLODED;
        return 1;
    }

    // Reveal this tile
    t->state = REVEALED;

    // If it's a number (>0), stop here
    if (t->adjacentMines > 0) return 1;

    // Otherwise (0 adjacent), flood-reveal neighbors
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            int nr = r + dr, nc = c + dc;
            if (inBounds(n, nr, nc)) {
                revealAt(board, n, nr, nc); // ignore return value for auto-spread
            }
        }
    }

    return 0;
}

// Save game state as binary file
int saveGameToBinary(const char *filename, Tile **board, int board_size) {
    FILE *file = fopen(filename, "wb");
    if (!file) return -1;

    // Write board size
    if (fwrite(&board_size, sizeof(int), 1, file) != 1) {
        fclose(file);
        return -1;
    }

    // Write each tile
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            Tile *t = &board[i][j];
            if (fwrite(t, sizeof(Tile), 1, file) != 1) {
                fclose(file);
                return -1;
            }
        }
    }

    fclose(file);
    return 0;
}

// Load game state from binary file
int loadGameFromBinary(const char *filename, Tile ***board, int *board_size) {
    FILE *file = fopen(filename, "rb");
    if (!file) return -1;

    // Read board size
    if (fread(board_size, sizeof(int), 1, file) != 1) {
        fclose(file);
        return -1;
    }

    // Read each tile
    for (int i = 0; i < *board_size; i++) {
        for (int j = 0; j < *board_size; j++) {
            Tile *t = &(*board)[i][j];
            if (fread(t, sizeof(Tile), 1, file) != 1) {
                fclose(file);
                return -1;
            }
        }
    }

    fclose(file);
    return 0;
}

// Save game state as text file that is human-readable (eg. # # F ? ...)
int saveGameToText(const char *filename, Tile **board, int board_size) {
    FILE *file = fopen(filename, "w");
    if (!file) return -1;

    // Write board size
    if (fprintf(file, "%d\n", board_size) < 0) {
        fclose(file);
        return -1;
    }

    // Write each tile
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            Tile *t = &board[i][j];
            if (fprintf(file, "%d %d %d\n", t->state, t->isMine, t->adjacentMines) < 0) {
                fclose(file);
                return -1;
            }
        }
    }

    fclose(file);
    return 0;
}

// Load game state from text file
int loadGameFromText(const char *filename, Tile ***board, int *board_size) {
    FILE *file = fopen(filename, "r");
    if (!file) return -1;

    // Read board size
    if (fscanf(file, "%d", board_size) != 1) {
        fclose(file);
        return -1;
    }

    // Read each tile
    for (int i = 0; i < *board_size; i++) {
        for (int j = 0; j < *board_size; j++) {
            Tile *t = &(*board)[i][j];
            if (fscanf(file, "%d %d %d", (int *)&t->state, &t->isMine, &t->adjacentMines) != 3) {
                fclose(file);
                return -1;
            }
        }
    }

    fclose(file);
    return 0;
}