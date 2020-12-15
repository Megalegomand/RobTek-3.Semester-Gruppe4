#include "Goertzel.h"

Goertzel::Goertzel()
{
}


Goertzel::Goertzel(int sampleFrequency)
{
	this->sampleFrequency = sampleFrequency;
}

float Goertzel::processSamples(deque<Int16>* samples, int start, int end, int targetFrequency)
{
	float k = round(float((end - start) * targetFrequency) / sampleFrequency);
	float sumr = 0;
	float sumi = 0;
	for (int n = start; n < end; n++) {
		float angle = ((-2 * PI * n) / (end - start)) * k;
		sumr += (*samples)[n] * cos(angle);
		sumi += (*samples)[n] * sin(angle);
	}
	return sqrt(pow(sumr, 2.0) + pow(sumi, 2.0));
}
