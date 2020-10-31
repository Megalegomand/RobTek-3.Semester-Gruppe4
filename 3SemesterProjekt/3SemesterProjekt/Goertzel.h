#pragma once
#include <math.h>
#include<SFML/Audio.hpp>

class Goertzel
{
public:
	Goertzel();
	Goertzel(int targetFrequency, int sampleFrequency);
	float processSamples(const sf::Int16 *samples, int count);

private:
	const double PI = 3.14159265359;
	int targetFrequency;
	int sampleFrequency;
	int k;
};

