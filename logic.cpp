#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <ctime>
#include <cmath>
#include "logic.h"
#include "parse.h"
#include "terminal.h"

using namespace std;

int DAY = 1;
int count_herb = 0;
int count_grass = 0;

map<string, vector<struct object>> objects = {
        {"herb",      vector<struct object>{}},
        {"pred",      vector<struct object>{}},
        {"nature",    vector<struct object>{}},
        {"tombstone", vector<struct object>{}},
        {"light",     vector<struct object>{}},
};

object generateNewAnimal(string type) {
    if (type == "pred") {
        object new_pred;
        new_pred.hp = random_number(60, 80);
        new_pred.max_hp = random_number(80, 120);
        new_pred.type = "pred";
        new_pred.vision = random_number(2, 6);
        new_pred.speed = random_number(1, 4);
        new_pred.time_sex = 0;

        if (random_number(1, 2) % 2 == 0) {
            new_pred.emodji = "🐅";
        } else {
            new_pred.emodji = "🐆";
        }

        return new_pred;

    } else if (type == "herb") {
        object new_herb;
        new_herb.hp = random_number(15, 25);
        new_herb.max_hp = random_number(25, 35);
        new_herb.type = "herb";
        new_herb.vision = random_number(2, 4);
        new_herb.speed = random_number(1, 2);
        new_herb.time_sex = 0;

        int r = random_number(1, 3);
        if (r % 3 == 0) {
            new_herb.emodji = "🐄";
        } else if (r % 3 == 1) {
            new_herb.emodji = "🐖";
        } else {
            new_herb.emodji = "🐂";
        }

        return new_herb;

        // if (type == "nature")
    } else {
        object new_nature;
        new_nature.hp = random_number(5, 10);
        new_nature.max_hp = new_nature.hp;
        new_nature.type = "nature";

        int r = random_number(1, 4);
        if (r == 1) {
            new_nature.emodji = "🌿";
        } else if (r == 2){
            new_nature.emodji = "🌱";
        } else if (r == 3) {
            new_nature.emodji = "🍀";
        } else if (r == 4) {
            new_nature.emodji = "🍃";
        }

        return new_nature;
    }
}

void set_start_objects(WINDOW *win) {
    DAY = 1;
    int yMax, xMax, x, y;
    getmaxyx(win, yMax, xMax);

    int square = (yMax - 2) * (xMax / 2 - 2);

    int pred = get_settings()[0] * square / 100;
    int herb = get_settings()[1] * square / 100;
    int grass = get_settings()[2] * square / 100;

    // pred
    for (int i = 0; i < pred; i++) {
        object new_pred = generateNewAnimal("pred");

        set_random_free_coords(win, new_pred);
        update_last_object(win, new_pred);

        objects["pred"].push_back(new_pred);
    }

    // herb
    for (int i = 0; i < herb; i++) {
        object new_herb = generateNewAnimal("herb");


        set_random_free_coords(win, new_herb);
        update_last_object(win, new_herb);


        objects["herb"].push_back(new_herb);
    }

    // nature
    for (int i = 0; i < grass; i++) {
        object new_nature = generateNewAnimal("nature");

        set_random_free_coords(win, new_nature);
        update_last_object(win, new_nature);


        objects["nature"].push_back(new_nature);
    }

    sort_objects();
    update_objects(objects);
}

