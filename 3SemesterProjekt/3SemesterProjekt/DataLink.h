#pragma once
#include<concurrent_queue.h>
#include<vector>
#include<iostream>
#include"DTMF.h"
#include"Frame.h"

// Temp
#include"VirtuelDTMF.h"

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
	vector<char> waitData(int timeout);
private:
	Frame* frame;
};

/*
Preamble SFD (1tone)
FA50FA50 0   TYPE    DATA Checksum (No transmission)


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