#include <cstdlib>
#include <iostream>
#include <csignal>
#include <chrono>
#include <thread>

#include "terminal.h"
#include <cmath>
#include <map>
#include <algorithm>

using namespace std;

map<string, vector<struct object>> objects = {
        {"herb",   vector<struct object>{}},
        {"pred",   vector<struct object>{}},
        {"nature", vector<struct object>{}},
        {"tombstone", vector<struct object>{}},
};

int random_number(int a, int b) {
    if (a > b) {
        swap(a, b);
    }
    return a + rand() % (b - a + 1);
}

void set_start_animals(int pred, int herb, int grass) {

    // pred
    for (int i = 0; i < pred; i++) {
        object new_pred;
        new_pred.hp = random_number(60, 80);
        new_pred.max_hp = random_number(80, 120);
        new_pred.type = "pred";
        new_pred.vision = random_number(3, 8);
        new_pred.speed = random_number(2, 5);

        set_random_free_coords(new_pred);
        update_last_object(new_pred);

        if (random_number(1, 2) % 2 == 0) {
            new_pred.emodji = "🦁";
        } else {
            new_pred.emodji = "🐯";
        }

        objects["pred"].push_back(new_pred);
    }

    // herb
    for (int i = 0; i < herb; i++) {
        object new_herb;
        new_herb.hp = random_number(15, 25);
        new_herb.max_hp = random_number(25, 35);

        new_herb.type = "herb";
        new_herb.vision = random_number(2, 4);
        new_herb.speed = random_number(1, 3);

        set_random_free_coords(new_herb);
        update_last_object(new_herb);

        int r = random_number(1, 3);
        if (r % 3 == 0) {
            new_herb.emodji = "🐮";
        } else if (r % 3 == 1) {
            new_herb.emodji = "🐷";
        } else {
            new_herb.emodji = "🐐";
        }

        objects["herb"].push_back(new_herb);
    }

    // nature
    for (int i = 0; i < grass; i++) {
        object new_nature;
        new_nature.hp = random_number(5, 10);
        new_nature.max_hp = new_nature.hp;
        new_nature.type = "nature";

        set_random_free_coords(new_nature);
        update_last_object(new_nature);

        if (random_number(1, 2) % 2 == 0) {
            new_nature.emodji = "🌿";
        } else {
            new_nature.emodji = "🌾";
        }


        objects["nature"].push_back(new_nature);
    }
    objects["pred"][0].emodji = "👾";

    sort_objects();
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
    hungryness();
    respawn_grass();
    update_objects(objects);
}

void hungryness() {
    for (auto p: objects) {
        for (int i = 0; i < objects[p.first].size(); ++i) {
            auto &obj = objects[p.first][i];
            if (p.first == "tombstone"){
                obj.hp -= 1;
            } else {
                obj.hp -= obj.max_hp * 0.03;
            }
            if (obj.hp <= 0) {
                objects[p.first].erase(objects[p.first].begin() + i);
                if (p.first == "herb" || p.first == "pred"){
                    set_tombstone(obj.x, obj.y);
                }
            }
        }
    }
}

void age_death() {

}

void update_age() {

}

void respawn_grass() {
    double curent = (double) objects["nature"].size();
    int X = give_terminal_size().first - 3;
    int Y = give_terminal_size().second - 2;
    while (curent / (double) (X * Y) < random_number(2, 12) / 100.0){
        object new_nature;
        new_nature.hp = random_number(5, 10);
        new_nature.max_hp = new_nature.hp;
        new_nature.type = "nature";

        set_random_free_coords(new_nature);
        update_last_object(new_nature);

        if (random_number(1, 2) % 2 == 0) {
            new_nature.emodji = "🌿";
        } else {
            new_nature.emodji = "🌾";
        }
        curent++;

        objects["nature"].push_back(new_nature);
    }
    sort_objects();
}

void move_objects() {
    for (int i = 0; i < objects["pred"].size(); i++) {
        auto pred = objects["pred"][i];
        move_pred(i);
    }

    for (int i = 0; i < objects["herb"].size(); i++) {
        auto herb = objects["herb"][i];
        move_herb(i);
    }
}

void move_pred(int index) {
    object &pred = objects["pred"][index];

    int v = pred.vision;
    int d = pred.speed;

    int hx, hy, lx, ly;
    get_near_coords(pred, v, hx, hy, lx, ly);

    pair<int, int> closed_herb = {-1, -1};
    int min_distance = 10000000;

    for (int i = hx; i <= lx; i++) {
        for (int j = hy; j <= ly; ++j) {
            if (!(i == pred.x && j == pred.y)) {
                if (is_object_this_type(i, j, "herb")) {
                    int current_distance = max(abs(pred.x - i), abs(pred.y - j));
                    if (min_distance > current_distance) {
                        closed_herb.first = i;
                        closed_herb.second = j;

                        min_distance = current_distance;
                    }
                }
            }
        }
    }

    // Кушаем
    if ((min_distance <= d) && (pred.hp / pred.max_hp < 0.8)) {
        int herb_index = get_obj(closed_herb.first, closed_herb.second, "herb");
        pred.hp += objects["herb"][herb_index].hp;
        objects["herb"].erase(objects["herb"].begin() + herb_index);
        pred.x = closed_herb.first;
        pred.y = closed_herb.second;
        set_tombstone(pred.x, pred.y);

        // Видим и подходим ближе
    } else if ((min_distance <= v) && (pred.hp / pred.max_hp < 0.8)) {
        int herb_index = get_obj(closed_herb.first, closed_herb.second, "herb");
        int x = objects["herb"][herb_index].x, y = objects["herb"][herb_index].y;

        get_near_coords(pred, d, hx, hy, lx, ly);
        int mindist = 10000000;
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

        // Никого не видим ходим рандомно
    } else {
        get_near_coords(pred, d, hx, hy, lx, ly);

        int i, j;
        do {
            i = random_number(hx, lx), j = random_number(hy, ly);
        } while (is_object_this_type(i, j, "pred"));

        pred.x = i, pred.y = j;
        int herb_index = get_obj(i, j, "herb");
        if (herb_index > -1) {
            pred.hp += objects["herb"][herb_index].hp;
            objects["herb"].erase(objects["herb"].begin() + herb_index);
            set_tombstone(i, j);
        }
    }
}

