#pragma once
#include <math.h>
#include<SFML/Audio.hpp>
#include<deque>
#include<iostream>
#include"Timer.h"

using namespace std;
using namespace sf;
class Goertzel
{
public:
	const double PI = 3.14159265359;

	Goertzel();
	Goertzel(int sampleFrequency);

	// Process samples from samples start to to sample end
	float processSamples(deque<Int16>* samples, int start, int end, int targetFrequency);

private:
	int sampleFrequency = 0;
};

