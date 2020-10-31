#include "Goertzel.h"

Goertzel::Goertzel(int targetFrequency, int sampleFrequency)
{
	this->targetFrequency = targetFrequency;
	this->sampleFrequency = sampleFrequency;
}

float Goertzel::processSamples(int* samples, int N)
{
	float k = round(float(N * targetFrequency) / sampleFrequency);
	return k;
}
