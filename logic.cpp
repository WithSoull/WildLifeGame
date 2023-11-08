#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <ctime>
#include <cmath>
#include "logic.h"
#include "terminal.h"

using namespace std;

map<string, vector<struct object>> objects = {
        {"herb",      vector<struct object>{}},
        {"pred",      vector<struct object>{}},
        {"nature",    vector<struct object>{}},
        {"tombstone", vector<struct object>{}},
};

void set_start_objects(WINDOW *win) {
    int pred = 0, herb = 2, grass = 2;

    int yMax, xMax, x, y;
    getmaxyx(win, yMax, xMax);

    // pred
    for (int i = 0; i < pred; i++) {
        object new_pred;
        new_pred.hp = random_number(60, 80);
        new_pred.max_hp = random_number(80, 120);
        new_pred.type = "pred";
        new_pred.vision = random_number(3, 8);
        new_pred.speed = random_number(2, 3);
        new_pred.speed = 1;

        set_random_free_coords(win, new_pred);

        update_last_object(win, new_pred);

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
        new_herb.speed = random_number(1, 2);
        new_herb.time_sex = 8;

        set_random_free_coords(win, new_herb);
        update_last_object(win, new_herb);

        int r = random_number(1, 3);
        if (r % 3 == 0) {
            new_herb.emodji = "🐮";
        } else if (r % 3 == 1) {
            new_herb.emodji = "🐷";
        } else {
            new_herb.emodji = "🐐";
        }
        new_herb.emodji = "🐷";
        objects["herb"].push_back(new_herb);
    }

    // nature
    for (int i = 0; i < grass; i++) {
        object new_nature;
        new_nature.hp = random_number(5, 10);
        new_nature.max_hp = new_nature.speed;
        new_nature.type = "nature";

        set_random_free_coords(win, new_nature);
        update_last_object(win, new_nature);

        if (random_number(1, 2) % 2 == 0) {
            new_nature.emodji = "🌿";
        } else {
            new_nature.emodji = "🌾";
        }


        objects["nature"].push_back(new_nature);
    }

    sort_objects();
    update_objects(objects);
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
    return o1_place <= o2_place;
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
    move_objects();
    hungryness();
    respawn_grass();
    reload_sex_time();
    update_objects(objects);
    sort_objects();

    WINDOW *game_win = get_window("game_win");
    WINDOW *pred_win = get_window("pred_win");


    wrefresh(game_win);
    info();
}

void move_objects() {
    for (int i = 0; i < objects["pred"].size(); i++) {
        auto pred = objects["pred"][i];
        move_pred(i);
    }

    WINDOW *herb_win = get_window("herb_win");
    for (int i = 0; i < objects["herb"].size(); i++) {
        auto herb = objects["herb"][i];
        move_herb(i);
        mvwprintw(herb_win, 1 + i, 1, (to_string(herb.time_sex) + " " + to_string(herb.max_hp)).c_str());
        wrefresh(herb_win);
    }
}

