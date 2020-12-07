#include "Goertzel.h"

Goertzel::Goertzel()
{
}

Goertzel::Goertzel(int targetFrequency, int sampleFrequency)
{
	this->targetFrequency = targetFrequency;
	this->sampleFrequency = sampleFrequency;
}

Goertzel::Goertzel(int sampleFrequency)
{
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

float Goertzel::processSamples(deque<Int16>* samples, int start, int end, int targetFrequency)
{
	float k = round(float((end - start) * targetFrequency) / sampleFrequency);
	float c = 2 * cos((2 * PI / (end - start)) * k);

	float q0 = 0.0;
	float q1 = 0.0;
	float q2 = 0.0;
	for (int n = start; n < end; n++) {
		q0 = c * q1 - q2 + (*samples)[n];
		q2 = q1;
		q1 = q0;
	}
	return sqrt(q1 * q1 + q2 * q2 - q1 * q2 * c);
}
