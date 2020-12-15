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

    // Start og restart timer
    void start();

    // Get milliseconds since start()
    int elapsedMillis();

    // Get time of start()
    time_point<steady_clock> getStartTime();

    // Sleep until offset millis has passed since start()
    void sleepUntil(int offset);
private:
    steady_clock clock;
    time_point<steady_clock> startTime;
};