void born_childs() {
    const int SEX_TIME = 30;
    WINDOW *win = get_window("game_win");
    vector<struct object> animals;
    for (auto p: objects) {
        if (p.first == "herb" || p.first == "pred") {
            for (auto &obj: p.second) {
                animals.push_back(obj);
            }
        }
    }

    for (auto &animal: animals) {
        if (animal.time_sex < SEX_TIME || (animal.type == "pred" && animal.time_sex < 2 * SEX_TIME)) {
            continue;
        }
        int hx, hy, lx, ly;
        get_near_coords(animal, 1, hx, hy, lx, ly);
        vector<pair<int, int>> this_type_animals;
        vector<pair<int, int>> empty_places;

        for (int i = hy; i <= ly; i++) {
            for (int j = hx; j <= lx; j++) {
                if (i != animal.x || j != animal.y) {
                    pair<int, int> pos = {i, j};
                    int index = get_obj(i, j, animal.type);

                    if (index > -1) {
                        object &another_animal = objects[animal.type][index];
                        if (another_animal.emodji == animal.emodji && another_animal.time_sex >= SEX_TIME)
                            this_type_animals.push_back(pos);
                    } else if (screen_place_is_empty(i, j)) {
                        empty_places.push_back(pos);
                    }
                }
            }
        }

        if (!empty_places.empty()) {
            if (this_type_animals.size() < 5 && this_type_animals.size() > 1) {
                int child_pos_index = random_number(0, empty_places.size() - 1);
                object child = generateNewAnimal(animal.type);
                child.y = empty_places[child_pos_index].first;
                child.x = empty_places[child_pos_index].second;

                child.time_sex = -20;
                update_last_object(win, child);
                objects[animal.type].push_back(child);

                int second_parent_index = random_number(0, this_type_animals.size() - 1);
                int second_parent_y = this_type_animals[second_parent_index].first;
                int second_parent_x = this_type_animals[second_parent_index].second;
                object &second_parent = objects[animal.type][get_obj(second_parent_y, second_parent_x, animal.type)];

                animal.time_sex = 0;
                second_parent.time_sex = 0;
            }
        }

        this_type_animals.clear();
        empty_places.clear();
    }
    animals.clear();
}

int random_number(int a, int b) {
    if (a > b) {
        swap(a, b);
    }
    return a + rand() % (b - a + 1);
}

void set_random_free_coords(WINDOW *win, struct object &obj) {
    int yMax, xMax, x, y;
    getmaxyx(win, yMax, xMax);

    do {
        x = random_number(1, xMax - 2);
        y = random_number(1, yMax - 2);
    } while (!screen_place_is_empty(y, x / 2) || x % 2 == 0);

    obj.x = x / 2;
    obj.y = y;
}

bool object_sort_crit(const struct object &o1, const struct object &o2) {
    int o1_place = absolute_path_by_coords(o1.y, o1.x);
    int o2_place = absolute_path_by_coords(o2.y, o2.x);
    return o1_place < o2_place;
}

int absolute_path_by_coords(int y, int x) {
    WINDOW *win = get_window("game_win");
    int yMax, xMax;
    getmaxyx(win, yMax, xMax);
    return (x - 1) * yMax + y;
}

void sort_objects() {
    for (auto type: objects) {
        sort(objects[type.first].begin(), objects[type.first].end(), object_sort_crit);
    }
}

void life_tick() {
    srand(time(0));

    move_objects();
    generate_lightning_strike();
    hungryness();
    respawn_grass();
    update_day();
    sort_objects();

    update_counts();
    born_childs();
    sort_objects();
    update_time_sex();
    update_objects(objects);
}

int get_animal_count() {
    return objects["herb"].size() + objects["pred"].size();
}

void update_counts() {
    int h = objects["herb"].size();
    int p = objects["pred"].size();
    int n = objects["nature"].size();
    WINDOW *w = get_window("data_win");

    wclear(w);
    box(w, 0, 0);

    int yMaxS, xMaxS;
    getmaxyx(w, yMaxS, xMaxS);

    wattron(w, COLOR_PAIR(2));
    mvwprintw(w, 1, xMaxS / 5, ("Preadators: " + to_string(p)).c_str());
    wattroff(w, COLOR_PAIR(2));

    wattron(w, COLOR_PAIR(3));
    mvwprintw(w, 1, 2 * xMaxS / 5, ("Herbivores: " + to_string(h)).c_str());
    wattroff(w, COLOR_PAIR(3));

    wattron(w, COLOR_PAIR(1));
    mvwprintw(w, 1, 3 * xMaxS / 5, ("Grass: " + to_string(n)).c_str());
    wattroff(w, COLOR_PAIR(1));

}

