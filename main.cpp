/*
Сделано студентом МИРЭА Гришиным Ильей Дмитриевичем
Чтобы запустить:
/home/WithSoull/Home/Coding/C++/WildLifeGame/cmake-build-debug/WildLifeGame
*/

#include "terminal.h"
#include "logic.h"
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <chrono>
#include <thread>
#include "emoji.h"

using namespace std;

const double PRC_START_PRED = 0.005;
const double PRC_START_HERB = 0.035;
const double PRC_START_NATURE = 0.1;


int main() {
    int X = give_terminal_size().first;
    int Y = give_terminal_size().second;
    int square = (X - 3) * (Y - 2);
    cout << square << "\n";
    int count_herb = (int) square * PRC_START_HERB;
    int count_pred = (int) square * PRC_START_PRED;
    int count_nature = (int) square * PRC_START_NATURE;

//    count_pred;

    srand(time(0));
//    start();
    sleep(1);
    fill_screen_empty();
    set_start_animals(count_pred, count_herb, count_nature);
    update_screen();
    sleep(1);
    for (;;){
        life_tick();
        update_screen();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

}