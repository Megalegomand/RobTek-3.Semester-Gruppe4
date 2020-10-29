#pragma once
#include<vector>
#include<SFML/Audio.hpp>
#include<iostream>
#include<chrono>
#include<thread>
#include<math.h>
#include<complex>
#include<valarray>
#include<complex.h>


using namespace std;
class DTMF
{
public:
	DTMF();
	void playDTMF(int tonevalg);
	void receiveDTMF();
	~DTMF();

private:
	const unsigned AMPLITUDE = 300;
	const double PI = 3.14159265359;
	const double tonesL[4] = { 697.,770.,852.,941. };
	const double tonesH[4] = { 1209.,1336.,1477.,1633. };
	typedef complex<double> Complex;
	
};