void hungryness() {
    for (auto p: objects) {
        vector<int> erase_list;
        for (int i = 0; i < objects[p.first].size(); ++i) {
            auto &obj = objects[p.first][i];

            // Вычитаем хп объектов
            if (p.first == "tombstone") {
                obj.hp -= 1;
            } else {
                obj.hp -= obj.max_hp / 40;
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

    WINDOW *pred_win = get_window("pred_win");
    wclear(pred_win);
    wrefresh(pred_win);

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

    vector<struct object> preds;
    vector<struct object> grass;
    vector<struct object> herbs;

    for (int i = hy; i <= ly; i++) {
        for (int j = hx; j <= lx; ++j) {
            if (!(i == herb.y && j == herb.x)) {
                if (is_object_this_type(i, j, "pred")) {
                    preds.push_back(objects["pred"][get_obj(i, j, "pred")]);
                } else if (is_object_this_type(i, j, "nature")) {
                    grass.push_back(objects["nature"][get_obj(i, j, "nature")]);
                } else{
                    int k = get_obj(i, j, "herb");
                    if (k > -1){
                        object h = objects["herb"][get_obj(i, j, "herb")];
                        if (herb.emodji == h.emodji && h.time_sex > 30){
                            herbs.push_back(objects["herb"][get_obj(i, j, "herb")]);
                        }
                    }
                }
            }
        }
    }

    // === Walk area ===

    get_near_coords(herb, d, hx, hy, lx, ly);

    int max_distance_preds = 0;
    int min_distance_nature = 1000000;
    int min_distance_herb = 1000000;

    vector<pair<int, int>> the_best_positions;
    int c = 1;
    bool sex = false;
    pair<int, int> sex_herb = {-1, -1};
    for (int i = hy; i <= ly; i++) {
        for (int j = hx; j <= lx; ++j, c++) {
            pair<int, int> curent_pos = {i, j};
            if (is_object_this_type(i, j, "herb") || (i == herb.y && j == herb.x)){
                continue;
            }

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
                sex = false;
            }

            // Если в этой позиции хищники дальше всего, то
            // Ищем среди этих позиций ту, которая ближе всего к траве
            else if (max_distance_preds == cur_distance_preds) {
                if (herb.hp / herb.max_hp < 0.6) {
                    int cur_distance_nature = 0;
                    for (const auto &nature: grass) {
                        cur_distance_nature += max(abs(nature.x - j), abs(nature.y - i));
                    }

                    // Если можем встать на траву, встаем
                    if (is_object_this_type(i, j, "nature")) {
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
                } else if (herb.time_sex > 30) {
                    // Определяем расстояние до цели
                    int cur_distance_herb;
                    for (const auto &another_herb: herbs) {
                        cur_distance_herb = max(abs(another_herb.x - j), abs(another_herb.y - i));
                        if (min_distance_herb > cur_distance_herb) {
                            min_distance_herb = cur_distance_herb;
                            the_best_positions = {curent_pos};

                            if (cur_distance_herb == 1) {
                                sex = true;
                                sex_herb = {another_herb.y, another_herb.x};
                            }
                            // Добавляем новую, выгодную позицию
                        } else if (min_distance_nature == cur_distance_herb) {
                            the_best_positions.push_back(curent_pos);
                        }
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

        if (sex) {
            int sx = sex_herb.second, sy = sex_herb.first;
            int x = pos.second, y = pos.first;
            int min_empty_dist = 10000;

            pair<int, int> child_coords = {-1, -1};
            // Ищем место для ребенка

            for (int i = min(sy, y) - 1; i < max(sy, y) + 2; ++i) {
                for (int j = min(sx, x) - 1; i < max(sx, x) + 2; ++i) {
                    int cur_dist1 = (i - y) * (i - y) + (j - x) * (j - x);
                    int cur_dist2 = (i - sy) * (i - sy) + (j - sx) * (j - sx);
                    if (cur_dist1 + cur_dist2 < min_empty_dist
                        && !is_object_this_type(i, j, "herb")
                        && !is_object_this_type(i, j, "pred")) {
                        min_empty_dist = cur_dist1 + cur_dist2;
                        child_coords = {i, j};
                    }
                }
            }

            // Нашли место для ребенка
            int parent2_index = get_obj(sy, sx, "herb");

            if (child_coords.first >= 0 && parent2_index > -1){
                object& another_herb = objects["herb"][parent2_index];
                born_new_child(child_coords.first, child_coords.second, herb, another_herb);

            } else {
                int i, j;
                do {
                    i = random_number(hy, ly);
                    j = random_number(hx, lx);
                } while (is_object_this_type(i, j, "herb") || is_object_this_type(i, j, "pred"));

                herb.x = j, herb.y = i;
            }
        }
    } else {
        int i, j;
        do {
            i = random_number(hy, ly);
            j = random_number(hx, lx);
        } while (is_object_this_type(i, j, "herb") || is_object_this_type(i, j, "pred"));

        herb.x = j, herb.y = i;
    }

    // Если мы наступаем на траву, то кушаем ее
    int grass_index = get_obj(herb.y, herb.x, "nature");
    if (grass_index > -1) {
        object eat_grass = objects["nature"][grass_index];
        herb.hp += eat_grass.hp;
        objects["nature"].erase(objects["nature"].begin() + grass_index);
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

    while (curent / (double) ((xMax / 2 - 1) * (yMax - 2)) < random_number(2, 12) / 100.0) {
        object new_nature;
        new_nature.hp = random_number(5, 10);
        new_nature.max_hp = new_nature.hp;
        new_nature.type = "nature";

        set_random_free_coords(game_win, new_nature);
        update_last_object(game_win, new_nature);

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

void born_new_child(int y, int x, object& parent1, object& parent2){
    object child;
    if (parent1.hp <= 0 || parent2.hp <= 0)
        return;

    child.emodji = parent1.emodji;
    child.x = x;
    child.y = y;
    child.time_sex = -10;
    child.max_hp = (parent1.max_hp + parent2.max_hp) / 2;

    child.hp = child.max_hp * 0.6;

    child.speed = (parent1.speed + parent2.speed) / 2;
//    child.speed += child.speed * random_number(-5, 5) / 100;

    child.vision = (parent1.vision + parent2.vision) / 2;
//    child.vision += child.vision * random_number(-5, 5) / 100;
    objects["herb"].push_back(child);

    parent1.time_sex = 0;
    parent2.time_sex = 0;

    sort_objects();
}

void reload_sex_time(){
    for (auto &obj: objects["herb"]){
        obj.time_sex++;
    }
}

void info(){
    WINDOW *pw = get_window("pred_win");
    if (objects["pred"].size() > 0)
        mvwprintw(pw, 1, 1, "%s", to_string(objects["pred"][0].hp).c_str());
    wrefresh(pw);
}