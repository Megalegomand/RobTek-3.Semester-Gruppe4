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
}

void DTMF::prepareTones()
{
	for (int tonei = 0; tonei < 16; tonei++) {
		preparedTones[tonei] = new vector<sf::Int16>();

		const double incrementL = ((double)TONES_L[tonei / 4]) / SAMPLE_RATE;
		const double incrementH = ((double)TONES_H[tonei % 4]) / SAMPLE_RATE;

		double x = 0;
		double y = 0;

		int TONE_SAMPLES = ((SAMPLE_RATE * TONE_DURATION) / 1000);

		for (unsigned int i = 0; i < TONE_SAMPLES; i++) {
			preparedTones[tonei]->push_back(AMPLITUDE * sin(x * 2 * PI) + AMPLITUDE * sin(y * 2 * PI));
			x += incrementL;
			y += incrementH;
		};
	}
}

int DTMF::syncMove(deque<Int16>* toneSamples, char tone)
{
	int ret = (determineDTMF(toneSamples, 0, TONE_SAMPLES / 2) == tone ? 0 : 1);
	return ret - (determineDTMF(toneSamples, TONE_SAMPLES / 2, TONE_SAMPLES) == tone ? 0 : 1);
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

void DTMF::sendSequence(vector<char>& sequence)
{
	vector<sf::Int16> samples = vector<sf::Int16>();
	for (char tone : sequence) {
		samples.insert(samples.end(), preparedTones[tone]->begin(), preparedTones[tone]->end());
	}

	sf::SoundBuffer buffer;

	buffer.loadFromSamples(&samples[0], samples.size(), 1, SAMPLE_RATE);

	sound.setBuffer(buffer);
	sound.play();
	this_thread::sleep_for(chrono::milliseconds(buffer.getDuration().asMilliseconds()));
}

vector<char> DTMF::listenSequence(int duration)
{
	this->start(SAMPLE_RATE); // Start recording
	Timer startTime = Timer();
	startTime.start();

	inputSamples = queue<Int16>();

	deque<Int16> currentTone = deque<Int16>();

	// Fill tone buffer
	while (inputSamples.size() < TONE_SAMPLES) {};
	inputSamples_mutex.lock();
	for (int i = 0; i < TONE_SAMPLES; i++) {
		currentTone.push_back(inputSamples.front());
		inputSamples.pop();
	}
	inputSamples_mutex.unlock();

	// Sync
	while (startTime.elapsedMillis() < duration) {

		if (determineDTMF(&currentTone, 0, TONE_SAMPLES / 2) != -1) {
			break;
		}

		searchSampleMove_mutex.lock();
		int sampleMoveC = searchSampleMove;
		searchSampleMove_mutex.unlock();
		moveSamples(&currentTone, sampleMoveC);
	}

	// Get tones
	//cout << "-----------------" << endl;
	vector<char> tones = vector<char>();
	char tone = determineDTMF(&currentTone, 0, TONE_SAMPLES);
	while (tone != -1) {
		tones.push_back(tone);
		//cout << "Tone: " << int(tone) << ":" << (int) determineDTMF(&currentTone, TONE_SAMPLES / 2, TONE_SAMPLES) << ":" << (int) determineDTMF(&currentTone, 0, TONE_SAMPLES / 2) << endl;
		//cout << "SM" << syncMove(&currentTone, tone) << endl;
		moveSamples(&currentTone, TONE_SAMPLES + syncMove(&currentTone, tone) * TONE_SAMPLES / 8);
		tone = determineDTMF(&currentTone, 0, TONE_SAMPLES);
	}
	//cout << "Tone: " << int(tone) << ":" << (int)determineDTMF(&currentTone, TONE_SAMPLES / 2, TONE_SAMPLES) << ":" << (int)determineDTMF(&currentTone, 0, TONE_SAMPLES / 2) << endl;
	//cout << "SM" << syncMove(&currentTone, tone) << endl;

	cout << "Recieved" << endl;
	for (char c : tones) {
		cout << (int)c << endl;
	}
	this->stop();
	return tones;
}

char DTMF::determineDTMF(deque<Int16>* samples, int start, int end)
{
	for (int i = 0; i < 4; i++) {
		goertzelresH[i] = goertzel->processSamples(samples, start, end, TONES_H[i]);
		goertzelresL[i] = goertzel->processSamples(samples, start, end, TONES_L[i]);
	}

	int pos1 = 0, pos2 = 0;
	float  largest = 0, second_largest = 0;
	int res = 0;
	//finding Largest second largset
	for (int i = 0; i < 4; i++)
	{
		if (goertzelresL[i] > largest)
		{
			largest = goertzelresL[i];
			pos1 = i;
		}
		if (goertzelresH[i] > second_largest)
		{
			second_largest = goertzelresH[i];
			pos2 = i;

		}
	}
	float P_Signal = (largest + second_largest) / 2;

	float sum = 0.0;
	for (int i = 0; i < 4; i++)
	{
		if (i != pos1) {
			sum += goertzelresL[i];
		}
		if (i != pos2) {
			sum += goertzelresH[i];
		}
	}
	float P_stoej = sum / 6.0;

	float SNR = P_Signal / P_stoej;

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

DTMF::~DTMF() {
	stop();
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

	return true;
}
;
