// main.c
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <ncurses.h>
#include "minesweeper.h"

// Visuals
#define TILE_COVERED_CHAR '#'
#define TILE_MINE_CHAR '*'
#define TILE_EXPLODED_CHAR 'X'

// Color pair IDs
typedef enum
{
    CP_DEFAULT = 1, // white on black
    CP_CURSOR,      // yellow on black for cursor highlight
    CP_FLAG,        // red flag
    CP_QMARK,       // magenta ?
    // Number colors to mimic classic Minesweeper
    CP_NUM_1,    // dark blue
    CP_NUM_2,    // green
    CP_NUM_3,    // red
    CP_NUM_4,    // dark purple/indigo (magenta approximation)
    CP_NUM_5,    // maroon-ish (red dim)
    CP_NUM_6,    // cyan/turquoise
    CP_NUM_7,    // black (on white to keep visible)
    CP_NUM_8,    // gray (white dim)
    CP_BOOM_MSG, // red bold
    CP_MINE,     // yellow mine
} ColorPairID;

static int clamp(int v, int lo, int hi)
{
    return (v < lo) ? lo : (v > hi ? hi : v);
}

// Initialize color pairs
static void initColors(void)
{
    start_color();
    use_default_colors();
    // Define color pairs.  Color pairs are (foreground, background)
    init_pair(CP_DEFAULT, COLOR_WHITE, -1);
    init_pair(CP_CURSOR, COLOR_YELLOW, -1);
    init_pair(CP_FLAG, COLOR_RED, -1);
    init_pair(CP_QMARK, COLOR_MAGENTA, -1);

    init_pair(CP_NUM_1, COLOR_BLUE, -1);
    init_pair(CP_NUM_2, COLOR_GREEN, -1);
    init_pair(CP_NUM_3, COLOR_RED, -1);
    init_pair(CP_NUM_4, COLOR_MAGENTA, -1);
    init_pair(CP_NUM_5, COLOR_RED, -1);
    init_pair(CP_NUM_6, COLOR_CYAN, -1);
    init_pair(CP_NUM_7, COLOR_BLACK, COLOR_WHITE);
    init_pair(CP_NUM_8, COLOR_WHITE, -1);

    init_pair(CP_BOOM_MSG, COLOR_RED, -1);
    init_pair(CP_MINE, COLOR_YELLOW, -1);
}

// Returns color pair for given number of adjacent mines, and sets extra_attrs_out if non-NULL
static int colorForNumber(unsigned n, attr_t *extra_attrs_out)
{
    if (extra_attrs_out)
        *extra_attrs_out = A_NORMAL;
    switch (n)
    {
    case 1:
        return CP_NUM_1;
    case 2:
        return CP_NUM_2;
    case 3:
        return CP_NUM_3;
    case 4:
        return CP_NUM_4;
    case 5:
        if (extra_attrs_out)
            *extra_attrs_out = A_DIM;
        return CP_NUM_5;
    case 6:
        return CP_NUM_6;
    case 7:
        return CP_NUM_7;
    case 8:
        if (extra_attrs_out)
            *extra_attrs_out = A_DIM;
        return CP_NUM_8;
    default:
        return CP_DEFAULT;
    }
}

// Draw the board at given position, highlighting (cur_r, cur_c) if not game_over
static void drawBoard(Tile **board, int n, int cur_r, int cur_c,
                      int top, int left, int game_over,
                      int exploded_r, int exploded_c,
                      const char *status_msg)
{
    // Header / controls
    attron(COLOR_PAIR(CP_DEFAULT));
    mvprintw(top - 1, left,
             "Minesweeper  |  Arrows: move  F: flag  Space: reveal  S: save  R: restart  Q: quit");

    // Draw each tile
    for (int r = 0; r < n; ++r)
    {
        for (int c = 0; c < n; ++c)
        {
            int y = top + r;
            int x = left + c * 2;
            int is_cursor = (r == cur_r && c == cur_c);

            chtype ch = ' ';
            int pair = CP_DEFAULT;
            attr_t extra = A_NORMAL;

            Tile t = board[r][c];

            if (game_over)
            {
                if (r == exploded_r && c == exploded_c)
                {
                    pair = CP_BOOM_MSG;
                    extra = A_BOLD;
                    ch = TILE_EXPLODED_CHAR;
                }
                else if (t.isMine)
                {
                    pair = CP_MINE;
                    extra = A_BOLD;
                    ch = TILE_MINE_CHAR;
                }
                else if (t.adjacentMines > 0)
                {
                    pair = colorForNumber(t.adjacentMines, &extra);
                    ch = '0' + t.adjacentMines;
                }
                else
                {
                    ch = ' ';
                }
            }
            else
            {
                if (t.state == COVERED)
                {
                    pair = CP_DEFAULT;
                    ch = TILE_COVERED_CHAR;
                }
                else if (t.state == FLAGGED)
                {
                    pair = CP_FLAG;
                    extra = A_BOLD;
                    ch = 'F';
                }
                else if (t.state == QUESTIONED)
                {
                    pair = CP_QMARK;
                    ch = '?';
                }
                else if (t.state == REVEALED)
                {
                    if (t.isMine)
                    {
                        pair = CP_MINE;
                        extra = A_BOLD;
                        ch = TILE_MINE_CHAR;
                    }
                    else if (t.adjacentMines > 0)
                    {
                        pair = colorForNumber(t.adjacentMines, &extra);
                        ch = '0' + t.adjacentMines;
                    }
                    else
                    {
                        ch = ' ';
                    }
                }
                else if (t.state == EXPLODED)
                {
                    pair = CP_BOOM_MSG;
                    extra = A_BOLD;
                    ch = TILE_EXPLODED_CHAR;
                }
            }

            if (is_cursor && !game_over)
            {
                // If cursor is on a revealed empty, show yellow "black square" so it's visible
                if (t.state == REVEALED && !t.isMine && t.adjacentMines == 0)
                {
                    attron(COLOR_PAIR(CP_CURSOR) | A_BOLD);
                    mvaddstr(y, x, "■"); // Unicode black square (U+25A0)
                    attroff(COLOR_PAIR(CP_CURSOR) | A_BOLD);
                }
                else
                {
                    attron(COLOR_PAIR(CP_CURSOR) | A_BOLD);
                    mvaddch(y, x, ch);
                    attroff(COLOR_PAIR(CP_CURSOR) | A_BOLD);
                }
            }
            else
            {
                attron(COLOR_PAIR(pair) | extra);
                mvaddch(y, x, ch);
                attroff(COLOR_PAIR(pair) | extra);
            }
        }
    }

    // Status line / game over message
    if (game_over)
    {
        attron(COLOR_PAIR(CP_BOOM_MSG) | A_BOLD);
        mvprintw(top + n + 1, left, "BOOM! you are dead");
        attroff(COLOR_PAIR(CP_BOOM_MSG) | A_BOLD);
        mvprintw(top + n + 3, left, "Press R to restart or Q to quit.");
    }
    else if (status_msg && *status_msg)
    {
        mvprintw(top + n + 1, left, "%s", status_msg);
    }
}

