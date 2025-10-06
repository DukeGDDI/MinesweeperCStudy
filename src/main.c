#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minesweeper.h"

// Prints the current state of the board to the console
void printBoard(Tile **board, int board_size){
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            switch (board[i][j].state)
            {
                case COVERED:
                    printf("# ");
                    break;
                case REVEALED:
                    if (board[i][j].isMine) {
                        printf("* ");
                    } else {
                        int adjacent = board[i][j].adjacentMines;
                        if (adjacent == 0) {
                            printf(". ");
                        } else {
                            printf("%d ", adjacent);
                        }
                    }
                    break;
                case FLAGGED:
                    printf("F ");
                    break;
                case QUESTIONED:
                    printf("? ");
                    break;
                case EXPLODED:
                    printf("X ");
                    break;
            }
        }
        printf("\n");
    }
    printf("\n");
}

// Print the board with all cells revealed (for debugging)
void printFullBoard(Tile **board, int board_size) {
    for (int i = 0; i < board_size; i++) {
        for (int j = 0; j < board_size; j++) {
            if (board[i][j].isMine) {
                printf("* ");
            } else {
                printf("%d ", board[i][j].adjacentMines);
            }
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    // Refactored to use const char*
    const char *usage = "Usage: minesweeper [board_size mine_count]\n"
                        "  board_size: Size of the board (default 10)\n"
                        "  mine_count: Number of mines (default 4)\n";

    if (argc > 3) {
        fprintf(stderr, "%s", usage);
        return 1;
    }

    // If 1 arg and it is "--help" or "-h", print usage
    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)) {
        printf("%s", usage);
        return 0;
    }

    int board_size = DEFAULT_BOARD_SIZE; 
    int mine_count = DEFAULT_MINE_COUNT;
    if (argc == 3) {
        board_size = atoi(argv[1]);
        mine_count = atoi(argv[2]);
    }

    printf("Creating a %dx%d Minesweeper board with %d mines.\n", board_size, board_size, mine_count);

    // Initialize the board
    Tile **board;
    initBoard(&board, board_size);
    layMines(board, board_size, mine_count);

    // Print the initial board
    printFullBoard(board, board_size);

    // Example reveal at (0,0)
    int exploded = revealAt(board, board_size, 0, 0); 

    if (exploded) {
        printf("Boom! You hit a mine!\n");
        freeBoard(&board, board_size); // Free memory before exiting
        return 1;
    } else {
        printf("Board after reveal:\n");
        printBoard(board, board_size);
    }


    if (saveGameToBinary("savegame.bin", board, board_size) == 1) {
        fprintf(stderr, "Failed to save game to binary file.\n");
        freeBoard(&board, board_size);
        return 1;
    }
    if (saveGameToText("savegame.txt", board, board_size) == 1) {
        fprintf(stderr, "Failed to save game to text file.\n");
        freeBoard(&board, board_size);
        return 1;
    }
    // Free the allocated memory
    freeBoard(&board, board_size);

    initBoard(&board, board_size);
    if (loadGameFromBinary("savegame.bin", &board, &board_size) == -1) {
        fprintf(stderr, "Failed to load game from binary file.\n");
        freeBoard(&board, board_size);
        return 1;
    }
    printf("Loaded game from binary file:\n");
    printBoard(board, board_size);
    freeBoard(&board, board_size);

    initBoard(&board, board_size);
    if (loadGameFromText("savegame.txt", &board, &board_size) == -1) {
        fprintf(stderr, "Failed to load game from text file.\n");
        freeBoard(&board, board_size);
        return 1;
    }
    printf("Loaded game from text file:\n");
    printBoard(board, board_size);
    freeBoard(&board, board_size);  
    return 0;
}