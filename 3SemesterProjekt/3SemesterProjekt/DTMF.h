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
#include "Goertzel.h"


using namespace std;
class DTMF
{
public:
	DTMF();
	void playDTMF(int tonevalg);
	void receiveDTMF();
	void determineDTMF();
	float processSamples(int* samples);

	~DTMF();

private:
	const unsigned AMPLITUDE = 300;
	const double PI = 3.14159265359;
	const int tonesL[4] = { 697,  770,  852,  941  };
	const int tonesH[4] = { 1209, 1336, 1477, 1633 };
	Goertzel* goertzelL[4];
	Goertzel* goertzelH[4];
	vector<float> goertzelresL;
	vector<float> goertzelresH;
	typedef complex<double> Complex;
	int SAMPLING_RATE = 44100; // Burde sættes til værdien af recorder.getSampleRate()
	int N = 205; // forklaring til hvorfor vi bruger lige denne N sampling
	
};

