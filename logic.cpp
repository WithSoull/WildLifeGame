#include <cstdlib>
#include <iostream>
#include <csignal>
#include <chrono>
#include <thread>

#include "terminal.h"
#include <cmath>
#include <map>

using namespace std;

int random_number(int a, int b) {
    if (a > b){
        swap(a, b);
    }
    return a + rand() % (b - a + 1);
}

// std::this_thread::sleep_for(std::chrono::milliseconds(200));

vector<struct object> objects;

void move_objects();

void respawn_grass();

void move_herb(object object1);

void move_pred(object &pred);

void update_age();

void age_death();

void get_near_coords(object obj, int v, int &hx, int &hy, int &lx, int &ly);

int get_obj(int x, int y);

void paint_square(object &object, int v);

//"🦁🐯 🐐🌱"🌾""🌿"   🐮🐷"
void set_start_animals(int pred, int herb, int grass) {

    // pred
    for (int i = 0; i < pred; i++) {
        object new_pred;
        new_pred.type = "pred";
        new_pred.vision = 3;
        new_pred.speed = 2;

        set_random_free_coords(new_pred);
        update_last_object(new_pred);

        if (random_number(1, 2) % 2 == 0) {
            new_pred.emodji = "🦁";
        } else {
            new_pred.emodji = "🐯";
        }

        objects.push_back(new_pred);
    }

    // herb
    for (int i = 0; i < herb; i++) {
        object new_herb;
        new_herb.type = "herb";
        new_herb.vision = 3;
        new_herb.speed = 1;

        set_random_free_coords(new_herb);
        update_last_object(new_herb);

        if (random_number(1, 2) % 2 == 0) {
            new_herb.emodji = "🐮";
        } else {
            new_herb.emodji = "🐷";
        }

        objects.push_back(new_herb);
    }

    // nature
    for (int i = 0; i < grass; i++) {
        object new_nature;
        new_nature.type = "nature";
        new_nature.speed = 2;

        set_random_free_coords(new_nature);
        update_last_object(new_nature);

        if (random_number(1, 2) % 2 == 0) {
            new_nature.emodji = "🌿";
        } else {
            new_nature.emodji = "🌾";
        }


//        new_nature.emodji = "☁ ";

        objects.push_back(new_nature);
    }

    update_objects(objects);
}

void set_random_free_coords(struct object &obj) {
    pair<int, int> terminal_size = give_terminal_size();
    int X = terminal_size.first - 1, Y = terminal_size.second, x, y;
    do {
        x = random_number(1, X - 2);
        y = random_number(1, Y - 1);
    } while (!screen_place_is_empty(x, y));

    obj.x = x;
    obj.y = y;
}


void life_tick() {
    move_objects();
    respawn_grass();
    update_age();
    age_death();
}


void age_death() {

}

void update_age() {

}

void respawn_grass() {

}

void move_objects() {
    for (int i = 0; i < objects.size(); i++) {
        if (objects[i].type == "pred") {
            move_pred(i);
        }
    }
}

void move_pred(int index) {
    object &pred = objects[index];
    int fx = pred.x, fy = pred.y;
    int v = pred.vision;
    int d = pred.speed;

    int hx, hy, lx, ly;
    get_near_coords(pred, d, hx, hy, lx, ly);

    bool move = false;
    for (int i = hx; i <= lx; i++) {
        if (move == true) break;
        for (int j = hy; j <= ly; ++j) {
            if (!(i == pred.x && j == pred.y)) {
                if (which_object(i, j) == "herb") {
                    int k = get_obj(i, j);
                    move = true;
                    pred.hp += objects[k].hp;
                    objects.erase(objects.begin() + k);
                    pred.x = i;
                    pred.y = j;
                    break;
                }
            }
        }
    }

    int herb_index = -1;
    if (!move) {
        get_near_coords(pred, v, hx, hy, lx, ly);

        // Ищем жертву которая ближе всего к нам

        int herb_min_distance = 1000000;
        for (int i = hx; i <= lx; i++) {
            for (int j = hy; j <= ly; ++j) {
                if (which_object(i, j) == "herb") {
                    cout << i << " " << j << "\n";
                    int curi = get_obj(i, j);
                    int curd2 = (i - pred.x) * (i - pred.x) + (j - pred.y) * (j - pred.y);

                    if (herb_min_distance > curd2) {
                        herb_index = curi;
                        herb_min_distance = curd2;

                    } else if (herb_min_distance == curd2) {
                        int hp1 = objects[herb_index].hp;
                        int hp2 = objects[curi].hp;
                        if (hp1 < hp2) {
                            herb_index = curi;
                        }
                    }
                }
            }
        }


    }
    get_near_coords(pred, d, hx, hy, lx, ly);
    if (herb_index == -1 && !move) {

        cout << "Жертвы нет, рандомный ход Наступил на: ";

        pred.x = random_number(hx, lx);
        pred.y = random_number(ly, hy);
        pred.last_step_emodji = get_pixel(pred.x, pred.y);

        cout << pred.last_step_emodji << "\n";

    } else if (herb_index > -1) {
        // Ищем позицию для льва чтобы встать ближе к корму :)

        cout << "Жертвы где-то рядом. ";

        int x = objects[herb_index].x, y = objects[herb_index].y;
        int mindist = 1000;
        for (int i = hx; i <= lx; i++) {
            for (int j = hy; j <= ly; ++j) {
                int curd2 = (i - x) * (i - x) + (j - y) * (j - y);
                if (mindist > curd2) {
                    mindist = curd2;
                    pred.x = i;
                    pred.y = j;
                }
            }
        }

        cout << "Лучшая позиция: " << x << " " << y << "\n";
    }

    change_pixel(fx, fy, pred.last_step_emodji);
    change_pixel(pred.x, pred.y, pred.emodji);
}

void get_near_coords(object obj, int v, int &hx, int &hy, int &lx, int &ly) {
    pair<int, int> terminal_size = give_terminal_size();
    int X = terminal_size.first - 3, Y = terminal_size.second - 2;
    int thx = 1, thy = 1;
    int tlx = thx + X - 1, tly = thy + Y - 1;
    hx = max(thx, obj.x - v);
    hy = max(thy, obj.y - v);
    lx = min(tlx, obj.x + v);
    ly = min(tly, obj.y + v);
}

int get_obj(int x, int y) {
    for (int i = 0; i < objects.size(); i++) {
        object el = objects[i];
        if (el.x == x && el.y == y) {
            return i;
        }
    }
    return -1;
}

void move_herb(object &herb) {

}
