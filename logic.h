#pragma once

int random_number(int a, int b);

void set_start_animals(int pred, int herb, int grass);

int random_even_number(int a, int b);

void set_random_free_coords(struct object& obj);

struct object {
    std::string type; // herb / pred / nature
    int x, y;
    int hp;
    int age;
    std::string emodji;
    int max_age;
    int time_sex;
    int vision;
    int speed;
};

void life_tick();
void move_pred(int i);
void move_objects();

int get_obj(int x, int y, std::string type);

bool is_object_this_type(int x, int y, std::string type);

void sort_objects();
