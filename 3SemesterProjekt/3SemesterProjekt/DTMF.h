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
	const int TONE_DURATION = 1000; // Millisseconds
	const int LISTEN_DURATION = 10; // Duration of microlisten

	DTMF();
	void sendTone(char tonevalg);
	void sendSequence(vector<char>& sequence);
	char listenTone(); // Listen time, return -1 for no DTMF


	
	char receiveDTMF();
	char determineDTMF(vector<float>goertzelresL,vector<float>goertzalresH);
	
	~DTMF();

private:
	const unsigned AMPLITUDE = 300;
	const double PI = 3.14159265359;
	const int tonesL[4] = { 697,  770,  852,  941  };
	const int tonesH[4] = { 1209, 1336, 1477, 1633 };
	Goertzel* goertzelL[4];
	Goertzel* goertzelH[4];
	bool values[8] = {};
	int DBthreshhold = 0;
	typedef complex<double> Complex;
	int SAMPLING_RATE = 44100; // Burde s�ttes til v�rdien af recorder.getSampleRate()
	int N = 205; // forklaring til hvorfor vi bruger lige denne N sampling

};

