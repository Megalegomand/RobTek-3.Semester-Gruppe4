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
	Goertzel();
	Goertzel(int sampleFrequency);
	Goertzel(int targetFrequency, int sampleFrequency);
	float processSamples(const sf::Int16 *samples, int count);
	float processSamples(deque<Int16>* samples, int start, int end, int targetFrequency);

private:
	const double PI = 3.14159265359;
	int targetFrequency;
	int sampleFrequency;
	int k;
};

