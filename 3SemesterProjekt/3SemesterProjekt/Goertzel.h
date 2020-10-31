#pragma once
#include <math.h>

class Goertzel
{
public:
	Goertzel(int targetFrequency, int sampleFrequency);
	float processSamples(int* samples, int count);

private:
	const double PI = 3.14159265359;
	int targetFrequency;
	int sampleFrequency;
	int k;
};

