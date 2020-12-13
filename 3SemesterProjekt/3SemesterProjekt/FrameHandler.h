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
class FrameHandler
{
public:
	const int BIND_WAIT_MIN = 10000;
	const int BIND_WAIT_DIFF = 10000;
	const int ATTEMPTS = 10; // Attempts at sending before termination, does not account for bind
	const int LISTEN_TIME = 5000;
	const int MAX_LOSS_CONNECTION = ATTEMPTS * LISTEN_TIME * 2; // Maximum loss of connection, sync will be send at half this time


	int resend = 0;
	int send = 0;


	FrameHandler();
	FrameHandler(function<void(vector<char>)> dataReadyEvent, function<void()> tokenPassEvent, function<void()> closed);

	bool bind(int attempts);

	bool sendData(vector<char> &data); // Returns true if succesfull

	bool passToken(); // Pass token, return false if pass unsuccesful (Waiting or Conection failed)
	void close();

	TransmissionState getState();
private:
	Frame* frame;
	mutex frame_mutex;
	TransmissionState state = TransmissionState::NotConnected;
	thread* connected_thread;

	function<void(vector<char>)> dataReadyEvent;
	function<void()> tokenPassEvent;
	function<void()> closeEvent;

	bool dataSeqSend = 0;
	bool dataSeqReceive = 0;

	bool sendWaitACK(TransmissionType type, vector<char> &data); // Sends data and waits for ACK, tries ATTEMPTS times
	bool sendWaitACK(TransmissionType type);					// Returns false if connection is lost
	void connectedRun();

	void terminate();
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