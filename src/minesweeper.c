#include <stdio.h>
#include <stdlib.h>
#include "minesweeper.h"
/**
 * A simple Minesweeper game implementation in C.
 * This code initializes a game board, allows for basic operations (eventually to be implemented),
 * and prints the board state to the console.
 * 
 * The primary objective of this code is to utilize all the features of the C language that we have learned so far.
 * @author Rodney Aiglstorfer
 */



// Creates the game board and initializes all tiles to COVERED state
// TODO: Add mine placement and adjacent mine counting
void initBoard(Tile ***board, int board_size) {
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
}

// Frees the allocated memory for the board
void freeBoard(Tile ***board, int board_size) {
    for (int i = 0; i < board_size; i++) {
        free((*board)[i]);
    }
    free(*board);
}
