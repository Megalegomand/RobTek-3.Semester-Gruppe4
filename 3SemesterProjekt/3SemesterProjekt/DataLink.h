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
	Token, 
	NotToken,
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
	bool passToken(); // Pass token, return false if doesn't have token
private:
	Frame* frame;
	bool hasToken = false;
};

/*
Preamble SFD (1tone)
FA50   TYPE    DATA Checksum (No transmission)


tone = 100ms


1: Bind
2: ACK

//1: Connect
//2: ACK
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