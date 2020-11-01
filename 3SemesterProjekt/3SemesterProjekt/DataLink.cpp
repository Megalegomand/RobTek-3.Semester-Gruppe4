#include "DataLink.h"

DataLink::DataLink()
{
}

bool DataLink::listen(int timeout)
{
	return false;
}

bool DataLink::bind(int attempts)
{
	return false;
}

bool DataLink::sendData(vector<char>* data)
{
	return false;
}

void DataLink::sendTone(char tone)
{
}

char DataLink::receiveTone()
{
	return 0;
}

void DataLink::sendFrame(TransmissionType transmissionType, vector<char> data)
{
	for (char c : PREAMBLE) {
		sendTone(c);
	}

	sendTone(transmissionType);
}
