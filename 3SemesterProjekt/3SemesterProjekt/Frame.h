#pragma once
#include<vector>
#include"DTMF.h"
#include"Timer.h"

// All diferent types of transmission with tone value
// NONE has a non existent tone
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
	const char PREAMBLE[4] = { 0xF, 0xE, 0xD, 0xC }; // Tones in preamble

	Frame();

	TransmissionType getType(); // Get transmission type
	vector<char> getData(); // Gets data as bytes, not tones

	// Sends a frame witht type and data
	void sendFrame(TransmissionType transmissionType); // Clears data
	void sendFrame(TransmissionType transmissionType, vector<char> dataTones);

	// Wait for a frame to arrive and update internal values with those from the frame
	bool wait(int timeout); // Timeout in millis, returns successful

	// Timer object from when the last frame was send or received.
	Timer* getLastActive();
private:
	TransmissionType transmissionType = NONE;
	vector<char> dataTones = vector<char>(); // Contains tones, not bytes

	DTMF* dtmf;
	Timer* lastActive;

	// Send data stored in frame
	void send(); 
	
	// Calculate CRC16 checksum
	unsigned short crc16(vector<char> data);
};
