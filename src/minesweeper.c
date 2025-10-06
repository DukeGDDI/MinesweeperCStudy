#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "minesweeper.h"
/**
 * A simple Minesweeper game implementation in C.
 * This code initializes a game board, allows for basic operations (eventually to be implemented),
 * and prints the board state to the console.
 * 
 * The primary objective of this code is to utilize all the features of the C language that we have learned so far.
 * @author Rodney Aiglstorfer
 */

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
// TODO: Add mine placement and adjacent mine counting
void initBoard(Tile ***board, int board_size, int mine_count) {
    size_t bytes = board_size * sizeof(Tile *); // refactored to use size_t
    *board = malloc(bytes);
    for (int i = 0; i < board_size; i++) {
        bytes = board_size * sizeof(Tile); // refactored to use size_t
        (*board)[i] = malloc(bytes);
    }

    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            Tile tile = (*board)[i][j];
            tile.state = COVERED;
            tile.isMine = 0;
            tile.adjacentMines = 0;
        }
    }

    layMines(*board, board_size, mine_count);
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

