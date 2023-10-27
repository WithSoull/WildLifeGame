#pragma once

#include <vector>
#include <string>
#include <map>
#include "logic.h"

void build_text(struct ascii_image asciiImage);

void update_screen();

void generate_ascii_logo();

std::pair<int, int> give_terminal_size();

void fill_screen_empty();

void start();

void change_pixel(int x, int y, std::string symbol);

void update_last_object(object obj);

void update_objects(std::map<std::string, std::vector<struct object>> objs);

bool screen_place_is_empty(int x, int y);

std::string get_pixel(int i, int j);

int absolute_path_by_coords(int x, int y);
