#pragma once

#include <map>
#include <vector>
#include <string>

#include "ncursesw/ncurses.h"

void draw_start_menu(int cursor);

void start_game();

void paint_logo();

void fill_screen_empty();

bool screen_place_is_empty(int y, int x);

void update_last_object(WINDOW *win, struct object obj);

void update_objects(std::map<std::string, std::vector<struct object>> objs);

WINDOW* get_window(std::string name);

void update_file_values();