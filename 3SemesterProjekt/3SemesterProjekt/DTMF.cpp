#include "DTMF.h"
DTMF::DTMF() {
	// For sendSequence
	prepareTones();

	// Set default microphone
	this->setDevice(getDefaultDevice());
	cout << getDefaultDevice() << endl;
	// Make sure recording is mono
	setChannelCount(1);

	goertzel = new Goertzel(SAMPLE_RATE);

	// Start recording, this will fill input samples
	this->start(SAMPLE_RATE);
}

void DTMF::sendSequence(vector<char>& sequence)
{
	// Calculate samples for entire sequence using preparedTones
	vector<sf::Int16> samples = vector<sf::Int16>();
	for (char tone : sequence) {
		samples.insert(samples.end(), preparedTones[tone]->begin(), preparedTones[tone]->end());
	}

	// Load sequence samples into buffer
	sf::SoundBuffer buffer;
	buffer.loadFromSamples(&samples[0], samples.size(), 1, SAMPLE_RATE);

	// Play sound
	sound.setBuffer(buffer);
	sound.play();
	// Wait for sound completion
	this_thread::sleep_for(chrono::milliseconds(buffer.getDuration().asMilliseconds()));
}

vector<char> DTMF::listenSequence(int timeout)
{
	// Timer to keep stop at timeout
	Timer startTime = Timer();
	startTime.start();

	// Clear input samples
	inputSamples_mutex.lock();
	inputSamples = queue<Int16>();
	inputSamples_mutex.unlock();

	// Current tone samples
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
		// If last part of tone is received, then a tone is ready and synced
		if (determineDTMF(&currentTone, 0, TONE_SAMPLES / 2) != -1) {
			break;
		}

		// Move samples by searchSampleMove
		searchSampleMove_mutex.lock();
		int searchSampleMoveC = searchSampleMove;
		searchSampleMove_mutex.unlock();
		moveSamples(&currentTone, searchSampleMoveC);
	}

	// Get tones in transmission
	vector<char> tones = vector<char>();
	char tone = determineDTMF(&currentTone, 0, TONE_SAMPLES); // First tone
	while (tone != -1) {
		tones.push_back(tone);

		// Move samples a tone and correct for syncronisation
		moveSamples(&currentTone, TONE_SAMPLES + syncMove(&currentTone, tone) * TONE_SAMPLES / 8);
		tone = determineDTMF(&currentTone, 0, TONE_SAMPLES);
	}

	// Debug
	cout << "Recieved" << endl;
	for (char c : tones) {
		cout << (int)c << endl;
	}

	return tones;
}

DTMF::~DTMF() {
	// Stop recording
	stop();
}


void DTMF::prepareTones()
{
	// Go through each tone
	for (int tone = 0; tone < 16; tone++) {
		preparedTones[tone] = new vector<sf::Int16>();

		// Increment value dependent on current tone
		const double iHigh = ((double)TONES_H[tone % 4]) / SAMPLE_RATE;
		const double iLow = ((double)TONES_L[tone / 4]) / SAMPLE_RATE;

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
		goertzelH[i] = goertzel->processSamples(samples, start, end, TONES_H[i]);
		goertzelL[i] = goertzel->processSamples(samples, start, end, TONES_L[i]);
	}
	// ------------------------------------------
	int pos1 = 0, pos2 = 0;
	float  largest = 0, second_largest = 0;
	int res = 0;
	//finding Largest second largset
	for (int i = 0; i < 4; i++)
	{
		if (goertzelL[i] > largest)
		{
			largest = goertzelL[i];
			pos1 = i;
		}
		if (goertzelH[i] > second_largest)
		{
			second_largest = goertzelH[i];
			pos2 = i;

		}
	}
	float P_Signal = (largest + second_largest) / 2;

	float sum = 0.0;
	for (int i = 0; i < 4; i++)
	{
		if (i != pos1) {
			sum += goertzelL[i];
		}
		if (i != pos2) {
			sum += goertzelH[i];
		}
	}
	float P_stoej = sum / 6.0;

	float SNR = P_Signal / P_stoej;
	cout << SNR << endl;

	// ------------------------------------------
	float signalH = 0.0;
	float signalL = 0.0;
	float noise_sum = 0.0;
	//finding Largest second largset
	for (int i = 0; i < 4; i++)
	{
		if (goertzelL[i] > signalL)
		{
			noise_sum += signalL;
			signalL = goertzelL[i];
		}
		else {
			noise_sum += goertzelL[i];
		}

		if (goertzelH[i] > signalH)
		{
			noise_sum += signalH;
			signalH = goertzelH[i];
		}
		else {
			noise_sum += goertzelH[i];
		}
	}
	float signal_avg = (signalH + signalL) / 2;

	P_stoej = noise_sum / 6.0;

	SNR = signal_avg / P_stoej;
	cout << SNR << endl;

	if (SNR > SNR_DB_THRESHHOLD)
	{
		//cout << SNR << endl;
		return  pos1 * 4 + pos2;
	}
	else
	{
		//cout << SNR << endl;
		return -1;
	}
}

void DTMF::moveSamples(deque<Int16>* tone, int amount)
{
	while (amount > inputSamples.size()) {} // Wait samples

	inputSamples_mutex.lock();
	for (int i = 0; i < amount; i++) { // Make sure there is still samples available
		tone->push_back(inputSamples.front());
		inputSamples.pop();
		tone->pop_front();
	}
	inputSamples_mutex.unlock();

	processCounter_mutex.lock();
	processCounter++;
	processCounter_mutex.unlock();
}

int DTMF::syncMove(deque<Int16>* toneSamples, char tone)
{
	int ret = (determineDTMF(toneSamples, 0, TONE_SAMPLES / 2) == tone ? 0 : 1);
	return ret - (determineDTMF(toneSamples, TONE_SAMPLES / 2, TONE_SAMPLES) == tone ? 0 : 1);
}








bool DTMF::onProcessSamples(const Int16* samples, std::size_t sampleCount)
{
	inputSamples_mutex.lock();
	for (int i = 0; i < sampleCount; i++) {
		inputSamples.push(samples[i]);
	}
	inputSamples_mutex.unlock();

	processCounter_mutex.lock();
	searchSampleMove_mutex.lock();
	searchSampleMove = inputSamples.size() / processCounter;
	processCounter = 1;
	processCounter_mutex.unlock();

	if (searchSampleMove > TONE_SAMPLES / 4) {
		//cout << "SMBefore" << sampleMove << endl;
		//cout << TONE_SAMPLES << ":" << TONE_SAMPLES / 4 << endl;
		searchSampleMove = TONE_SAMPLES / 4;
	}
	searchSampleMove_mutex.unlock();

	inputSamples_mutex.lock();
	while (inputSamples.size() > INPUT_SAMPLES_MAX_SIZE) {
		inputSamples.pop();
	}
	inputSamples_mutex.unlock();

	return true;
}
;
