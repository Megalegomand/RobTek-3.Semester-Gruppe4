#include "Timer.h"

Timer::Timer()
{
}

void Timer::start()
{
	startTime = clock.now();
}

int Timer::elapsedMillis()
{   // Returns duration of the time past
	return std::chrono::duration<double, std::milli>(clock.now() - startTime).count();
}

time_point<steady_clock> Timer::getStartTime()
{   // Returns the timer
	return startTime;
}

void Timer::sleepUntil(int offset)
{   // Sets the sleep duration
	std::this_thread::sleep_until(startTime + milliseconds(offset));
}
