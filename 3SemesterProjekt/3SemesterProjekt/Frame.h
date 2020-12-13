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
	TOKENPASS = 0x4,
	TOKENPASSACK = 0x5,
	ALIVE = 0x6,
	CLOSE = 0x7,
	DATA0 = 0xE,
	DATA1 = 0xF,
};

using namespace std;
class Frame
{
public:
	const char PREAMBLE[7] = { 0xF, 0xE, 0xD, 0xC };

	Frame();

	TransmissionType getType();
	vector<char> getData();

	void sendFrame(TransmissionType transmissionType); // Clears data
	void sendFrame(TransmissionType transmissionType, vector<char> dataTones);

	bool wait(int timeout); // Timeout in millis, returns successful

	Timer* getLastActive(); // Returns milliseconds since last frame was recieved
private:
	
	TransmissionType transmissionType = NONE;
	vector<char> dataTones = vector<char>();
	DTMF* dtmf;
	Timer* lastActive;

	void send(); // Send data stored in frame
	
	unsigned short crc16(vector<char> data);
};

