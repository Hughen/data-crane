#ifndef _UTILS_H_
#define _UTILS_H_

#include <chrono>
#include <thread>
#include <cstdlib>

using namespace std;

// unit is millisecond
void sleepRandom(int min, int max) {
    int t = rand() % max + min;
    if (t == 0) {
        return;
    }
    this_thread::sleep_for(chrono::milliseconds(t));
}

#endif