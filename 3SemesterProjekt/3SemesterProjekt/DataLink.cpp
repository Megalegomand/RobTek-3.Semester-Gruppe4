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

void DataLink::sendFrame(Frame frame)
{
	vector<char> toneFrame;

	for (char c : PREAMBLE) {
		toneFrame.push_back(c);//PREAMBLE
	}

	toneFrame.push_back(SFD);//SFD

	toneFrame.push_back(frame.getType());//TYPE

	//frame.push_back(data.size());//DATALENGTH

	for (char c : frame.getData()) {
		toneFrame.push_back(c);//DATA
	}

	dtmf->sendSequence(toneFrame);
}

Frame DataLink::waitFrame(int timeout)
{
	auto start = chrono::system_clock::now();
	while ((chrono::system_clock::now() - start).count() * 1000 < timeout) {
		char tone = dtmf->listenTone();
		if (tone != -1) {
			break;
		}
	}


	return Frame();
}

