#include <clocale>
#include <ctime>
#include <cstdlib>
#include <ncursesw/ncurses.h>
#include "terminal.h"
#include "logic.h"



int main(){
    srand(time(0));
    setlocale(LC_ALL, "");

    initscr();
    noecho();
    keypad(stdscr, true);
    start_color();

    init_color(COLOR_GREEN, 400, 750, 202);
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);

    curs_set(0);

    draw_start_menu(0);

    getch();
    endwin();
}
