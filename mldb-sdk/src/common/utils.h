#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

#include <chrono>   // NOLINT
#include <thread>   // NOLINT
#include <random>

using std::this_thread::sleep_for;
using std::chrono::milliseconds;
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;

// unit is millisecond
void sleepRandom(int min, int max) {
    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> dist(min, max);
    sleep_for(milliseconds(dist(mt)));
}

#endif  // COMMON_UTILS_H_
