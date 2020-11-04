#include "DataLink.h"

DataLink::DataLink() {
	dtmf = new  VirtuelDTMF();
}

bool DataLink::listen(int timeout)
{
	waitFrame(1000);
	return false;
}

bool DataLink::bind(int attempts)
{
	sendFrame(TransmissionType::BIND);
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
		frame.push_back(c);//PREAMBLE
	}

	frame.push_back(SFD);//SFD

	frame.push_back(transmissionType);//TYPE

	//frame.push_back(data.size());//DATALENGTH

	for (char c : data) {
		frame.push_back(c);//DATA
	}

	dtmf->sendSequence(frame);
}

void DataLink::sendFrame(TransmissionType transmissionType)
{
	sendFrame(transmissionType, vector<char>());
}

vector<char> DataLink::waitFrame(int timeout)
{
	auto start = chrono::system_clock::now();
	while ((chrono::system_clock::now() - start).count() * 1000 < timeout) {
		char tone = dtmf->listenTone();
		if (tone != -1) {
			break;
		}
	}


	return vector<char>();
}

