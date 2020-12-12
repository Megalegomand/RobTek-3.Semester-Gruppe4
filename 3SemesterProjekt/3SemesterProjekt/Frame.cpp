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
	
	//TYPE
	if (transmissionType == DATA) {
		if (dataSeqSend) {
			toneFrame.push_back(DATA1);
		}
		else {
			toneFrame.push_back(DATA0);
		}
	}
	else {
		toneFrame.push_back(transmissionType);
	}

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
		//cout << "S" << tones.size()<< endl;

		if (tones.size() > 5) { // Preamble (min 2) + Type (1) + Length (2) = 5
			// Check preamble
			int firstPreamble = -1;
			while (tones.size() > 0) {
				for (int p = 0; p < 6; p++) { // Min 2, hence last element in preamble can't be first in sequence
					if (tones.front() == PREAMBLE[p]) {
						firstPreamble = p;
						break;
					}
				}
				if (firstPreamble != -1) {
					break;
				}
				tones.erase(tones.begin());
			}
			if (firstPreamble == -1) {
				continue;
			}

			//cout << "KKDS" << int(tones.front()) << endl;

			if (tones.size() < 10 - firstPreamble) { // Preamble (7) + Type (1) + Length (2) = 10
				continue;
			}

			bool p = false;
			for (int i = firstPreamble; i < 7; i++) {
				if (tones.front() == PREAMBLE[i]) {
					p = true;
				}
				else {
					p = false;
					break;
				}
				tones.erase(tones.begin());
			}
			if (!p) {
				continue;
			}

			// Transmission type
			transmissionType = (TransmissionType) tones.front();
			tones.erase(tones.begin());

			// Check data sequence
			if (transmissionType == DATA0 && dataSeqReceive || transmissionType == DATA1 && !dataSeqReceive) {
				sendFrame(ACK);
				transmissionType = NONE;
				continue;
			}

			if (transmissionType == DATA0 || transmissionType == DATA1) {
				transmissionType = DATA;
			}

			// Data length
			unsigned char dataLength = (tones[0] << 4) | tones[1];
			tones.erase(tones.begin(), tones.begin() + 2);

			// Check length
			if (tones.size() < dataLength * 2) {
				continue;
			}

			// Add data to dataTones
			dataTones.clear();
			for (int i = 0; i < dataLength * 2; i++) {
				dataTones.push_back(tones[i]);
			}

			lastActive->start();

			//cout << "TT" << transmissionType << endl;
			//for (char c : dataTones) {
			//	cout << int(c) << endl;
			//}
			return true;
		}
	}
	return false;
}

Timer* Frame::getLastActive()
{
	return lastActive;
}

