/* =============================================================                                       
 *   _____ _                                       
 *  |     |_|___ ___ ___ _ _ _ ___ ___ ___ ___ ___ 
 *  | | | | |   | -_|_ -| | | | -_| -_| . | -_|  _|
 *  |_|_|_|_|_|_|___|___|_____|___|___|  _|___|_|  
 *                                  |_|                
 *
 * =============================================================
 *
 * tui/app.cpp
 * Controls:
 *   Arrows / H J K L  → move cursor
 *   Space / Enter     → reveal
 *   f                 → flag / cycle flag (Board::toggleTile)
 *   r                 → restart same config
 *   s                 → save to the current save path
 *   q                 → quit
 *
 * Run:
 *   ./ms_tui                 (defaults: 16 30 99)
 *   ./ms_tui 10 20 40       (rows cols mines)
 *   ./ms_tui savefile.txt   (load from file)
 *
 * Link: -lncursesw (Linux) or -lncurses (macOS)
 * 
 * =============================================================
 */

#include <ncurses.h>
#include <locale.h>
#include <cstdlib>
#include <string>
#include <algorithm>
#include "board.hpp"
using namespace std;

struct Config { int rows=16, cols=30, mines=99; };
struct Cursor { int r=0, c=0; };

enum CP : short {
    CP_DEFAULT=1, CP_FRAME, CP_NUM1, CP_NUM2, CP_NUM3, CP_NUM4, CP_NUM5, CP_NUM6, CP_NUM7, CP_NUM8,
    CP_MINE, CP_FLAG, CP_EXPLODE, CP_WIN, CP_LOSE, CP_CURSOR
};

static void init_colors() {
    if (!has_colors()) return;
    start_color(); use_default_colors();
    init_pair(CP_DEFAULT,-1,-1);            init_pair(CP_FRAME,  COLOR_CYAN,-1);
    init_pair(CP_NUM1,  COLOR_BLUE,-1);     init_pair(CP_NUM2,   COLOR_GREEN,-1);
    init_pair(CP_NUM3,  COLOR_RED,-1);      init_pair(CP_NUM4,   COLOR_MAGENTA,-1);
    init_pair(CP_NUM5,  COLOR_YELLOW,-1);   init_pair(CP_NUM6,   COLOR_CYAN,-1);
    init_pair(CP_NUM7,  COLOR_WHITE,-1);    init_pair(CP_NUM8,   COLOR_BLACK,-1);
    init_pair(CP_MINE,  COLOR_RED,-1);      init_pair(CP_FLAG,   COLOR_YELLOW,-1);
    init_pair(CP_EXPLODE, COLOR_WHITE, COLOR_RED);
    init_pair(CP_WIN,   COLOR_GREEN,-1);    init_pair(CP_LOSE,   COLOR_RED,-1);
    init_pair(CP_CURSOR,COLOR_BLACK, COLOR_YELLOW);
}
static short num_color(int n){
    switch(n){case 1:return CP_NUM1;case 2:return CP_NUM2;case 3:return CP_NUM3;case 4:return CP_NUM4;
               case 5:return CP_NUM5;case 6:return CP_NUM6;case 7:return CP_NUM7;case 8:return CP_NUM8;
               default:return CP_DEFAULT;}
}

struct Layout { int top=1, left=1, cellw=2; }; // left/top aligned with small margin
static Layout layout_for_left(int /*term_r*/,int /*term_c*/,int /*rows*/,int /*cols*/) { return {}; }

static bool check_win(Board& B){
    for(int r=0;r<B.getRows();++r)
        for(int c=0;c<B.getColumns();++c){
            Tile t=B.getTile(r,c);
            if(!t.isMine && t.state!=REVEALED) return false;
        }
    return true;
}

static int count_mines(Board& B){
    int m=0;
    for(int r=0;r<B.getRows();++r)
        for(int c=0;c<B.getColumns();++c)
            if(B.getTile(r,c).isMine) ++m;
    return m;
}

static void draw_frame(const Layout& L,int R,int C){
    attron(COLOR_PAIR(CP_FRAME));
    mvaddch(L.top, L.left,'+'); mvhline(L.top, L.left+1,'-', C*L.cellw);
    mvaddch(L.top, L.left+1+C*L.cellw,'+');
    mvvline(L.top+1, L.left,'|', R);
    mvvline(L.top+1, L.left+1+C*L.cellw,'|', R);
    mvaddch(L.top+1+R, L.left,'+'); mvhline(L.top+1+R,L.left+1,'-', C*L.cellw);
    mvaddch(L.top+1+R, L.left+1+C*L.cellw,'+');
    attroff(COLOR_PAIR(CP_FRAME));
}

