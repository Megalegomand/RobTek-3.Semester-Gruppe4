#include "DataLink.h"

DataLink::DataLink() {
	dtmf = new  VirtuelDTMF();
}

bool DataLink::listen(int timeout)
{
	return false;
}

bool DataLink::bind(int attempts)
{
	return false;
}

bool DataLink::sendData(vector<char> &data)
{
	dtmf->sendSequence(data);
	return false;
}

void DataLink::sendTone(char tone)
{
	sendBuffer.push(tone);
}

char DataLink::receiveTone()
{
	char tone = 0x0;
	if (recieveBuffer.try_pop(tone)) {
		return tone;
	}
	else {
		return -1;
	}
	
}

void DataLink::sendFrame(TransmissionType transmissionType, vector<char> data)
{
	vector<char> frame;

	for (char c : PREAMBLE) {
		frame.push_back(c);
	}

	frame.push_back(transmissionType);

	dtmf->sendSequence(frame);
}