// Start a new game, freeing old board if needed
static void startNewGame(Tile ***board_io, int n, int mines,
                         int *cur_r, int *cur_c,
                         int *game_over, int *exploded_r, int *exploded_c)
{
    if (*board_io)
        freeBoard(board_io, n);
    initBoard(board_io, n);
    layMines(*board_io, n, mines);
    *cur_r = 0;
    *cur_c = 0;
    *game_over = 0;
    *exploded_r = -1;
    *exploded_c = -1;
}

int main(int argc, char **argv)
{
    // CLI args: <board-size> <mine-count>
    int n = DEFAULT_BOARD_SIZE;
    int mines = DEFAULT_MINE_COUNT;
    if (argc >= 2)
        n = clamp(atoi(argv[1]), 2, 99);
    if (argc >= 3)
        mines = clamp(atoi(argv[2]), 1, n * n - 1);

    // Locale for UTF-8 box glyphs (■)
    setlocale(LC_ALL, "");

    // Game state
    Tile **board = NULL;
    int cur_r = 0, cur_c = 0;
    int game_over = 0;
    int exploded_r = -1, exploded_c = -1;

    startNewGame(&board, n, mines, &cur_r, &cur_c, &game_over, &exploded_r, &exploded_c);

    // ncurses init
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    if (has_colors())
        initColors();

    // Layout
    int top = 2;
    int left = 2;

    // Status message buffer
    char status[128] = {0};

    // First draw
    clear();
    drawBoard(board, n, cur_r, cur_c, top, left, game_over, exploded_r, exploded_c, status);
    refresh();

    // Input loop
    for (;;)
    {
        int ch = getch();
        if (ch == 'q' || ch == 'Q')
            break; // quit anytime

        if (!game_over)
        {
            switch (ch)
            {
            case KEY_UP:
                cur_r = (cur_r + n - 1) % n;
                status[0] = 0;
                break;
            case KEY_DOWN:
                cur_r = (cur_r + 1) % n;
                status[0] = 0;
                break;
            case KEY_LEFT:
                cur_c = (cur_c + n - 1) % n;
                status[0] = 0;
                break;
            case KEY_RIGHT:
                cur_c = (cur_c + 1) % n;
                status[0] = 0;
                break;

            case 'f':
            case 'F':
                toggleFlagAt(board, n, cur_r, cur_c);
                status[0] = 0;
                break;

            case ' ':
            {
                int exploded = revealAt(board, n, cur_r, cur_c);
                status[0] = 0;
                if (exploded)
                {
                    game_over = 1;
                    exploded_r = cur_r;
                    exploded_c = cur_c;
                    board[cur_r][cur_c].state = EXPLODED;
                }
                break;
            }

            case 's':
            case 'S':
            {
                int rc = saveGameToBinary("savegame.bin", board, n);
                if (rc == 0)
                {
                    snprintf(status, sizeof(status), "Saved to savegame.bin");
                }
                else
                {
                    snprintf(status, sizeof(status), "Save failed (code %d)", rc);
                }
                break;
            }

            case 'r':
            case 'R':
                startNewGame(&board, n, mines, &cur_r, &cur_c, &game_over, &exploded_r, &exploded_c);
                status[0] = 0;
                break;
            }
        }
        else
        {
            // After game over, allow R to restart, Q handled at top
            if (ch == 'r' || ch == 'R')
            {
                startNewGame(&board, n, mines, &cur_r, &cur_c, &game_over, &exploded_r, &exploded_c);
                status[0] = 0;
            }
            else if (ch == 's' || ch == 'S')
            {
                int rc = saveGameToBinary("savegame.bin", board, n);
                if (rc == 0)
                {
                    snprintf(status, sizeof(status), "Saved to savegame.bin");
                }
                else
                {
                    snprintf(status, sizeof(status), "Save failed (code %d)", rc);
                }
            }
        }

        clear();
        drawBoard(board, n, cur_r, cur_c, top, left, game_over, exploded_r, exploded_c, status);
        refresh();
    }

    endwin();
    freeBoard(&board, n);
    return 0;
}
