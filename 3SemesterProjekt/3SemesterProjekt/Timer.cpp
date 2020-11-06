#include "Timer.h"

Timer::Timer()
{
}

void Timer::start()
{
	startTime = clock.now();
}

int Timer::elapsedMillis()
{
	return std::chrono::duration<double, std::milli>(clock.now() - startTime).count();
}

time_point<steady_clock> Timer::getStartTime()
{
	return startTime;
}

void Timer::sleepUntil(int offset)
{
	std::this_thread::sleep_until(startTime + milliseconds(offset));
}
