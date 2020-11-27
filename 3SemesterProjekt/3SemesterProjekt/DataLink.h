#pragma once
#include<concurrent_queue.h>
#include<vector>
#include<iostream>
#include"DTMF.h"
#include"Frame.h"
#include<mutex>

// Temp
#include"VirtuelDTMF.h"

enum class TransmissionState {
	NotConnected, 
	Token, // Has token, is able to send data along
	Waiting, // Waiting for data and token
};

using namespace std;
using namespace concurrency;
class DataLink
{
public:
	const int BIND_WAIT_MIN = 10;
	const int BIND_WAIT_DIFF = 500;
	const int MAX_LOSS_CONNECTION = 10000; // Maximum loss of connection, sync will be send at half this time
	const int ATTEMPTS = 10; // Attempts at sending before termination, does not account for bind

	DataLink();
	//DataLink(const DataLink&);

	bool bind(int attempts);

	bool sendData(vector<char> &data); // Returns true if succesfull
	vector<char> waitData(int timeout);
	bool passToken(); // Pass token, return false if doesn't have token
private:
	Frame* frame;
	mutex frame_mutex;
	TransmissionState state = TransmissionState:: NotConnected;
	thread* alive_thread;

	void alive();
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