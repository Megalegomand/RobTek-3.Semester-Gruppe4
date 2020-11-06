#include "VirtuelDTMF.h"

char VirtuelDTMF::medium = -1;
mutex VirtuelDTMF::medium_mutex;

void VirtuelDTMF::sendSequence(vector<char>& sequence, int duration) {
	for (char tone : sequence) {
		sendTone(tone, duration);
	}
}

void VirtuelDTMF::sendTone(char tone, int duration)
{
	medium_mutex.lock();
	medium = tone;
	medium_mutex.unlock();
	this_thread::sleep_for(chrono::milliseconds(duration));
	medium_mutex.lock();
	medium = -1;
	medium_mutex.unlock();
}

char VirtuelDTMF::listenTone(int duration)
{
	medium_mutex.lock();
	char oldTone = medium;
	medium_mutex.unlock();
	//cout << "Tone old " << int(oldTone) << endl;
	this_thread::sleep_for(chrono::milliseconds(duration));
	medium_mutex.lock();
	char newTone = medium;
	medium_mutex.unlock();
	//cout << "Tone new " << int(newTone) << endl;
	if (oldTone != newTone) {
		return -1;
	}
	return newTone;
}

void VirtuelDTMF::outputMedium()
{
	for (int i = 0; i < 20; i++) {
	//while (true) {
		medium_mutex.lock();
		cout << int(medium) << endl;
		medium_mutex.unlock();
		this_thread::sleep_for(chrono::milliseconds(TONE_DURATION/10));
	}
}

