#include "DTMF.h"
DTMF::DTMF() {
	// For sendSequence
	prepareTones();

	// Set default microphone
	this->setDevice(getDefaultDevice());
	cout << getDefaultDevice() << endl;

	// Make sure recording is mono
	setChannelCount(1);

	// Setup G�rtzel filter
	goertzel = new Goertzel(SAMPLE_RATE);

	// Start recording, this will fill input samples
	this->start(SAMPLE_RATE);
}

void DTMF::sendSequence(vector<char>& sequence)
{
	// Calculate samples for entire sequence using preparedTones
	vector<Int16> samples = vector<Int16>();
	for (char tone : sequence) {
		// Add margin
		for (int i = 0; i < TONE_MARGIN; i++) {
			samples.push_back(0);
		}

		// Insert tones from prepared tones
		samples.insert(samples.end(), preparedTones[tone]->begin() + TONE_MARGIN, preparedTones[tone]->end() - TONE_MARGIN);
		
		// Add margin
		for (int i = 0; i < TONE_MARGIN; i++) {
			samples.push_back(0);
		}
	}
	
	// Wait for receiver to start recording
	this_thread::sleep_for(chrono::milliseconds(TONE_DURATION));

	// Load sequence samples into buffer
	SoundBuffer buffer = SoundBuffer();
	buffer.loadFromSamples(&samples[0], samples.size(), 1, SAMPLE_RATE);

	// Play sound
	sound.setBuffer(buffer);
	sound.play();

	// Wait for sound completion
	this_thread::sleep_for(chrono::milliseconds(buffer.getDuration().asMilliseconds()));
	sound.stop();
}

vector<char> DTMF::listenSequence(int timeout)
{
	// Timer to keep track of timeout
	Timer startTime = Timer();
	startTime.start();

	// Clear input samples
	inputSamples_mutex.lock();
	inputSamples = queue<Int16>();
	inputSamples_mutex.unlock();

	// Samples currently being analysed
	// Has constant length of TONE_SAMPLES
	deque<Int16> currentTone = deque<Int16>();

	// Wait for input samples
	while (inputSamples.size() < TONE_SAMPLES) {};

	// Fill current tone from input samples
	inputSamples_mutex.lock();
	for (int i = 0; i < TONE_SAMPLES; i++) {
		currentTone.push_back(inputSamples.front());
		inputSamples.pop();
	}
	inputSamples_mutex.unlock();

	// Search for tone
	while (startTime.elapsedMillis() < timeout) {
		// If the whole tone at the last part is detectable, then a tone is ready and synced
		if (determineDTMF(&currentTone, 0, TONE_SAMPLES) != -1 && determineDTMF(&currentTone, 0, TONE_SAMPLES / 2) != -1) {
			break;
		}

		// Move samples by searchSampleMove
		searchSampleMove_mutex.lock();
		int searchSampleMoveC = searchSampleMove; // Copy to keep mutex unlocked
		searchSampleMove_mutex.unlock();
		moveSamples(&currentTone, searchSampleMoveC);
	}

	// Get tones from transmission while syncing
	vector<char> tones = vector<char>();
	char tone = determineDTMF(&currentTone, 0, TONE_SAMPLES); // First tone
	while (tone != -1) {
		// Save tone
		tones.push_back(tone);

		// Determine first and last part of tone
		char f = determineDTMF(&currentTone, 0, TONE_SAMPLES / 2);
		char l = determineDTMF(&currentTone, TONE_SAMPLES / 2, TONE_SAMPLES);
		
		// Calculate direction that needs to be moved
		int dir = (f == tone ? 0 : 1) - (l == tone ? 0 : 1);

		// Move samples a tone and correct for syncronisation
		moveSamples(&currentTone, TONE_SAMPLES + dir * TONE_SAMPLES / 16);

		// Determine the next tone
		tone = determineDTMF(&currentTone, 0, TONE_SAMPLES);
	}
	
	return tones;
}

DTMF::~DTMF() {
	// Stop recording, SFML Requirement
	stop();
}


