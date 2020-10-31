#include "Goertzel.h"

Goertzel::Goertzel()
{
}

Goertzel::Goertzel(int targetFrequency, int sampleFrequency)
{
	this->targetFrequency = targetFrequency;
	this->sampleFrequency = sampleFrequency;
}

float Goertzel::processSamples(const sf::Int16 *samples, int N)
{
	float k = round(float(N * targetFrequency) / sampleFrequency);
	float sumr = 0;
	float sumi = 0;
	for (int n = 0; n < N; n++) {
		float angle = -2 * PI * n * k / N;
		sumr += samples[n] * cos(angle);
		sumi += samples[n] * sin(angle);
	}
	return sqrt(pow(sumr, 2.0) + pow(sumi, 2.0));
}
