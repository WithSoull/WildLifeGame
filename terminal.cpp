#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>
#include <utility>
#include "terminal.h"
#include "logic.h"

#include <ncurses.h>
#include <map>

using namespace std;

pair<int, int> give_terminal_size();



/* Структуры блоки */
const string fence_vertical_left = "| ";
const string fence_vertical_right = " |";
const string fence_horizontal = "==";
/* =============== */

int X, Y;

vector<vector<string>> screen;


struct ascii_image {
    int len_x, len_y;
    int x, y;
    vector<vector<string>> image;
};


void build_text(struct ascii_image asciiImage) {
    for (int i = asciiImage.x; i < asciiImage.x + asciiImage.len_x; ++i) {
        for (int j = asciiImage.y; j < asciiImage.y + asciiImage.len_y; ++j) {
            screen[i][j] = asciiImage.image[i - asciiImage.x][j - asciiImage.y];
        }
    }
}


void build_fence() {
    for (int i = 0; i < X; ++i) {
        screen[i][0] = fence_vertical_left;
        screen[i][Y - 1] = fence_vertical_right;
    }

    for (int i = 0; i < Y; ++i) {
        screen[0][i] = fence_horizontal;
        screen[X - 1][i] = fence_horizontal;
    }
}


void update_screen() {
//    system("clear");

    for (int i = 0; i < screen.size(); i++) {
        for (int j = 0; j < screen[i].size(); j++) {
            cout << screen[i][j];
        }
        cout << endl;
    }

}

void generate_ascii_logo() {
    ascii_image logo;
    logo.len_x = 11;
    logo.len_y = 32;
    logo.x = (X - logo.len_x) / 2;
    logo.y = (Y - logo.len_y) / 2;
    vector<string> img_v_s = {
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

    vector<vector<string>> img_v_v_s;

    for (int x = 0; x < img_v_s.size(); ++x){
        vector<string> newline;
        for (int i = 0; i < img_v_s[x].size(); i += 2) {
            string box = img_v_s[x].substr(i, 2);
            newline.push_back(box);
        }
        img_v_v_s.push_back(newline);
    }

    logo.image = img_v_v_s;

    build_text(logo);
}


pair<int, int> give_terminal_size() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    pair<int, int> res = {w.ws_row, w.ws_col / 2};
    return res;
}


void fill_screen_empty() {
    pair<int, int> terminal_size = give_terminal_size();
    X = terminal_size.first - 1, Y = terminal_size.second;
    screen.clear();
    for (int i = 0; i < X; i++) {
        vector<string> new_row;
        for (int j = 0; j < Y; j++) {
            new_row.push_back("  ");
        }
        screen.push_back(new_row);
    }

    build_fence();
}


void start() {
    pair<int, int> terminal_size = give_terminal_size();
    X = terminal_size.first - 1, Y = terminal_size.second;

    fill_screen_empty();
    generate_ascii_logo();
    update_screen();
}

void change_pixel(int x, int y, string symbol){
    screen[x][y] = symbol;
}


void update_objects(vector<struct object> objs){
    for (const auto& obj : objs)
        change_pixel(obj.x, obj.y, obj.emodji);
}


bool screen_place_is_empty(int x, int y){
    if (screen[x][y] == "  ") {
        return true;
    } else {
        return false;
    }
}

void update_last_object(object obj){
    screen[obj.x][obj.y] = obj.emodji;
};

map<string, string> hm = {
        {"🐮", "herb"},
        {"🐷", "herb"},
        {"🦁", "pred"},
        {"🐯", "pred"},
        {"🌿", "nature"},
        {"🌾", "nature"}
};

string which_object(int x, int y){
    return hm[screen[x][y]];
}