#pragma once

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

int random_number(int a, int b);

void set_start_animals(int pred, int herb, int grass);

void set_random_free_coords(struct object& obj);

void life_tick();

void move_objects();

int get_obj(int x, int y, std::string type);

bool is_object_this_type(int x, int y, std::string type);

void sort_objects();



int absolute_path_by_coords(int x, int y);

void move_objects();

void respawn_grass();

void move_herb(int index);

void move_pred(int index);

void update_age();

void age_death();

void get_near_coords(object obj, int v, int &hx, int &hy, int &lx, int &ly);

void set_tombstone(int i, int j);

void hungryness();