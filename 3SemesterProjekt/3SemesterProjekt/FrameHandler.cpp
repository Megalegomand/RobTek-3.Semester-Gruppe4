#include "FrameHandler.h"

FrameHandler::FrameHandler() {
	frame = new Frame();

	// To make sure virtual DTMF works
	// Used for debugging
	srand(time(NULL)); 
}

FrameHandler::FrameHandler(function<void(vector<char>)> dataReadyEvent, function<void()> tokenPassEvent, function<void()> closeEvent) : FrameHandler()
{
	this->dataReadyEvent = dataReadyEvent;
	this->tokenPassEvent = tokenPassEvent;
	this->closeEvent = closeEvent;
}

bool FrameHandler::bind(int attempts)
{
	// Make sure device is not connected already
	if (state != TransmissionState::NotConnected) {
		return false;
	}

	// Reset sequence numbers
	dataSeqReceive = 0;
	dataSeqSend = 0;

	frame_mutex.lock(); // Locked throughout, since other threads are disabled during bind
	for (int i = 0; i < attempts; i++) {
		frame->sendFrame(BIND);

		// Wait random time for collision avoidance
		if (frame->wait(rand() % BIND_WAIT_DIFF + BIND_WAIT_DIFF)) {
			// Set state and start background thread
			switch (frame->getType()) {
			case ACK:
				state = TransmissionState::Primary;
				connected_thread = new thread(&FrameHandler::connectedRun, this);
				frame_mutex.unlock();
				return true;
			case BIND:
				frame->sendFrame(ACK);
				state = TransmissionState::Secondary;
				connected_thread = new thread(&FrameHandler::connectedRun, this);
				frame_mutex.unlock();
				return true;
			}
		}
	}
	frame_mutex.unlock();

	return false;
}

bool FrameHandler::sendData(vector<char> &data)
{	
	bool ret = false;

	// Make sure device is allowed to send
	if (state == TransmissionState::Primary) {
		// Send type based on sequencenumber
		if (dataSeqSend) {
			ret = sendWaitACK(DATA1, data);
		}
		else 
		{
			ret = sendWaitACK(DATA0, data);
		}

		// Update sequence number
		if (ret) {
			dataSeqSend = !dataSeqSend;
		}
		else {
			terminate();
		}
	}
	return ret;
}

bool FrameHandler::passToken()
{
	// Make sure device is allowed
	if (state != TransmissionState::Primary) {
		return false;
	}

	// For testing
	send++;
	
	// Send TOKENPASS until TOKENPASSACK is recieved
	frame_mutex.lock();
	for (int i = 0; i < ATTEMPTS; i++) {
		frame->sendFrame(TOKENPASS);
		if (frame->wait(LISTEN_TIME)) {
			if (frame->getType() == TOKENPASSACK) {
				frame_mutex.unlock();
				// Update state
				state = TransmissionState::Secondary;
				frame->sendFrame(ACK);
				resend += i; // For testing
				return true;
			}
		}
	}
	frame_mutex.unlock();
	return false;
}

void FrameHandler::close()
{
	// Make sure device is allowed, otherwise cut the connection
	if (state == TransmissionState::Primary) {
		sendWaitACK(CLOSE);
		terminate();
	}
	else 
	{
		terminate();
	}
}

TransmissionState FrameHandler::getState()
{
	return state;
}

bool FrameHandler::sendWaitACK(TransmissionType type, vector<char> &data)
{
	// Make sure device is allowed
	if (state != TransmissionState::Primary) {
		return false;
	}

	// For testing
	send++;

	// Send frame until ACK is recieved
	frame_mutex.lock();
	for (int i = 0; i < ATTEMPTS; i++) {
		frame->sendFrame(type, data);
		if (frame->wait(LISTEN_TIME)) {
			if (frame->getType() == ACK) {
				frame_mutex.unlock();
				resend += i; // For testing
				return true;
			}
		}
	}
	frame_mutex.unlock();
	return false;
}

bool FrameHandler::sendWaitACK(TransmissionType type)
{
	vector<char> data = vector<char>(); // Empty data array
	return sendWaitACK(type, data);
}

void FrameHandler::connectedRun()
{
	// Make sure thread only runs when connected
	while (state != TransmissionState::NotConnected) {
		// Check for state
		if (state == TransmissionState::Secondary) {
			// Check if MAX_LOSS_CONNECTION is exceeded
			if (frame->getLastActive()->elapsedMillis() > MAX_LOSS_CONNECTION) {
				terminate();
			}
			else {
				// Wait for incomming frames
				if (frame->wait(MAX_LOSS_CONNECTION - frame->getLastActive()->elapsedMillis())) {
					switch (frame->getType()) {
					case DATA0: // Recieve data and check sequence number
						if (!dataSeqReceive) { // Discard frame if it doesn't match
							dataReadyEvent(frame->getData());
							dataSeqReceive = !dataSeqReceive;
						}
						frame->sendFrame(ACK); // ACK either way
						break;
					case DATA1: // Same as DATA0
						if (dataSeqReceive) {
							dataReadyEvent(frame->getData());
							dataSeqReceive = !dataSeqReceive;
						}
						frame->sendFrame(ACK);
						break;
					case TOKENPASS: // Pass token
						state = TransmissionState::Primary; // Update state
						sendWaitACK(TOKENPASSACK);
						tokenPassEvent(); // Call application layer
						break;
					case CLOSE:
						frame->sendFrame(ACK);
						terminate(); // Stops this thread
						break;
					case BIND:
					case TOKENPASSACK:
					case ALIVE:
						frame->sendFrame(ACK);
						break;

					}
				}
			}
		} else if (state == TransmissionState::Primary) {
			// Send alive if MAX_LOSS_CONNECTION / 2 is exceeded
			if (frame->getLastActive()->elapsedMillis() > MAX_LOSS_CONNECTION / 2) {
				if (!sendWaitACK(ALIVE)) {
					terminate();
				}
			}
			else {
				// Wait until MAX_LOSS_CONNECTION / 2 is exceeded
				frame->getLastActive()->sleepUntil(MAX_LOSS_CONNECTION / 2);
			}
		}
	}
}

void FrameHandler::terminate()
{
	state = TransmissionState::NotConnected; // Terminates background thread
	closeEvent(); // Notify application layer
}


