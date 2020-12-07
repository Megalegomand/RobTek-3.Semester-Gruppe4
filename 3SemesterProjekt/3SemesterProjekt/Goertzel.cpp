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

float Goertzel::processSamples(deque<Int16> samples, int start, int end, int targetFrequency)
{
	//cout << "---------------------------------- Goertzel" << endl;
	Timer t = Timer();
	t.start();
	float k = round(float((end - start) * targetFrequency) / sampleFrequency);
	float sumr = 0;
	float sumi = 0;
	float angleC = -2 * PI * k / (end - start);
	//cout << "t" << t.elapsedMillis() << endl;
	for (int n = 0; n < end - start; n++) {
		float angle =  n * angleC;
		sumr += samples[n] * cos(angle);
		sumi += samples[n] * sin(angle);
	}
	//cout << "t" << t.elapsedMillis() << endl;
	return sqrt(sumr*sumr + sumi*sumi);
}
