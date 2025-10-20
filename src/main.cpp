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

int main(int argc, char** argv) {
    istringstream iss(kTestBoard);
    Board board(iss);

    cout << "Initial Board:\n";
    cout << board << "\n";

    Board board2(10, 10, 15);
    cout << "Random Board:\n";
    cout << board2 << "\n";
}
