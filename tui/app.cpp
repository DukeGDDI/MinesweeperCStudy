// tui/app.cpp
// Controls:
//   Arrows / H J K L  → move cursor
//   Space / Enter     → reveal
//   f                 → flag / cycle flag (uses Board::toggleTile)
//   m                 → chord (auto-reveal neighbors when flags match number)
//   r                 → restart same config
//   q                 → quit
//
// Run: ./ms_tui [rows] [cols] [mines]   (defaults 16 30 99)

#include <ncurses.h>
#include <cstdlib>
#include <string>
#include <algorithm>
#include "minesweeper.hpp"

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

struct Layout { int top=0,left=0,cellw=2; };
static Layout layout_for(int term_r,int term_c,int rows,int cols){
    Layout L;
    int w = cols * L.cellw + 2;
    int h = rows + 2;
    L.top  = 1;   // small margin from top (or 0 if you prefer flush)
    L.left = 1;   // small margin from left
    return L;
}

static bool is_flag(TileState s){ return s==FLAGGED || s==QUESTIONED; }

static bool check_win(Board& B){
    for(int r=0;r<B.getRows();++r)
        for(int c=0;c<B.getColumns();++c){
            Tile t=B.getTile(r,c);
            if(!t.isMine && t.state!=REVEALED) return false;
        }
    return true;
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

static void draw_board(Board& B, const Layout& L, const Cursor& cur, bool over, int boom_r, int boom_c) {
    const int R = B.getRows(), C = B.getColumns();
    draw_frame(L, R, C);

    for (int r = 0; r < R; ++r) {
        for (int c = 0; c < C; ++c) {
            const int y = L.top + 1 + r;
            const int x = L.left + 1 + c * L.cellw;
            const bool on = (r == cur.r && c == cur.c);

            const Tile t = B.getTile(r, c);

            // Highlight cursor on ANY tile (revealed or not) without clobbering colors.
            if (on) attron(A_REVERSE);

            if (t.state == COVERED || t.state == FLAGGED || t.state == QUESTIONED) {
                if (t.state == FLAGGED) {
                    attron(COLOR_PAIR(CP_FLAG));
                    mvprintw(y, x, "F ");
                    attroff(COLOR_PAIR(CP_FLAG));
                } else if (t.state == QUESTIONED) {
                    attron(A_DIM);
                    mvprintw(y, x, "? ");
                    attroff(A_DIM);
                } else {
                    mvprintw(y, x, "[]");
                }
            } else { // REVEALED / EXPLODED
                if (t.isMine) {
                    if (over && r == boom_r && c == boom_c) {
                        attron(COLOR_PAIR(CP_EXPLODE));
                        mvprintw(y, x, "* ");
                        attroff(COLOR_PAIR(CP_EXPLODE));
                    } else {
                        attron(COLOR_PAIR(CP_MINE));
                        mvprintw(y, x, "* ");
                        attroff(COLOR_PAIR(CP_MINE));
                    }
                } else if (t.adjacentMines == 0) {
                    mvprintw(y, x, "  ");
                } else {
                    const short cp = num_color((int)t.adjacentMines);
                    attron(COLOR_PAIR(cp) | A_BOLD);
                    mvprintw(y, x, "%u ", t.adjacentMines);
                    attroff(COLOR_PAIR(cp) | A_BOLD);
                }
            }

            if (on) attroff(A_REVERSE);
        }
    }
}
static void draw_status(const Config& cfg,bool over,bool win,int y,int x){
    move(y,x); clrtoeol();
    if(over){
        if(win){ attron(COLOR_PAIR(CP_WIN)|A_BOLD); mvprintw(y,x,"You win!  r=replay  q=quit"); attroff(COLOR_PAIR(CP_WIN)|A_BOLD); }
        else   { attron(COLOR_PAIR(CP_LOSE)|A_BOLD); mvprintw(y,x,"BOOM! You lost. r=replay  q=quit"); attroff(COLOR_PAIR(CP_LOSE)|A_BOLD); }
    }else{
        mvprintw(y,x,"Arrows/HJKL move • Space/Enter reveal • f flag • m chord • r restart • q quit");
    }
    mvprintw(std::max(0,y-1), x, "Minesweeper %dx%d (%d mines)", cfg.rows, cfg.cols, cfg.mines);
}

static void chord(Board& B,int r,int c,bool& over,bool& win,int& boom_r,int& boom_c){
    Tile t=B.getTile(r,c); if(t.state!=REVEALED) return;
    int need=t.adjacentMines, flags=0;
    for(int dr=-1;dr<=1;++dr)for(int dc=-1;dc<=1;++dc){
        if(!dr&&!dc) continue; int nr=r+dr,nc=c+dc;
        if(!B.inBounds(nr,nc)) continue;
        if(is_flag(B.getTile(nr,nc).state)) ++flags;
    }
    if(flags!=need) return;
    for(int dr=-1;dr<=1;++dr)for(int dc=-1;dc<=1;++dc){
        if(!dr&&!dc) continue; int nr=r+dr,nc=c+dc;
        if(!B.inBounds(nr,nc)) continue;
        if(B.getTile(nr,nc).state==COVERED){
            bool boom=B.revealTile(nr,nc);
            if(boom){ over=true; win=false; boom_r=nr; boom_c=nc; return; }
        }
    }
    if(check_win(B)){ over=true; win=true; }
}

int main(int argc,char** argv){
    Config cfg;
    if(argc==4){
        cfg.rows = std::max(5, std::atoi(argv[1]));
        cfg.cols = std::max(5, std::atoi(argv[2]));
        cfg.mines= std::max(1, std::atoi(argv[3]));
        cfg.mines= std::min(cfg.mines, std::max(1, cfg.rows*cfg.cols-1));
    }

    Board board(cfg.rows,cfg.cols,cfg.mines);
    Cursor cur{0,0};
    bool over=false, win=false; int boom_r=-1, boom_c=-1;

    initscr(); cbreak(); noecho(); keypad(stdscr, TRUE); curs_set(0);
    if(has_colors()) init_colors();

    bool running=true;
    while(running){
        int tr,tc; getmaxyx(stdscr,tr,tc);
        Layout L=layout_for(tr,tc,board.getRows(),board.getColumns());
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

            // chord
            case 'm':
                if(!over) chord(board,cur.r,cur.c,over,win,boom_r,boom_c);
                break;

            // restart
            case 'r':
                board = Board(cfg.rows,cfg.cols,cfg.mines);
                cur={0,0}; over=false; win=false; boom_r=boom_c=-1;
                break;

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