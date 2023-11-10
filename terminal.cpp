#include <iostream>
#include <vector>
#include <map>
#include <thread>

#include "terminal.h"
#include "ncursesw/ncurses.h"
#include "parse.h"
#include "logic.h"


WINDOW *game_win;
WINDOW *pred_win;
WINDOW *herb_win;
WINDOW *menu;
WINDOW *data_win;

using namespace std;

vector<vector<string>> screen;

void draw_start_menu(int cursor = 0) {
    clear();

    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    paint_logo();

    menu = newwin(6, 24, 5 * yMax / 7, xMax / 2 - 12);
    box(menu, 0, 0);
    keypad(menu, true);

    vector<string> menubar = {
            "Generate new world",
            "World settings",
            "HotKeys",
            "Quit",
    };

    for (int i = 0; i < menubar.size(); ++i) {
        mvwprintw(menu, i + 1, 12 - menubar[i].size() / 2, menubar[i].c_str());
    }

    refresh();

    wattron(menu, A_STANDOUT);
    mvwprintw(menu, cursor + 1, 12 - menubar[cursor].size() / 2, menubar[cursor].c_str());
    wattroff(menu, A_STANDOUT);

    for (;;) {
        int c = wgetch(menu);
        if (c == KEY_UP || c == 'k') cursor--;
        else if (c == KEY_DOWN || c == 'j') cursor++;
        else if (c == 10) {
            if (cursor == 0) {
                if (check()) {
                    start_game();
                } else {
                    start_parse();
                }
            }
            if (cursor == 1) start_parse();
            if (cursor == 3) endwin();
            break;
        } else if (c == 27 || c == 'x') {
            endwin();
            break;
        };

        if (cursor <= -1) {
            cursor = menubar.size() - 1;
        } else if (cursor >= menubar.size()) {
            cursor = 0;
        }

        // Рисуем выделения
        wattron(menu, A_STANDOUT);
        mvwprintw(menu, cursor + 1, 12 - menubar[cursor].size() / 2, menubar[cursor].c_str());
        wattroff(menu, A_STANDOUT);

        for (int i = 0; i < menubar.size(); ++i) {
            if (cursor != i)
                mvwprintw(menu, i + 1, 12 - menubar[i].size() / 2, menubar[i].c_str());
        }
    }
}


void paint_logo() {
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    vector<string> logo = {
            "                  _   _       _     _   _    __                 ",
            "      __      __ (_) | |   __| |   | | (_)  / _|   ___          ",
            "      \\ \\ /\\ / / | | | |  / _` |   | | | | | |_   / _ \\         ",
            "       \\ V  V /  | | | | | (_| |   | | | | |  _| |  __/         ",
            "        \\_/\\_/   |_| |_|  \\__,_|   |_| |_| |_|    \\___|         ",
            "                                                                ",
            "        _                       _           _                   ",
            "  ___  (_)  _ __ ___    _   _  | |   __ _  | |_    ___    _ __  ",
            " / __| | | | '_ ` _ \\  | | | | | |  / _` | | __|  / _ \\  | '__| ",
            " \\__ \\ | | | | | | | | | |_| | | | | (_| | | |_  | (_) | | |    ",
            " |___/ |_| |_| |_| |_|  \\__,_| |_|  \\__,_|  \\__|  \\___/  |_|    ",
    };

    for (int i = 0; i < logo.size(); i++) {
        mvprintw(yMax / 15 + i, xMax / 2 - logo[0].size() / 2, logo[i].c_str());
    }
};


void start_game() {
    clear();

    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    int startx = 8 * xMax / 10;

    game_win = newwin(yMax, startx - (startx % 2), 0, 0);
    box(game_win, 0, 0);
    wrefresh(game_win);

    data_win = newwin(4, xMax - startx + (startx % 2), 0, startx);
    box(data_win, 0, 0);
    wrefresh(data_win);

    pred_win = newwin((yMax - 4) / 2, xMax - startx, 4, startx);
    box(pred_win, 0, 0);
    wrefresh(pred_win);

    herb_win = newwin(yMax - (yMax - 4) / 2 - 4, xMax - startx, (yMax - 4) / 2 + 4, startx);
    box(herb_win, 0, 0);
    wrefresh(herb_win);

    fill_screen_empty();

    set_start_objects(game_win);
    wrefresh(game_win);


    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    for (;;) {
        life_tick();

//        std::this_thread::sleep_for(std::chrono::milliseconds(500));


        wgetch(game_win);
    }

    wgetch(game_win);
}

bool screen_place_is_empty(int y, int x) {
    return screen[y][x] == "  ";
}

void fill_screen_empty() {
    screen.clear();

    wclear(game_win);
    box(game_win, 0, 0);

    wclear(pred_win);
    box(pred_win, 0, 0);

    int yMax, xMax;
    getmaxyx(game_win, yMax, xMax);

    for (int y = 0; y < yMax; y++) {
        vector<string> new_line;
        for (int x = 0; x < xMax / 2; x++) {
            new_line.push_back("  ");
        }
        screen.push_back(new_line);
    }
}

void update_objects(map<string, vector<struct object>> objs) {
    fill_screen_empty();
    for (const auto &obj: objs["nature"])
        mvwprintw(game_win, obj.y, obj.x * 2 + 1, obj.emodji.c_str());
    for (const auto &obj: objs["tombstone"])
        mvwprintw(game_win, obj.y, obj.x * 2 + 1, obj.emodji.c_str());
    for (const auto &obj: objs["herb"])
        mvwprintw(game_win, obj.y, obj.x * 2 + 1, obj.emodji.c_str());
    for (const auto &obj: objs["pred"])
        mvwprintw(game_win, obj.y, obj.x * 2 + 1, obj.emodji.c_str());
}


void update_last_object(WINDOW *win, struct object obj) {
    screen[obj.y][obj.x] = obj.emodji;
}

WINDOW *get_window(string name) {
    if (name == "game_win") {
        return game_win;
    } else if (name == "pred_win") {
        return pred_win;
    } else if (name == "herb_win") {
        return herb_win;
    } else if (name == "data_win") {
        return data_win;
    }
    return NULL;
}