static void draw_board(Board& B,const Layout& L,const Cursor& cur,bool over,int boom_r,int boom_c){
    int R=B.getRows(), C=B.getColumns(); draw_frame(L,R,C);
    for(int r=0;r<R;++r)for(int c=0;c<C;++c){
        int y=L.top+1+r, x=L.left+1+c*L.cellw;
        Tile t=B.getTile(r,c);
        bool on=(r==cur.r && c==cur.c);

        // Always highlight the cursor (even on revealed cells)
        if(on) attron(A_REVERSE);

        if(t.state==COVERED || t.state==FLAGGED || t.state==QUESTIONED){
            if(t.state==FLAGGED){ attron(COLOR_PAIR(CP_FLAG)); mvprintw(y,x,"F "); attroff(COLOR_PAIR(CP_FLAG)); }
            else if(t.state==QUESTIONED){ attron(A_DIM); mvprintw(y,x,"? "); attroff(A_DIM); }
            else mvprintw(y,x,"[]");
        }else{ // REVEALED / EXPLODED
            if(t.isMine){
                if(over && r==boom_r && c==boom_c){ attron(COLOR_PAIR(CP_EXPLODE)); mvprintw(y,x,"* "); attroff(COLOR_PAIR(CP_EXPLODE)); }
                else { attron(COLOR_PAIR(CP_MINE)); mvprintw(y,x,"* "); attroff(COLOR_PAIR(CP_MINE)); }
            }else if(t.adjacentMines==0){
                mvprintw(y,x,"  ");
            }else{
                short cp=num_color((int)t.adjacentMines);
                attron(COLOR_PAIR(cp)|A_BOLD);
                mvprintw(y,x,"%u ",t.adjacentMines);
                attroff(COLOR_PAIR(cp)|A_BOLD);
            }
        }

        if(on) attroff(A_REVERSE);
    }
}

static void draw_status(const Config& cfg,bool over,bool win,int y,int x){
    move(y,x); clrtoeol();
    if(over){
        if(win){ attron(COLOR_PAIR(CP_WIN)|A_BOLD); mvprintw(y,x,"You win!  r=replay  s=save  q=quit"); attroff(COLOR_PAIR(CP_WIN)|A_BOLD); }
        else   { attron(COLOR_PAIR(CP_LOSE)|A_BOLD); mvprintw(y,x,"BOOM! You lost. r=replay  s=save  q=quit"); attroff(COLOR_PAIR(CP_LOSE)|A_BOLD); }
    }else{
        mvprintw(y,x,"Arrows/HJKL move | Space/Enter reveal | f flag | r restart | s save | q quit");
    }
    mvprintw(max(0,y-1), x, "Minesweeper %dx%d (%d mines)", cfg.rows, cfg.cols, cfg.mines);
}

int main(int argc,char** argv){
    setlocale(LC_ALL, ""); // enable UTF-8 safely

    Config cfg;               // defaults: 16x30, 99
    string save_path = "save_game.txt";
    Board board(cfg.rows, cfg.cols, cfg.mines); // will be replaced if we load
    Cursor cur{0,0};
    bool over=false, win=false; int boom_r=-1, boom_c=-1;

    // --- CLI parsing ---
    if(argc == 2){
        save_path = argv[1];
        ifstream ifs(save_path);
        if(board.load(ifs)){
            // infer config from the loaded board
            cfg.rows  = board.getRows();
            cfg.cols  = board.getColumns();
            cfg.mines = count_mines(board);
        } else {
            // if load fails, keep defaults but remember the save path
        }
    } else if(argc == 4){
        cfg.rows  = max(5, atoi(argv[1]));
        cfg.cols  = max(5, atoi(argv[2]));
        cfg.mines = max(1, atoi(argv[3]));
        cfg.mines = min(cfg.mines, max(1, cfg.rows * cfg.cols - 1));
        board = Board(cfg.rows, cfg.cols, cfg.mines);
    } else {
        // no args: defaults already set, board constructed above
    }

    // --- ncurses init ---
    initscr(); cbreak(); noecho(); keypad(stdscr, TRUE); curs_set(0);
    if(has_colors()) init_colors();

    bool running=true;
    while(running){
        int tr,tc; getmaxyx(stdscr,tr,tc);
        Layout L = layout_for_left(tr,tc,board.getRows(),board.getColumns());
        if(!board.inBounds(cur.r,cur.c)) cur={0,0};

        clear();
        draw_board(board,L,cur,over,boom_r,boom_c);
        draw_status(cfg,over,win, L.top+2+board.getRows(), L.left);
        refresh();

        int ch=getch();
        switch(ch){
            // movement
            case KEY_UP: case 'k': if(cur.r>0) --cur.r; break;
            case KEY_DOWN: case 'j': if(cur.r+1<board.getRows()) ++cur.r; break;
            case KEY_LEFT: case 'h': if(cur.c>0) --cur.c; break;
            case KEY_RIGHT: case 'l': if(cur.c+1<board.getColumns()) ++cur.c; break;

            // reveal
            case ' ': case '\n':
                if(!over){
                    bool boom=board.revealTile(cur.r,cur.c);
                    if(boom){ over=true; win=false; boom_r=cur.r; boom_c=cur.c; }
                    else if(check_win(board)){ over=true; win=true; }
                } break;

            // flag
            case 'f':
                if(!over) board.toggleTile(cur.r,cur.c);
                break;

            // restart
            case 'r':
                //board = Board(cfg.rows,cfg.cols,cfg.mines);
                board.reset(cfg.rows,cfg.cols,cfg.mines);
                cur={0,0}; over=false; win=false; boom_r=boom_c=-1;
                break;

            // save
            case 's': {
                ofstream ofs(save_path);
                bool ok = board.save(ofs);   // uses your Board::save(...)
                int y = L.top+3+board.getRows();
                move(y, L.left); clrtoeol();
                mvprintw(y, L.left, ok ? "Saved to %s" : "Save failed: %s", save_path.c_str());
                refresh(); napms(500);
            } break;

            case 'q': running=false; break;
#ifdef KEY_RESIZE
            case KEY_RESIZE: break;
#endif
            default: break;
        }
    }

    endwin();
    return 0;
}