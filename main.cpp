#include <clocale>
#include <ctime>
#include <cstdlib>
#include <ncursesw/ncurses.h>
#include "terminal.h"
#include "logic.h"



int main(){
    srand(time(0));
    resize_term(5, 5);
    setlocale(LC_ALL, "");

    initscr();
    noecho();
    keypad(stdscr, true);
    start_color();
    curs_set(0);

    draw_start_menu();

    getch();
    endwin();
}
