#include "minesweeper.hpp"
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;


static const string kTestBoard = R"(
5 6 4
. . * . . .
. * . . . .
. . . * . .
. . . . . .
. . . . . *
)";

void printBoardStates(Board& board) {
    for (int r = 0; r < board.getRows(); r++) {
        for (int c = 0; c < board.getColumns(); c++) {
            Tile tile = board.getTile(r, c);
            switch (tile.state) {
                case COVERED:
                    cout << "# ";
                    break;
                case REVEALED:
                    cout << tile.adjacentMines << " ";
                    break;
                case FLAGGED:
                    cout << "F ";
                    break;
                case QUESTIONED:
                    cout << "? ";
                    break;
                case EXPLODED:
                    cout << "* ";
                    break;
            }
        }
        cout << "\n";
    }
}


int main(int argc, char** argv) {
    istringstream iss(kTestBoard);
    Board board(iss);

    cout << "Initial Board:\n";
    cout << board << "\n";

    Board board2(10, 10, 15);
    cout << "Random Board:\n";
    cout << board2 << "\n";

    // Save board2 to file
    ofstream ofs("saved_game.txt");
    board2.save(ofs);
    ofs.close();

    // Load board2 from file
    Board board3;
    ifstream ifs("saved_game.txt");
    board3.load(ifs);
    ifs.close();

    cout << "Loaded Board:\n";
    cout << board3 << "\n";

    // Check equality
    if (board2 == board3) {
        cout << "Loaded board matches saved board.\n";
    } else {
        cout << "Loaded board does NOT match saved board.\n";
    }

    // Test reveal
    int result = board.revealTile(0, 0);
    cout << "Revealing (0,0):\n";

    // test state of revealed tile
    Tile tile = board.getTile(0, 0);
    cout << "Tile (0,0) state: " << tile.state << "\n";

    // test state of adjacent tile
    tile = board.getTile(0, 1);
    cout << "Tile (0,1) state: " << tile.state << "\n";
    tile = board.getTile(1, 0);
    cout << "Tile (1,0) state: " << tile.state << "\n";
    tile = board.getTile(1, 1);
    cout << "Tile (1,1) state: " << tile.state << "\n";

    printBoardStates(board);

    // Reveal a mine at (0,5)
    cout << "Revealing (0,5):\n";
    result = board.revealTile(0, 5);
    printBoardStates(board);

    result = board.revealTile(0, 2);
    cout << "Revealing (0,2):\n";
    printBoardStates(board);

    if (result == 1) {
        cout << "Mine exploded at (0,2)!\n";
    } else {
        cout << "No mine at (0,2).\n";
    }   
    
    return 0;
}
