#pragma once
#include<concurrent_queue.h>
#include<vector>
#include<iostream>
#include"DTMF.h"
#include"Frame.h"
#include<mutex>
#include <functional>

// Temp
#include"VirtuelDTMF.h"

enum class TransmissionState {
	NotConnected, 
	Token, // Has token, is able to send data along
	Waiting, // Waiting for data and token
};

using namespace std;
using namespace placeholders;
using namespace concurrency;
class DataLink
{
public:
	const int BIND_WAIT_MIN = 10;
	const int BIND_WAIT_DIFF = 500;
	const int MAX_LOSS_CONNECTION = 1000; // Maximum loss of connection, sync will be send at half this time
	const int ATTEMPTS = 10; // Attempts at sending before termination, does not account for bind
	const int LISTEN_TIME = 1000;

	DataLink();
	DataLink(function<void(TransmissionType, vector<char>)> dataReady, function<void()> tokenPass);
	//DataLink(const DataLink&);

	bool bind(int attempts);

	bool sendData(vector<char> &data); // Returns true if succesfull
	//vector<char> waitData(int timeout);
	bool passToken(); // Pass token, return false if pass unsuccesful (Waiting or Conection failed)
private:
	Frame* frame;
	mutex frame_mutex;
	TransmissionState state = TransmissionState:: NotConnected;
	thread* connected_thread;

	function<void(TransmissionType, vector<char>)> dataReady;
	function<void()> tokenPass;

	bool sendWaitACK(TransmissionType type, vector<char> data); // Sends data and waits for ACK, tries ATTEMPTS times
	bool sendWaitACK(TransmissionType type);					  // Returns false if connection is lost
	void connectedRun();
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