#pragma once

#include <ncursesw/curses.h>

void set_start_objects(WINDOW *win);

int random_number(int a, int b);

void set_random_free_coords(WINDOW *win, struct object &obj);

struct object {
    std::string type; /* herb / pred / nature / tombstone */
    int x, y;
    double hp;
    double max_hp;
    int age;
    std::string emodji;
    int max_age;
    int time_sex;
    int vision;
    int speed;
};

int absolute_path_by_coords(int x, int y);

void sort_objects();

bool object_sort_crit(const struct object &o1, const struct object &o2);

void life_tick();

void move_objects();

void move_pred(int index);

void move_herb(int index);

void get_near_coords(object obj, int v, int &hx, int &hy, int &lx, int &ly);

bool is_object_this_type(int x, int y, std::string type);

int get_obj(int x, int y, std::string type);

void set_tombstone(int i, int j);

void hungryness();

void respawn_grass();