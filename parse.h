#pragma once

void start_parse();

void parse_from_file();

WINDOW *get_settings_window();

void change_NAMES_size_for_this_window();

std::string is_digit(int c);

void update_menu(int cursor);

bool check();

void print_input();

void print_warning();

std::vector<int> get_settings();