#include "DataLink.h"

DataLink::DataLink() {
	frame = new Frame();
	srand(time(NULL)); // To make sure virtual DTMF works
}

bool DataLink::bind(int attempts)
{
	if (state != TransmissionState::NotConnected) {
		return false;
	}

	for (int i = 0; i < attempts; i++) {
		frame->setFrame(BIND);
		frame->send();

		// Wait random time for collision avoidance
		if (frame->wait(rand() % BIND_WAIT_DIFF + BIND_WAIT_DIFF)) {
			switch (frame->getType()) {
			case ACK:
				state = TransmissionState::Token;
				//alive_thread = new thread(&DataLink::alive, this);
				return true;
			case BIND:
				frame->setFrame(ACK);
				frame->send();
				state = TransmissionState::Waiting;
				alive_thread = new thread(&DataLink::alive, this);
				return true;
			}
		}
	}
	return false;
}

bool DataLink::sendData(vector<char> &data)
{	
	frame->setFrame(DATA, data);
	frame->send();

	if (frame->wait(1000)) {
		if (frame->getType() == ACK) {
			return true;
		}
	}
		
	return false;
}

vector<char> DataLink::waitData(int timeout)
{
	vector<char> data;
	if (frame->wait(timeout)) {
		if (frame->getType() == DATA) {
			data = frame->getData();
			frame->setFrame(ACK);
			frame->send();
		}
	}
	for (char c : data) {
		cout << int(c) << ", ";
	}
	cout << endl;
	return data;
}

bool DataLink::passToken()
{
	if (state == TransmissionState::Token) {

		frame->setFrame(TOKEN_PASS);
		frame->send();

		if (frame->wait(100000)) {
			if (frame->getType() == ACK) {
				TransmissionState::Waiting;
				return true;
			}
		}
		return false;
	}
	return false;

	/*if (frame->wait(hasToken = false)) {
		hasToken 

		frame->setFrame(TOKEN_PASS);
		frame->send();
	}*/

}

void DataLink::alive()
{
	while (state != TransmissionState::NotConnected) {
		cout << "Kage" << endl;
		switch (state) {
		case TransmissionState::Waiting:
			if (frame->getLastActive()->elapsedMillis() > MAX_LOSS_CONNECTION) {
				state = TransmissionState::NotConnected;
			}
			else {
				frame->getLastActive()->sleepUntil(MAX_LOSS_CONNECTION);
			}
			break;
		case TransmissionState::Token:
			if (frame->getLastActive()->elapsedMillis() > MAX_LOSS_CONNECTION/2) {
				//frame->setFrame()
			}
			else {
				frame->getLastActive()->sleepUntil(MAX_LOSS_CONNECTION);
			}
			break;
		}
	}
}


