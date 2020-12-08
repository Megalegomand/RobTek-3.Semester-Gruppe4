#pragma once
#include<vector>
#include"DTMF.h"
#include"VirtuelDTMF.h"
#include"Timer.h"

enum TransmissionType {
	NONE = 0xFF,
	BIND = 0x0,
	ACK = 0x2,
	NACK = 0x3,
	TOKEN_PASS = 0x4,
	ALIVE = 0x5,
	CLOSE = 0xE,
	DATA = 0xF,
};

using namespace std;
class Frame
{
public:
	const char PREAMBLE[4] = { 0xF, 0xA, 0x5, 0x0 };

	Frame();

	TransmissionType getType();
	vector<char> getData();

	void sendFrame(TransmissionType transmissionType); // Clears data
	void sendFrame(TransmissionType transmissionType, vector<char> data);

	bool wait(int timeout); // Timeout in millis, returns successful

	Timer* getLastActive(); // Returns milliseconds since last frame was recieved
private:
	
	TransmissionType transmissionType = NONE;
	vector<char> data;
	DTMF* dtmf;
	Timer* lastActive;

	
	void send(); // Send data stored in frame
};

