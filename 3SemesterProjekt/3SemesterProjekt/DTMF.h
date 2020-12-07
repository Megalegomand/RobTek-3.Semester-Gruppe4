#pragma once
#include<vector>
#include<SFML/Audio.hpp>
#include<iostream>
#include<chrono>
#include<thread>
#include<math.h>
#include<complex>
#include<valarray>
#include "Goertzel.h"
#include<SFML/Audio/SoundRecorder.hpp>
#include<queue>
#include<mutex>
#include"Timer.h"
#include<deque>

using namespace std;
using namespace sf;
class DTMF : public SoundRecorder
{
public:
	const int TONE_DURATION = 20; // Millisseconds
	const unsigned AMPLITUDE = 10000;
	const double PI = 3.14159265359;
	const int TONES_L[4] = { 697,  770,  852,  941 };
	const int TONES_H[4] = { 1209, 1336, 1477, 1633 };
	const int SAMPLE_RATE = 44100;
	const int TONE_SAMPLES = ((SAMPLE_RATE * TONE_DURATION) / 1000);

	DTMF();
	//void sendTone(char tonevalg, int duration);
	void sendSequence(vector<char>& sequence);


	vector<char> listenSequence(int timeout);

	char listenTone(int duration); // Listen time, return -1 for no DTMF

	
	char receiveDTMF(int duration);
	char determineDTMF(vector<float>goertzelresL,vector<float>goertzalresH);

	char determineDTMF(deque<Int16> samples, int start, int end);

	void prepareTones(int duration);
	
	~DTMF();

private:
	vector<sf::Int16>* preparedTones[16];

	sf::Sound sound;

	queue<Int16> inputSamples;
	mutex inputSamples_mutex;
	Goertzel* goertzel;

	int sampleMove = 500;
	mutex sampleMove_mutex;

	int processCounter = 1;
	mutex processCounter_mutex;

	vector<float> goertzelresH = vector<float>();
	vector<float> goertzelresL = vector<float>();

	Goertzel* goertzelL[4];
	Goertzel* goertzelH[4];

	bool values[8] = {};
	int DBthreshhold = 10;
	typedef complex<double> Complex;
	int N = 205; // forklaring til hvorfor vi bruger lige denne N sampling
	std::vector<std::string> availableDevices = SoundRecorder::getAvailableDevices();
	std::string inputDevice = availableDevices[0];

	void prepareTones();
	
protected:
	virtual bool onProcessSamples(const Int16* samples, std::size_t sampleCount);
};