void generate_lightning_strike() {
    int r = random_number(1, 100);
    WINDOW *win = get_window("game_win");
    if (r <= get_settings()[4]) {
        int yMax, xMax;
        getmaxyx(win, yMax, xMax);
        int y = random_number(1, yMax - 1);
        int x = random_number(1, xMax - 1) / 2;

        int hx, hy, lx, ly;
        int r = random_number(2, 5);
        get_near_coords(y, x, r, hx, hy, lx, ly);

        for (int i = hy; i <= ly; i++) {
            for (int j = hx; j <= lx; j++) {
                if ((i - y) * (i - y) + (j - x) * (j - x) <= r * r) {
                    int index;
                    index = get_obj(i, j, "herb");
                    if (index > -1)
                        objects["herb"].erase(objects["herb"].begin() + i);

                    index = get_obj(i, j, "pred");
                    if (index > -1)
                        objects["pred"].erase(objects["pred"].begin() + i);

                    index = get_obj(i, j, "nature");
                    if (index > -1)
                        objects["nature"].erase(objects["nature"].begin() + i);

                    object light;
                    light.hp = 5 + random_number(1, 4);
                    light.max_hp = light.hp;
                    light.type = "light";
                    light.x = j;
                    light.y = i;
                    int r = random_number(1, 3);
                    if (r == 1) light.emodji = "💥";
                    else light.emodji = "⚡";
                    objects["light"].push_back(light);
                }
            }
        }
    }
}

void move_objects() {
    for (int i = 0; i < objects["pred"].size(); i++) move_pred(i);
    for (int i = 0; i < objects["herb"].size(); i++) move_herb(i);
}

void hungryness() {
    for (auto p: objects) {
        vector<int> erase_list;
        for (int i = 0; i < objects[p.first].size(); ++i) {
            auto &obj = objects[p.first][i];

            // Вычитаем хп объектов
            if (p.first == "tombstone" || p.first == "tombstone") {
                obj.hp -= 1;
            } else {
                obj.hp -= obj.max_hp / 20;
            }

            // Убиваем объект
            if (obj.hp <= 0) {
                erase_list.push_back(i); // Далее эти элементы мы будем стирать
                if (p.first == "herb" || p.first == "pred") {
                    set_tombstone(obj.y, obj.x);
                }
            }
        }
        // Стираем мертвые объекты
        for (int i: erase_list) {
            objects[p.first].erase(objects[p.first].begin() + i);
        }
    }
}

