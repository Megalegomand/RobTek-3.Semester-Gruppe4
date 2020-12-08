#include "Frame.h"

Frame::Frame()
{
	transmissionType = BIND;
	dtmf = new DTMF();
	lastActive = new Timer();
}

TransmissionType Frame::getType()
{
	return transmissionType;
}

vector<char> Frame::getData()
{
	return dataTones;
}

void Frame::sendFrame(TransmissionType transmissionType)
{
	this->transmissionType = transmissionType;
	this->dataTones.clear();
	send();
}

void Frame::sendFrame(TransmissionType transmissionType, vector<char> data)
{
	this->transmissionType = transmissionType;
	// Convert data from bytes to tones
	dataTones.clear();
	if (data.size() < 256) {
		for (char c : data) {
			dataTones.push_back(char(c & 0xF0) >> 4);
			dataTones.push_back(char(c & 0x0F) >> 0);
		}
	}
	this->dataTones = dataTones;
	send();
}

void Frame::send()
{
	if (transmissionType == NONE) {
		return;
	}

	vector<char> toneFrame;

	for (char c : PREAMBLE) {
		toneFrame.push_back(c);//PREAMBLE
	}

	toneFrame.push_back(transmissionType);//TYPE

	unsigned char size = dataTones.size() / 2;
	toneFrame.push_back(char(size & 0xF0) >> 4);
	toneFrame.push_back(char(size & 0x0F) >> 0);

	for (char c : dataTones) {
		toneFrame.push_back(c);//DATA
	}

	dtmf->sendSequence(toneFrame);
}

bool Frame::wait(int timeout)
{
	Timer startTime = Timer();
	startTime.start();

	this->dataTones.clear();
	this->transmissionType = NONE;

	while (startTime.elapsedMillis() < timeout) {
		vector<char> tones = dtmf->listenSequence(timeout - startTime.elapsedMillis());
		if (tones.size() > 7) { // Preamble (5) + Type (1) + Length (2) = 8
			// Check preamble
			bool p = true;
			for (int i = 0; i < 5; i++) {
				if (tones.front() != PREAMBLE[i]) {
					p = false;
				}
				tones.erase(tones.begin());
			}
			if (!p) {
				continue;
			}

			// Transmission type
			transmissionType = (TransmissionType) tones.front();
			tones.erase(tones.begin());

			// Data length
			unsigned char dataLength = (tones[0] << 4) | tones[1];
			cout << "Length" << int(dataLength) << endl;
			tones.erase(tones.begin(), tones.begin() + 2);

			// Check length
			if (tones.size() != dataLength * 2) {
				continue;
			}

			for (char c : tones) {
				cout << int(c) << endl;
			}

			// Add data to dataTones
			dataTones.clear();
			for (char c : tones) {
				dataTones.push_back(c);
			}
		}
		cout << "K" << endl;

		cout << "TT" << int(transmissionType) << endl;
		for (char c : dataTones) {
			cout << int(c) << endl;
		}
		return true;
	}
	return false;
}

Timer* Frame::getLastActive()
{
	return lastActive;
}

