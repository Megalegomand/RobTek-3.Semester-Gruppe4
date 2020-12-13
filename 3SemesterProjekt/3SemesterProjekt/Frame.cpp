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
	toneFrame.push_back(transmissionType);

	unsigned char size = dataTones.size() / 2;
	if (size > 15) {
		return;
	}
	toneFrame.push_back(char(size & 0x0F) >> 0);

	for (char c : dataTones) {
		toneFrame.push_back(c);//DATA
	}

	//for (char c : toneFrame) {
	//	cout << int(c) << endl;
	//}

	vector<char> crcData = vector<char>();
	crcData.push_back(transmissionType);
	for (int i = 0; i < dataTones.size(); i+=2) {
		crcData.push_back((dataTones[i] << 4) | dataTones[i+1]);
	}
	unsigned short crc = crc16(crcData);

	toneFrame.push_back((crc & 0xF000) >> 12); // & for readability
	toneFrame.push_back((crc & 0x0F00) >> 8);
	toneFrame.push_back((crc & 0x00F0) >> 4);
	toneFrame.push_back(crc & 0x000F);
	
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

		//for (char c : tones) {
		//	cout << int(c) << endl;
		//}
		
		if (tones.size() >= 9) { // Preamble (min 2) + Type (1) + Length (2) + CRC (4) = 9
			// Check preamble
			int firstPreamble = -1;
			while (tones.size() > 0) {
				cout << "S" << tones.size() << endl; 
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

			// Data length
			char dataLength = tones[0];
			tones.erase(tones.begin());

			// Check length
			if (tones.size() < dataLength * 2 + 4) { // Data (dataLength * 2) + CRC (4)
				continue;
			}

			// Add data to dataTones
			dataTones.clear();
			for (int i = 0; i < dataLength * 2; i++) {
				dataTones.push_back(tones[i]);
			}
			tones.erase(tones.begin(), tones.begin() + dataLength * 2);
			
			// Check CRC
			vector<char> crcData = vector<char>();
			crcData.push_back(transmissionType);
			for (int i = 0; i < dataTones.size(); i += 2) {
				crcData.push_back((dataTones[i] << 4) | dataTones[i + 1]);
			}
			
			crcData.push_back((tones[0] << 4) | tones[1]);
			crcData.push_back((tones[2] << 4) | tones[3]);

			unsigned short crc = crc16(crcData);
			
			if (crc) { // CRC wrong
				transmissionType = NONE;
				dataTones.clear();
				continue;
			}

			lastActive->start();

			cout << "TT" << transmissionType << endl;
			for (char c : dataTones) {
				cout << int(c) << endl;
			}
			return true;
		}
	}
	return false;
}

Timer* Frame::getLastActive()
{
	return lastActive;
}

unsigned short Frame::crc16(vector<char> data) {
	unsigned char x;
	unsigned short crc = 0xFFFF;

	for (char c : data) {
		x = crc >> 8 ^ c;
		x ^= x >> 4;
		crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^ ((unsigned short)(x << 5)) ^ ((unsigned short)x);
	}

	return crc;
}