void DTMF::prepareTones()
{
	// Go through each tone
	for (int tone = 0; tone < 16; tone++) {
		preparedTones[tone] = new vector<sf::Int16>();

		// Increment value dependent on current tone
		double iHigh = ((double)TONES_H[tone % 4]) / SAMPLE_RATE;
		double iLow = ((double)TONES_L[tone / 4]) / SAMPLE_RATE;

		// Start low and high
		double low = 0.0;
		double high = 0.0;

		// Calculate each sample
		for (unsigned int i = 0; i < TONE_SAMPLES; i++) {
			// Sample based on sin
			preparedTones[tone]->push_back(AMPLITUDE * sin(low * 2 * PI) + AMPLITUDE * sin(high * 2 * PI));

			// Increment values
			low += iLow;
			high += iHigh;
		};
	}
}

char DTMF::determineDTMF(deque<Int16>* samples, int start, int end)
{
	// Calculate Goertzel for DTMF high and low tones
	float goertzelH[4];
	float goertzelL[4];
	for (int i = 0; i < 4; i++) {
		goertzelH[i] = goertzel->processSamples(samples, start + TONE_MARGIN * 2, end - TONE_MARGIN * 2, TONES_H[i]);
		goertzelL[i] = goertzel->processSamples(samples, start + TONE_MARGIN * 2, end - TONE_MARGIN * 2, TONES_L[i]);
	}

	// Position for calculating DTMF tone
	int posL = 0;
	int posH = 0;

	// Signal strenght
	float signalH = 0.0;
	float signalL = 0.0;

	// Sum of every other tone
	float noise_sum = 0.0;

	// Go through each tone and find the two largest tones
	// and the sum of the rest, used as noise
	for (int i = 0; i < 4; i++)
	{
		// Get largest tone and define it as signal
		if (goertzelL[i] > signalL)
		{
			// If a higher tone is found, the previous is added to noise
			noise_sum += signalL; 
			signalL = goertzelL[i];
			posL = i;
		}
		else {
			noise_sum += goertzelL[i];
		}

		// Same code as high tones
		if (goertzelH[i] > signalH)
		{
			noise_sum += signalH;
			signalH = goertzelH[i];
			posH = i;
		}
		else {
			noise_sum += goertzelH[i];
		}
	}

	// Calculate average
	float signal_avg = (signalH + signalL) / 2;
	float noise_avg = noise_sum / 6.0;

	// Calculate SNR
	float SNR = signal_avg / noise_avg;
	
	// Return DTMF tone or -1 if threshold is not reached
	if (SNR > SNR_THRESHHOLD)
	{
		return  posL * 4 + posH;
	}
	else
	{
		return -1;
	}
}

void DTMF::moveSamples(deque<Int16>* tone, int amount)
{
	// Wait samples until enough samples is recorded
	while (amount > inputSamples.size()) {}

	// Move samples from inputSamples into tone
	inputSamples_mutex.lock();
	for (int i = 0; i < amount; i++) {
		tone->push_back(inputSamples.front());
		inputSamples.pop();
		tone->pop_front();
	}
	inputSamples_mutex.unlock();

	// This counts as one processing of samples
	processCounter_mutex.lock();
	processCounter++;
	processCounter_mutex.unlock();
}

bool DTMF::onProcessSamples(const Int16* samples, std::size_t sampleCount)
{
	// Move samples from the recorded samples into input samples
	inputSamples_mutex.lock();
	for (int i = 0; i < sampleCount; i++) {
		inputSamples.push(samples[i]);
	}
	inputSamples_mutex.unlock();

	// Calculate searchSampleMove from the size of input samples and processCounter
	processCounter_mutex.lock();
	searchSampleMove_mutex.lock();
	searchSampleMove = inputSamples.size() / processCounter;
	processCounter = 1;
	processCounter_mutex.unlock();

	// Make sure the sample move isn't so large that tones can't be found
	if (searchSampleMove > TONE_SAMPLES / 4) {
		searchSampleMove = TONE_SAMPLES / 4;
	}
	searchSampleMove_mutex.unlock();

	// Limit the size of inputSamples to INPUT_SAMPLES_MAX_SIZE
	inputSamples_mutex.lock();
	while (inputSamples.size() > INPUT_SAMPLES_MAX_SIZE) {
		inputSamples.pop();
	}
	inputSamples_mutex.unlock();

	return true;
};
