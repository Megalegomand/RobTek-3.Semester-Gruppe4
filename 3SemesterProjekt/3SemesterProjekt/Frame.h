#pragma once
#include<vector>
#include"DTMF.h"
#include"VirtuelDTMF.h"
#include"Timer.h"

enum TransmissionType {
	BIND = 0x0,
	CONNECT = 0x1,
	ACK = 0x2,
	NACK = 0x3,
	TOKEN_PASS = 0x4,
	ALIVE = 0x5,
	DATA = 0xF,
};

using namespace std;
class Frame
{
public:
	const int TONE_DURATION = 100; // Millisseconds
	const int LISTEN_MARGIN = 4 * TONE_DURATION / 10; // Margin applied on each side of the tone, to reduce noise
	const int LISTEN_DURATION = 10; // Listen duration to catch the first tone

	Frame();

	TransmissionType getType();
	vector<char> getData();

	void setFrame(TransmissionType transmissionType); // Clears data
	void setFrame(TransmissionType transmissionType, vector<char> data);

	void send();
	bool wait(int timeout); // Timeout in millis, returns successful

	Timer* getLastActive(); // Returns milliseconds since last frame was recieved
private:
	const char PREAMBLE[4] = { 0xF, 0xA, 0x5, 0x0 };

	TransmissionType transmissionType;
	vector<char> data;
	VirtuelDTMF* dtmf;
	Timer* lastActive;

	char nextTone(Timer* timer, int toneNum);
};

