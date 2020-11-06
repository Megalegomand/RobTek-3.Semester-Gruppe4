#pragma once
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;
class Timer
{
public:
    Timer();

    void start();
    int elapsedMillis();
    time_point<steady_clock> getStartTime();
    void sleepUntil(int offset);
private:
    steady_clock clock;
    time_point<steady_clock> startTime;
};

