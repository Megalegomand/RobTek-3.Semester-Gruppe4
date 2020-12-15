#pragma once
#include<concurrent_queue.h>
#include<vector>
#include<iostream>
#include"DTMF.h"
#include"Frame.h"
#include<mutex>
#include <functional>

enum class TransmissionState {
	NotConnected, 
	Primary, // Has token, is able to send data along
	Secondary, // Waiting for data and token
};

using namespace std;
using namespace placeholders;
using namespace concurrency;
class FrameHandler
{
public:
	const int BIND_WAIT_MIN = 10000; // Minimum wait when binding
	const int BIND_WAIT_DIFF = 10000; // Random time between BIND_WAIT_MIN and BIND_WAIT_MIN + BIND_WAIT_DIFF
	const int ATTEMPTS = 10; // Attempts at sending before termination, does not account for bind
	const int LISTEN_TIME = 5000; // How long to listen for between each frame
	const int MAX_LOSS_CONNECTION = ATTEMPTS * LISTEN_TIME * 2; // Maximum loss of connection, sync will be send at half this time

	// For testing
	int resend = 0;
	int send = 0;


	FrameHandler();
	// Construtor with event functions, use std::bind for these
	FrameHandler(function<void(vector<char>)> dataReadyEvent, function<void()> tokenPassEvent, function<void()> closed);

	// Connect to another device
	bool bind(int attempts);

	// Send a char array (byte array) as data
	// Returns true if succesfull
	bool sendData(vector<char>& data);

	// Pass token, return false if pass unsuccesful (Waiting or Conection failed)
	bool passToken();

	// Close connection
	void close();

	// Get current state
	TransmissionState getState();
private:
	Frame* frame;
	mutex frame_mutex;
	TransmissionState state = TransmissionState::NotConnected;
	thread* connected_thread; // Background thread running while connected

	// Events for application layer
	function<void(vector<char>)> dataReadyEvent;
	function<void()> tokenPassEvent;
	function<void()> closeEvent;

	// Sequence numbers
	bool dataSeqSend = 0;
	bool dataSeqReceive = 0;

	// Sends data and waits for ACK, tries ATTEMPTS times
	// Returns false if all attempts have been used
	bool sendWaitACK(TransmissionType type, vector<char>& data);
	bool sendWaitACK(TransmissionType type);

	// Function run in background thread
	void connectedRun();

	// Close connection without notifying other device
	void terminate();
};
