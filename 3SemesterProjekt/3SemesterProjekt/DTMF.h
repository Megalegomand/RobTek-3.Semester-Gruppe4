#pragma once
#include<vector>
#include<SFML/Audio.hpp>
#include<iostream>
using namespace std;
class DTMF
{
public:
	DTMF();
	void playDTMF(int tonevalg);
	~DTMF();

private:
	const unsigned AMPLITUDE = 30000;
	const double TWO_PI = 6.28318;
	vector<vector<sf::Int16>> tonesL;
	vector<vector<sf::Int16>> tonesH;
	
};

