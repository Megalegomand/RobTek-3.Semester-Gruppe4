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
	const int SAMPLE_RATE = 44100;

	const int TONE_DURATION = 100; // Millisseconds
	const unsigned AMPLITUDE = 10000;

	const int TONES_L[4] = { 697,  770,  852,  941 };  // DTMF low tones
	const int TONES_H[4] = { 1209, 1336, 1477, 1633 }; // DTMF high tones

	const int SNR_DB_THRESHHOLD = 10;

	const int TONE_SAMPLES = ((SAMPLE_RATE * TONE_DURATION) / 1000);
	const double PI = 3.14159265359;

	DTMF();

	void sendSequence(vector<char>& sequence);
	vector<char> listenSequence(int timeout);

	~DTMF();

private:
	vector<sf::Int16>* preparedTones[16];

	sf::Sound sound;

	Goertzel* goertzel;

	// Samples from recorder
	queue<Int16> inputSamples;
	mutex inputSamples_mutex;

	// How much should the sample move while searching
	int searchSampleMove = 500;
	mutex searchSampleMove_mutex;

	// How many times has the current input samples been processed
	// Used to calculate sampleMove
	int processCounter = 1;
	mutex processCounter_mutex;

	float goertzelresH[4];
	float goertzelresL[4];

	// Precalculate tone samples
	void prepareTones();

	// Determine tone from samples by analysing samples from start to end
	char determineDTMF(deque<Int16>* samples, int start, int end);
	// Move samples amount by inserting from input samples
	// Tone size is maintained
	void moveSamples(deque<Int16>* tone, int amount);
	// Determine whether to move samples left or right
	int syncMove(deque<Int16>* toneSamples, char tone);

protected:
	virtual bool onProcessSamples(const Int16* samples, std::size_t sampleCount); // From SoundRecorder
};

