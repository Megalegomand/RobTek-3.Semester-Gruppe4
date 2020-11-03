#pragma once
#include<concurrent_queue.h>
#include<vector>
#include<iostream>
#include "DTMF.h"
// Temp
#include "VirtuelDTMF.h"

enum TransmissionType {
	BIND = 0x0,
    CONNECT = 0x1,
	ACK = 0x2,
	NACK = 0x3,
	TOKEN_PASS = 0x4,
	DATA = 0xF,
};

enum class TransmissionStates {
	NotConnected, 
	Ready, 
	Wait,
};

using namespace std;
using namespace concurrency;
class DataLink
{
public:
	DataLink();
	

	bool listen(int timeout); // Timeout in ms
	bool bind(int attempts);

	bool sendData(vector<char> &data); // Returns true if succesfull
private:
	const char PREAMBLE[8] = { 0xF, 0xA, 0x5, 0x0, 0xF, 0xA, 0x5, 0x0 };
	const char SFD = 0xF;

	concurrent_queue<char> recieveBuffer;
	concurrent_queue<char> sendBuffer;

	VirtuelDTMF* dtmf;

	void sendTone(char tone);
	char receiveTone(); // Returns -1 if no tone available

	void sendFrame(TransmissionType transmissionType, vector<char> data);
};

/*
Preamble SFD (1tone)
FA50FA50 F   TYPE    DATA Checksum (No transmission)


tone = 100ms


1: Bind
2: ACK

1: Connect
2: ACK
1: Data
2: ACK
1: Token til 2
2: ACK
Time 100000 (
	2: Connect
	1: ACK
)
2: Connect
1: ACK
2: Data
1: ACK
2: Token til 1
1: ACK

*/