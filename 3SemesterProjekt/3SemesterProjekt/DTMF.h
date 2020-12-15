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
	const int SAMPLE_RATE = 44100; // For playing and recording

	const int TONE_DURATION = 100; // Millisseconds
	const unsigned AMPLITUDE = 5000; // Amplitude of tones, can't be too high since tones will be distorted

	const int TONE_MARGIN = 200; // Margin between tones, this value is double for listening
	const int SNR_THRESHHOLD = 10;

	const int TONE_SAMPLES = ((SAMPLE_RATE * TONE_DURATION) / 1000);
	const double PI = 3.14159265359;

	// This will keep a min transmission - 1, hence it wont be able to read itself
	const unsigned int INPUT_SAMPLES_MAX_SIZE = TONE_SAMPLES * 6; 

	const int TONES_L[4] = { 697,  770,  852,  941 };  // DTMF low tones
	const int TONES_H[4] = { 1209, 1336, 1477, 1633 }; // DTMF high tones

	DTMF();

	// Send sequence of DTMF tones
	void sendSequence(vector<char>& sequence);

	// Listen for DTMF tone until timeout is reached
	// Return DTMF tone or -1 if no tone was found
	vector<char> listenSequence(int timeout); // Timeout in millis

	// Destructor for SFML
	~DTMF();

private:
	// Precalculated tones
	vector<sf::Int16>* preparedTones[16];

	sf::Sound sound;

	Goertzel* goertzel;

	// Samples from recorder
	queue<Int16> inputSamples = queue<Int16>();
	mutex inputSamples_mutex;

	// How much should the sample move while searching
	int searchSampleMove = 500;
	mutex searchSampleMove_mutex;

	// How many times has the current input samples been processed
	// Used to calculate sampleMove
	int processCounter = 1;
	mutex processCounter_mutex;

	// Precalculate tone samples
	void prepareTones();

	// Determine tone from samples by analysing samples from start to end
	char determineDTMF(deque<Int16>* samples, int start, int end);

	// Move samples amount by inserting from input samples
	// Tone size is maintained
	void moveSamples(deque<Int16>* tone, int amount);

protected:
	// From SoundRecorder
	virtual bool onProcessSamples(const Int16* samples, std::size_t sampleCount); 
};

