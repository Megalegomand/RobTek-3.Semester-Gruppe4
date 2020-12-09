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
	vector<char> data = vector<char>();
	for (int i = 0; i < dataTones.size(); i+=2) {
		data.push_back((dataTones[i] << 4) | dataTones[i+1]);
	}
	return data;
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

	lastActive->start();
	dtmf->sendSequence(toneFrame);
}

bool Frame::wait(int timeout)
{
	Timer startTime = Timer();
	startTime.start();

	dataTones.clear();
	transmissionType = NONE;

	while (startTime.elapsedMillis() < timeout) {
		vector<char> tones = dtmf->listenSequence(timeout - startTime.elapsedMillis());

		cout << "S" << tones.size() << endl; 
		for (char t : tones) {
			cout << int(t) << endl;
		}

		if (tones.size() > 4) { // Preamble (min 1) + Type (1) + Length (2) = 4
			// Check preamble
			int p = 0; // 0, 2 skip package
			for (int i = 0; i < 7; i++) {
				if (tones.front() == PREAMBLE[i]) {
					if (p == 0) {
						p = 1;
					}
				} else {
					if (p == 1) {
						p == 2;
					}
				}
				tones.erase(tones.begin());
			}
			if (p == 0 || p == 2) {
				cout << "--------" << endl;
				continue;
			}

			// Transmission type
			transmissionType = (TransmissionType) tones.front();
			tones.erase(tones.begin());

			// Data length
			unsigned char dataLength = (tones[0] << 4) | tones[1];
			tones.erase(tones.begin(), tones.begin() + 2);

			// Check length
			if (tones.size() != dataLength * 2) {
				continue;
			}

			// Add data to dataTones
			dataTones.clear();
			for (char c : tones) {
				dataTones.push_back(c);
			}

			cout << "TT" << transmissionType << endl;
			for (char t : dataTones) {
				cout << int(t) << endl;
			}

			lastActive->start();
			return true;
		}
	}
	return false;
}

Timer* Frame::getLastActive()
{
	return lastActive;
}

