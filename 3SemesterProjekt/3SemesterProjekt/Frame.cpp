#include "Frame.h"

Frame::Frame()
{
	transmissionType = NONE;
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
	// Conversion from tones to bytes
	for (int i = 0; i < dataTones.size(); i+=2) {
		data.push_back((dataTones[i] << 4) | dataTones[i+1]);
	}
	return data;
}

void Frame::sendFrame(TransmissionType transmissionType)
{
	// Sets internal data
	this->transmissionType = transmissionType;
	this->dataTones.clear();

	// Sends frame
	send();
}

void Frame::sendFrame(TransmissionType transmissionType, vector<char> data)
{
	this->transmissionType = transmissionType;
	// Convert data from bytes to tones
	dataTones.clear();
	if (data.size() < 16) {
		for (char c : data) {
			dataTones.push_back(char(c & 0xF0) >> 4);
			dataTones.push_back(char(c & 0x0F));
		}
	}
	this->dataTones = dataTones;

	// Sends frame
	send();
}

void Frame::send()
{
	// Check for inccorect transmission type
	if (transmissionType == NONE) {
		return;
	}

	// Tones part of the frame
	vector<char> toneFrame;

	// Add preamble
	for (char c : PREAMBLE) {
		toneFrame.push_back(c);
	}
	
	// Add type
	toneFrame.push_back(transmissionType);

	// Check size
	unsigned char size = dataTones.size() / 2;
	if (size > 15) {
		return;
	}

	// Add size
	toneFrame.push_back(char(size & 0x0F));

	// Add data
	for (char c : dataTones) {
		toneFrame.push_back(c);
	}

	// Create CRC data
	vector<char> crcData = vector<char>();
	crcData.push_back(transmissionType); // Add type
	for (int i = 0; i < dataTones.size(); i+=2) {
		crcData.push_back((dataTones[i] << 4) | dataTones[i+1]); // Add data as bytes
	}

	// Calculate CRC 
	unsigned short crc = crc16(crcData);

	// Add CRC
	toneFrame.push_back((crc & 0xF000) >> 12); // & for readability
	toneFrame.push_back((crc & 0x0F00) >> 8);
	toneFrame.push_back((crc & 0x00F0) >> 4);
	toneFrame.push_back(crc & 0x000F);
	
	// Send the sequence
	dtmf->sendSequence(toneFrame);

	// Restart lastActive timer
	lastActive->start();
}

bool Frame::wait(int timeout)
{
	// Timer to keep within timeout
	Timer startTime = Timer();
	startTime.start();

	// Make sure data and type is cleared
	dataTones.clear();
	transmissionType = NONE;

	// Try until a frame is found or timeout is exceeded
	while (startTime.elapsedMillis() < timeout) {
		// Get sequence from DTMF
		vector<char> tones = dtmf->listenSequence(timeout - startTime.elapsedMillis());
		
		// Should be larger than minimum size
		// Preamble (min 1) + Type (1) + Length (1) + CRC (4) = 7
		while (tones.size() >= 7) { 
			// Check last tone of preamble
			if (tones.front() != PREAMBLE[3]) {
				tones.erase(tones.begin()); // Remove tones until reached, or minimum size is reached
				continue;
			}

			// Remove last tone of preamble
			tones.erase(tones.begin());

			// Get transmission type
			transmissionType = (TransmissionType) tones.front();
			tones.erase(tones.begin());

			// Get data length
			char dataLength = tones[0];
			tones.erase(tones.begin());

			// Check length
			if (tones.size() < dataLength * 2 + 4) { // Data (dataLength * 2) + CRC (4)
				tones.erase(tones.begin());
				continue;
			}

			
			// Make datatones is cleared
			dataTones.clear(); 

			// Add data to dataTones
			for (int i = 0; i < dataLength * 2; i++) {
				dataTones.push_back(tones[i]);
			}
			tones.erase(tones.begin(), tones.begin() + dataLength * 2);
			
			// Collect CRC calculation data
			vector<char> crcData = vector<char>();
			crcData.push_back(transmissionType);
			for (int i = 0; i < dataTones.size(); i += 2) {
				crcData.push_back((dataTones[i] << 4) | dataTones[i + 1]);
			}
			
			// Check CRC from frame
			crcData.push_back((tones[0] << 4) | tones[1]);
			crcData.push_back((tones[2] << 4) | tones[3]);

			// Calculate CRC, 0 if no errors
			unsigned short crc = crc16(crcData);
			
			// CRC wrong
			if (crc) {
				transmissionType = NONE;
				dataTones.clear();
				tones.erase(tones.begin());
				continue;
			}

			// Update last active timer
			lastActive->start();

			// Found valid frame
			return true;
		}
	}
	return false;
}

Timer* Frame::getLastActive()
{
	return lastActive;
}

// CRC 16 CCIT code from https://stackoverflow.com/questions/10564491/function-to-calculate-a-crc16-checksum/23726131#23726131
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