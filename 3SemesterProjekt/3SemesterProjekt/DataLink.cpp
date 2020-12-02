#include "DataLink.h"

DataLink::DataLink() {
	frame = new Frame();
	srand(time(NULL)); // To make sure virtual DTMF works
}

DataLink::DataLink(function<void(vector<char>)> dataReadyEvent, function<void()> tokenPassEvent, function<void()> closeEvent) : DataLink()
{
	this->dataReadyEvent = dataReadyEvent;
	this->tokenPassEvent = tokenPassEvent;
	this->closeEvent = closeEvent;
}

bool DataLink::bind(int attempts)
{
	if (state != TransmissionState::NotConnected) {
		return false;
	}
	frame_mutex.lock(); // Locked throughout, since other threads are disabled during bind
	for (int i = 0; i < attempts; i++) {
		frame->sendFrame(BIND);

		// Wait random time for collision avoidance
		if (frame->wait(rand() % BIND_WAIT_DIFF + BIND_WAIT_DIFF)) {
			switch (frame->getType()) {
			case ACK:
				state = TransmissionState::Token;
				connected_thread = new thread(&DataLink::connectedRun, this);
				frame_mutex.unlock();
				return true;
			case BIND:
				frame->sendFrame(ACK);
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
	bool ret = false;
	if (state == TransmissionState::Token) {
		frame_mutex.lock();
		ret = sendWaitACK(DATA, data);
		frame_mutex.unlock();
	}
	return ret;
}

bool DataLink::passToken()
{
	if (state == TransmissionState::Token) {
		if (sendWaitACK(TOKEN_PASS)) {
			state = TransmissionState::Waiting;
			return true;
		}
	}
	return false;
}

void DataLink::close()
{
	if (state == TransmissionState::Token) {
		sendWaitACK(CLOSE);
		terminate();
	}
	else 
	{
		terminate();
	}
}

TransmissionState DataLink::getState()
{
	return state;
}

bool DataLink::sendWaitACK(TransmissionType type, vector<char> data)
{
	if (state != TransmissionState::Token) {
		return false;
	}

	frame_mutex.lock();
	for (int i = 0; i < ATTEMPTS; i++) {
		frame->sendFrame(type, data);

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
				terminate();
			}
			else {
				if (frame->wait(MAX_LOSS_CONNECTION - frame->getLastActive()->elapsedMillis())) {
					switch (frame->getType()) {
					case DATA:
						dataReadyEvent(frame->getData());
						frame->sendFrame(ACK);
						break;
					case TOKEN_PASS:
						tokenPassEvent();
						state = TransmissionState::Token;
						frame->sendFrame(ACK);
						break;
					case ALIVE:
						frame->sendFrame(ACK);
						break;
					case CLOSE:
						frame->sendFrame(ACK);
						terminate();
						break;
					}
				}
			}
		} else if (state == TransmissionState::Token) {
			if (frame->getLastActive()->elapsedMillis() > MAX_LOSS_CONNECTION / 2) {
				if (!sendWaitACK(ALIVE)) {
					terminate();
				}
			}
			else {
				frame->getLastActive()->sleepUntil(MAX_LOSS_CONNECTION / 2);
			}
		}
	}
}

void DataLink::terminate()
{
	state = TransmissionState::NotConnected;
	closeEvent();
}


