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
	const unsigned AMPLITUDE = 3000;
	const double TWO_PI = 6.28318;
	const double tonesL[4] = { 697,770,852,941 };
	const double tonesH[4] = { 1209,1336,1477,1633 };
	
};

