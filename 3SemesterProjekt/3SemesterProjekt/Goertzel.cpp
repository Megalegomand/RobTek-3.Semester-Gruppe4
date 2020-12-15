#include "Goertzel.h"

Goertzel::Goertzel() {}


Goertzel::Goertzel(int sampleFrequency)
{
	this->sampleFrequency = sampleFrequency;
}

float Goertzel::processSamples(deque<Int16>* samples, int start, int end, int targetFrequency)
{
	// Calculate k
	float k = round(float((end - start) * targetFrequency) / sampleFrequency);

	// Real and imginary sums
	float sumr = 0;
	float sumi = 0;

	// Run throuh each sample
	for (int n = start; n < end; n++) {
		// Calc angle
		float angle = ((-2 * PI * n) / (end - start)) * k;

		// Add real and img 
		sumr += (*samples)[n] * cos(angle);
		sumi += (*samples)[n] * sin(angle);
	}

	// Return magnitude
	return sqrt(pow(sumr, 2.0) + pow(sumi, 2.0));
}
