#include <ncursesw/ncurses.h>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <cmath>
#include "parse.h"
#include "terminal.h"

WINDOW *settings_win;

using namespace std;

vector<int> SETTINGS = {0, 0, 0, 0};

vector<string> NAMES = {
        "Percentage of predators    ",
        "Percentage of herbivores   ",
        "Percentage of grass        ",
        "Percentage of grass respawn",
};

vector<string> menubar;

vector<int> get_settings(){
    update_file_values();
    return SETTINGS;
}
void start_parse() {
    parse_from_file();

    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    int Y = 6;
    int X = 2 * Y;
    settings_win = newwin(yMax - 2 * Y, xMax - 4 * X, Y, 2 * X);
    keypad(settings_win, TRUE);
    box(settings_win, 0, 0);

    change_NAMES_size_for_this_window();

    wrefresh(settings_win);

    for (int i = 0; i < NAMES.size(); i++) {
        if (to_string(SETTINGS[i]).size() == 1) {
            menubar.push_back(NAMES[i] + to_string(SETTINGS[i]) + " ");
        } else {
            menubar.push_back(NAMES[i] + to_string(SETTINGS[i]));
        }
    }

    int yMaxS, xMaxS;
    getmaxyx(settings_win, yMaxS, xMaxS);

    int cursor = -1;
    update_menu(cursor);

    init_color(COLOR_GREEN, 400, 750, 202);
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);


    if (!check()) print_warning();

    for (;;) {
        int c = wgetch(settings_win);
        if (c == KEY_UP || c == 'k') cursor--;
        else if (c == KEY_DOWN || c == 'j') cursor++;
        else if (c == 10) {
            print_input();

            menubar[cursor] = NAMES[cursor] + "0 ";
            update_menu(cursor);

            wrefresh(settings_win);
            string new_value = "";

            bool flag_1 = false;
            bool flag_2 = false;
            string c1, c2;
            while (!flag_1) {
                int gt = wgetch(settings_win);
                if (gt == 10) {
                    flag_1 = true;
                    flag_2 = true;
                    new_value = "0 ";
                    break;
                }
                c1 = is_digit(gt);
                if (c1.size() > 0) {
                    menubar[cursor] = NAMES[cursor] + c1 + " ";
                    update_menu(cursor);
                    flag_1 = true;
                    new_value += c1;
                }
            }

            bool flag_0 = true;
            while (!flag_2) {
                int gt = wgetch(settings_win);

                if (gt == 10) {
                    flag_2 = true;
                    new_value += " ";
                }

                c2 = is_digit(gt);
                if (c2.size() > 0 && flag_0) {
                    menubar[cursor] = NAMES[cursor] + c1 + c2;
                    flag_0 = false;
                    new_value += c2;
                    update_menu(cursor);
                }
            }

            SETTINGS[cursor] = stoi(new_value);
            update_file_values();
            wclear(settings_win);
            box(settings_win, 0, 0);
        } else if (c == 27 || c == 'x') {
            if (check()) {
                delwin(settings_win);
                draw_start_menu(1);
                break;
            } else {
                print_warning();
            }
        };

        if (cursor <= -1) {
            cursor = menubar.size() - 1;
        } else if (cursor >= menubar.size()) {
            cursor = 0;
        }

        update_menu(cursor);

        wrefresh(settings_win);
    }
}


void parse_from_file() {
    string line;
    ifstream out("settings.txt");
    if (out.is_open()) {
        for (int i = 0; i < NAMES.size(); i++) {
            getline(out, line);
            int value = stoi(line);
            SETTINGS[i] = value;
        }
    }
    out.close();
}

void update_file_values() {
    ofstream in("settings.txt");
    for (int i = 0; i < SETTINGS.size(); i++) {
        in << SETTINGS[i] << endl;
    }
    in.close();
}

void change_NAMES_size_for_this_window() {
    int yMax, xMax;
    getmaxyx(settings_win, yMax, xMax);

    string slice = "";

    for (int i = NAMES[3].size() + 6; i < xMax; i++) {

        slice += " ";
    }

    for (int i = 0; i < NAMES.size(); i++) {
        NAMES[i] += slice;
    }
}

string is_digit(int c) {
    if (c == '0') return "0";
    else if (c == '1') return "1";
    else if (c == '2') return "2";
    else if (c == '3') return "3";
    else if (c == '4') return "4";
    else if (c == '5') return "5";
    else if (c == '6') return "6";
    else if (c == '7') return "7";
    else if (c == '8') return "8";
    else if (c == '9') return "9";
    else return "";
}

void update_menu(int cursor) {
    for (int i = 0; i < menubar.size(); ++i) {
        if (cursor != i)
            mvwprintw(settings_win, 1 + i, 2, (menubar[i]).c_str());
        else {
            wattron(settings_win, A_STANDOUT);
            mvwprintw(settings_win, cursor + 1, 2, (menubar[cursor]).c_str());
            wattroff(settings_win, A_STANDOUT);
        }
    }
}

bool check() {
    parse_from_file();
    if (SETTINGS[0] + SETTINGS[1] + SETTINGS[2] > 100) return false;
    return true;
}

void print_input() {
    string txt = "<Insert Value>";

    int yMaxS, xMaxS;
    getmaxyx(settings_win, yMaxS, xMaxS);

    wattron(settings_win, COLOR_PAIR(1));
    wattron(settings_win, A_BOLD);
    mvwprintw(settings_win, yMaxS - 3, (xMaxS - txt.size()) / 2, txt.c_str());
    wattroff(settings_win, COLOR_PAIR(1));
    wattroff(settings_win, A_BOLD);
}

void print_warning() {
    string txt = "  <WARNING!>  ";

    int yMaxS, xMaxS;
    getmaxyx(settings_win, yMaxS, xMaxS);

    wattron(settings_win, COLOR_PAIR(2));
    wattron(settings_win, A_BLINK);
    wattron(settings_win, A_BOLD);
    mvwprintw(settings_win, yMaxS - 3, (xMaxS - txt.size()) / 2, txt.c_str());
    wattroff(settings_win, COLOR_PAIR(2));
    wattroff(settings_win, A_BLINK);
    wattroff(settings_win, A_BOLD);
}