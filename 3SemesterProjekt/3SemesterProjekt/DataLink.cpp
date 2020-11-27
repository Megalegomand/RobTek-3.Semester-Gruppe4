#include "DataLink.h"

DataLink::DataLink() {
	frame = new Frame();
	srand(time(NULL)); // To make sure virtual DTMF works
}

DataLink::DataLink(function<void(TransmissionType, vector<char>)> dataReady, function<void()> tokenPass) : DataLink()
{
	this->dataReady = dataReady;
	this->tokenPass = tokenPass;
}

bool DataLink::bind(int attempts)
{
	if (state != TransmissionState::NotConnected) {
		return false;
	}
	frame_mutex.lock(); // Locked throughout, since other threads are disabled during bind
	for (int i = 0; i < attempts; i++) {
		frame->setFrame(BIND);
		frame->send();

		// Wait random time for collision avoidance
		if (frame->wait(rand() % BIND_WAIT_DIFF + BIND_WAIT_DIFF)) {
			switch (frame->getType()) {
			case ACK:
				state = TransmissionState::Token;
				connected_thread = new thread(&DataLink::connectedRun, this);
				frame_mutex.unlock();
				return true;
			case BIND:
				frame->setFrame(ACK);
				frame->send();
				state = TransmissionState::Waiting;
				connected_thread = new thread(&DataLink::connectedRun, this);
				frame_mutex.unlock();
				return true;
			}
		}
	}
	frame_mutex.unlock();
	return false;
}

bool DataLink::sendData(vector<char> &data)
{	
	frame_mutex.lock();
	sendWaitACK(DATA, data);
	frame_mutex.unlock();
	return false;
}

bool DataLink::passToken()
{
	if (state == TransmissionState::Token) {
		if (sendWaitACK(TOKEN_PASS)) {
			return true;
		}
	}
	return false;
}

bool DataLink::sendWaitACK(TransmissionType type, vector<char> data)
{
	frame_mutex.lock(); // Locked throughout, since other threads should be disabled from trying
	for (int i = 0; i < ATTEMPTS; i++) {
		frame->setFrame(type, data);
		frame->send();

		if (frame->wait(LISTEN_TIME)) {
			if (frame->getType() == ACK) {
				frame_mutex.unlock();
				return true;
			}
		}
	}
	frame_mutex.unlock();
	return false;
}

bool DataLink::sendWaitACK(TransmissionType type)
{
	return sendWaitACK(type, vector<char>());
}

void DataLink::connectedRun()
{
	while (state != TransmissionState::NotConnected) {
		if (state == TransmissionState::Waiting) {
			if (frame->getLastActive()->elapsedMillis() > MAX_LOSS_CONNECTION) {
				state = TransmissionState::NotConnected;
			}
			else {
				frame->getLastActive()->sleepUntil(MAX_LOSS_CONNECTION);
			}
		} else if (state == TransmissionState::Token) {
			if (frame->getLastActive()->elapsedMillis() > MAX_LOSS_CONNECTION / 2) {
				if (!sendWaitACK(ALIVE)) {
					state = TransmissionState::NotConnected;
				}
			}
			else {
				frame->getLastActive()->sleepUntil(MAX_LOSS_CONNECTION / 2);
			}
		}
	}
}