void move_herb(int index) {
    object &herb = objects["herb"][index];

    int v = herb.vision;
    int d = herb.speed;

    int hx, hy, lx, ly;
    // === Vision area ===
    get_near_coords(herb, v, hx, hy, lx, ly);

    // Добавляем в вектор всех ближайших львов
    // Добавляем в вектор всю ближайшую траву
    // Добавляем в вектор всю ближайшую свиней

    vector<struct object> preds;
    vector<struct object> grass;
    vector<struct object> herbs;

    for (int i = hx; i <= lx; i++) {
        for (int j = hy; j <= ly; ++j) {
            if (!(i == herb.x && j == herb.y)) {
                if (is_object_this_type(i, j, "pred")) {
                    preds.push_back(objects["pred"][get_obj(i, j, "pred")]);
                } else if (is_object_this_type(i, j, "nature")) {
                    grass.push_back(objects["nature"][get_obj(i, j, "nature")]);
                } else if (is_object_this_type(i, j, "herb")) {
                    herbs.push_back(objects["herb"][get_obj(i, j, "herb")]);
                }
            }
        }
    }

    // === Walk area ===

    get_near_coords(herb, d, hx, hy, lx, ly);

    int max_distance_preds = 0;
    int min_distance_nature = 1000000;
    vector<pair<int, int>> the_best_positions;
    int c = 1;
    for (int i = hx; i <= lx; i++) {
        for (int j = hy; j <= ly; ++j, c++) {
            if (!is_object_this_type(i, j, "herb")) {
                pair<int, int> curent_pos = {i, j};

                int cur_distance_preds = 0;
                for (const auto &pred: preds) {
                    cur_distance_preds += max(abs(pred.x - i), abs(pred.y - j));
                }
                // Находим позицию где хищники дальше всего
                if (max_distance_preds < cur_distance_preds) {
                    max_distance_preds = cur_distance_preds;

                    // Самое главное найти более выгодную позицую по хищникам,
                    // поэтому сбрасываем инфу по траве
                    min_distance_nature = 1000000;
                    the_best_positions = {curent_pos};
                }

                    // Если в этой позиции хищники дальше всего, то
                    // Ищем среди этих позиций ту, которая ближе всего к траве
                else if (max_distance_preds == cur_distance_preds) {
                    int cur_distance_nature = 0;
                    for (const auto &nature: grass) {
                        cur_distance_nature += max(abs(nature.x - i), abs(nature.y - j));
                    }

                    // Если можем встать на траву, встаем
                    if (is_object_this_type(i, j, "nature") && (herb.hp / herb.max_hp < 0.8)) {
                        cur_distance_nature = 0;
                    }

                    // Среди позиций где хищник дальше всего, эта отличается тем,
                    // что трава здесь ближе чем в остальных, поэтому обновляем лучшие позиции
                    if (min_distance_nature > cur_distance_nature) {
                        min_distance_nature = cur_distance_nature;
                        the_best_positions = {curent_pos};

                        // Добавляем новую, выгодную позицию
                    } else if (min_distance_nature == cur_distance_nature) {
                        the_best_positions.push_back(curent_pos);
                    }
                }
            }
        }
    }

    if (!the_best_positions.empty()) {
        int k = random_number(0, the_best_positions.size() - 1);
        auto pos = the_best_positions[k];

        herb.x = pos.first;
        herb.y = pos.second;


        // Если мы наступаем на траву, то кушаем ее
        int grass_index = get_obj(herb.x, herb.y, "nature");
        if (grass_index > -1) {
            object grass = objects["nature"][grass_index];
            herb.hp += grass.hp;
            objects["nature"].erase(objects["nature"].begin() + grass_index);
        }
    }


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

bool is_object_this_type(int x, int y, string type) {
    if (get_obj(x, y, type) > -1) {
        return true;
    }
    return false;
}

bool object_sort_crit(const struct object &o1, const struct object &o2) {
    int o1_place = absolute_path_by_coords(o1.x, o1.y);;
    int o2_place = absolute_path_by_coords(o2.x, o2.y);
    return o1_place < o2_place;
}

void sort_objects() {
    for (auto type: objects) {
        sort(objects[type.first].begin(), objects[type.first].end(), object_sort_crit);
    }
}

int get_obj(int x, int y, string type) {
    auto v = objects[type];
    int target = absolute_path_by_coords(x, y);
    int l = 0, r = v.size() - 1;

    while (l <= r) {
        int m = l + (r - l) / 2;
        int cur = absolute_path_by_coords(v[m].x, v[m].y);
        if (cur == target)
            return m;
        if (cur < target)
            l = m + 1;
        else
            r = m - 1;
    }

    return -1;
}

int absolute_path_by_coords(int x, int y) {
    int X = give_terminal_size().first - 3;
    int Y = give_terminal_size().second - 2;
    return (x - 1) * Y + y;
}

void set_tombstone(int i, int j){
    object tomb;
    tomb.x = i;
    tomb.y = j;
    tomb.hp = 5;
    tomb.emodji = "☠ ";
    objects["tombstone"].push_back(tomb);
}