void move_pred(int index) {
    object &pred = objects["pred"][index];

    int v = pred.vision;
    int d = pred.speed;

    WINDOW *game_win = get_window("game_win");

    int hx, hy, lx, ly;

    get_near_coords(pred, v, hx, hy, lx, ly);

    wrefresh(game_win);

    pair<int, int> closed_herb = {-1, -1};
    int min_distance = 10000000;


    for (int i = hy; i <= ly; i++) {
        for (int j = hx; j <= lx; ++j) {
            if (!(i == pred.x && j == pred.y)) {
                if (is_object_this_type(i, j, "herb")) {
                    int current_distance = max(abs(pred.y - i), abs(pred.x - j));
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
        pred.y = closed_herb.first;
        pred.x = closed_herb.second;
        set_tombstone(pred.y, pred.x);

        // Видим и подходим ближе
    } else if ((min_distance <= v) && (pred.hp / pred.max_hp < 0.8)) {
        int herb_index = get_obj(closed_herb.first, closed_herb.second, "herb");
        int x = objects["herb"][herb_index].x, y = objects["herb"][herb_index].y;

        get_near_coords(pred, d, hx, hy, lx, ly);
        int mindist = 10000000;
        for (int i = hy; i <= ly; i++) {
            for (int j = hx; j <= lx; ++j) {
                int curd2 = (i - y) * (i - y) + (j - x) * (j - x);
                if (mindist > curd2) {
                    mindist = curd2;
                    pred.y = i;
                    pred.x = j;
                }
            }
        }

        // Никого не видим ходим рандомно
    } else {
        get_near_coords(pred, d, hx, hy, lx, ly);

        int i, j;
        do {
            i = random_number(hy, ly), j = random_number(hx, lx);
        } while (is_object_this_type(i, j, "pred"));

        pred.y = i, pred.x = j;
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

    for (int i = hy; i <= ly; i++) {
        for (int j = hx; j <= lx; ++j) {
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
    for (int i = hy; i <= ly; i++) {
        for (int j = hx; j <= lx; ++j, c++) {
            if (!is_object_this_type(i, j, "herb") && !is_object_this_type(i, j, "light")) {
                pair<int, int> curent_pos = {i, j};

                int cur_distance_preds = 0;
                for (const auto &pred: preds) {
                    cur_distance_preds += max(abs(pred.x - j), abs(pred.y - i));
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
                        cur_distance_nature += max(abs(nature.x - j), abs(nature.y - i));
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

        herb.y = pos.first;
        herb.x = pos.second;


        // Если мы наступаем на траву, то кушаем ее
        int grass_index = get_obj(herb.y, herb.x, "nature");
        if (grass_index > -1) {
            object grass = objects["nature"][grass_index];
            herb.hp += grass.hp;
            objects["nature"].erase(objects["nature"].begin() + grass_index);
        }
    }
}

void get_near_coords(object obj, int v, int &hx, int &hy, int &lx, int &ly) {
    WINDOW *game_win = get_window("game_win");
    int yMax, xMax;
    getmaxyx(game_win, yMax, xMax);

    int thx = 0, thy = 1;
    int tlx = xMax / 2 - 2, tly = yMax - 2;

    hx = max(thx, obj.x - v);
    hy = max(thy, obj.y - v);
    lx = min(tlx, obj.x + v);
    ly = min(tly, obj.y + v);
}

void get_near_coords(int y, int x, int v, int &hx, int &hy, int &lx, int &ly) {
    WINDOW *game_win = get_window("game_win");
    int yMax, xMax;
    getmaxyx(game_win, yMax, xMax);

    int thx = 0, thy = 1;
    int tlx = xMax / 2 - 2, tly = yMax - 2;

    hx = max(thx, x - v);
    hy = max(thy, y - v);
    lx = min(tlx, x + v);
    ly = min(tly, y + v);
}

bool is_object_this_type(int y, int x, string type) {
    if (get_obj(y, x, type) > -1) {
        return true;
    }
    return false;
}

int get_obj(int y, int x, string type) {
    auto v = objects[type];
    int target = absolute_path_by_coords(y, x);
    int l = 0, r = v.size() - 1;

    while (l <= r) {
        int m = l + (r - l) / 2;
        int cur = absolute_path_by_coords(v[m].y, v[m].x);
        if (cur == target)
            return m;
        if (cur < target)
            l = m + 1;
        else
            r = m - 1;
    }

    return -1;
}

void set_tombstone(int i, int j) {
    object tomb;
    tomb.y = i;
    tomb.x = j;
    tomb.hp = 5;
    tomb.emodji = "☠ ";
    objects["tombstone"].push_back(tomb);
}

void respawn_grass() {
    WINDOW *game_win = get_window("game_win");
    int yMax, xMax;
    getmaxyx(game_win, yMax, xMax);

    double curent = (double) objects["nature"].size();

    int square = (xMax / 2 - 2) * (yMax - 2);

    int r = get_settings()[3];

    while (curent / (double) ((xMax / 2 - 2) * (yMax - 2)) < random_number(r - 5, r + 5) / 100.0) {
        int all_objects_count = objects["herb"].size() + objects["pred"].size() + objects["nature"].size();
//        if (all_objects_count >= square){
//            break;
//        }
        object new_nature = generateNewAnimal("nature");

        set_random_free_coords(game_win, new_nature);
        update_last_object(game_win, new_nature);

        if (random_number(1, 2) % 2 == 0) {
            new_nature.emodji = "🌿";
        } else {
            new_nature.emodji = "🌱";
        }
        curent++;

        objects["nature"].push_back(new_nature);
    }
    sort_objects();
}


void update_time_sex() {
    for (auto &object: objects["herb"]) {
        object.time_sex++;
    }
    for (auto &object: objects["pred"]) {
        object.time_sex++;
    }
}

void update_day() {
    WINDOW *w = get_window("data_win");
    int yMax, xMax;
    getmaxyx(w, yMax, xMax);

    mvwprintw(w, 1, 1, get_date().c_str());
    string txt = "Day - " + to_string(DAY);
    mvwprintw(w, 1, xMax - txt.size() - 2, txt.c_str());

    wrefresh(w);

    DAY += 1;
}

string get_date() {
    int num = DAY;
    int mas[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int m, y = 1;
    while (num > 365) {
        num -= 365;
        y += 1;
    }
    for (m = 1; num > mas[m - 1]; m++)
        num -= mas[m - 1];

    string res = "";

    if (to_string(num).size() == 1) res += "0";
    res += to_string(num) + ".";

    if (to_string(m).size() == 1) res += "0";
    res += to_string(m) + ".";

    if (to_string(y).size() == 1) res += "0";
    res += to_string(y);

    return res;
}
