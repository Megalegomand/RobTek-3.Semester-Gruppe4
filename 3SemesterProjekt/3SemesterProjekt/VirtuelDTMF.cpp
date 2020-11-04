#include "VirtuelDTMF.h"

char VirtuelDTMF::medium = -1;
mutex VirtuelDTMF::medium_mutex;

void VirtuelDTMF::sendSequence(vector<char>& sequence) {
	for (char tone : sequence) {
		sendTone(tone);
	}
}

void VirtuelDTMF::sendTone(char tone)
{
	medium_mutex.lock();
	medium = tone;
	medium_mutex.unlock();
	this_thread::sleep_for(chrono::milliseconds(TONE_DURATION));
	medium_mutex.lock();
	medium = -1;
	medium_mutex.unlock();
}

char VirtuelDTMF::listenTone()
{
	char oldTone = medium;
	this_thread::sleep_for(chrono::milliseconds(LISTEN_DURATION));
	char newTone = medium;
	if (oldTone != newTone) {
		return -1;
	}
	return newTone;
}

void VirtuelDTMF::outputMedium()
{
	for (int i = 0; i < 20; i++) {
	//while (true) {
		cout << int(medium) << endl;
		this_thread::sleep_for(chrono::milliseconds(TONE_DURATION));
	}
}

