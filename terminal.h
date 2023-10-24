#pragma once

#include <vector>
#include <string>
#include "logic.h"

void build_text(struct ascii_image asciiImage);

void update_screen();

void generate_ascii_logo();

std::pair<int, int> give_terminal_size();

void fill_screen_empty();

void start();

void change_pixel(int x, int y, std::string symbol);

void update_objects(std::vector<struct object> objs);

void update_last_object(object obj);

std::string which_object(int x, int y);

void update_objects(std::vector<struct object> objs);

bool screen_place_is_empty(int x, int y);

std::string get_pixel(int i, int j);