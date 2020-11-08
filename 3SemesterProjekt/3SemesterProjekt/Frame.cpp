#include "Frame.h"

Frame::Frame()
{
	transmissionType = BIND;
	dtmf = new DTMF();
	timer = new Timer();
}

TransmissionType Frame::getType()
{
	return transmissionType;
}

vector<char> Frame::getData()
{
	return data;
}

void Frame::setFrame(TransmissionType transmissionType)
{
	this->transmissionType = transmissionType;
	this->data.clear();
}

void Frame::setFrame(TransmissionType transmissionType, vector<char> data)
{
	this->transmissionType = transmissionType;
	this->data = data;
}

void Frame::send()
{
	vector<char> toneFrame;

	for (char c : PREAMBLE) {
		toneFrame.push_back(c);//PREAMBLE
	}

	//toneFrame.push_back(SFD);//SFD

	toneFrame.push_back(transmissionType);//TYPE

	//frame.push_back(data.size());//DATALENGTH

	for (char c : data) {
		toneFrame.push_back(c);//DATA
	}

	dtmf->sendSequence(toneFrame, TONE_DURATION);
}

bool Frame::wait(int timeout)
{
	this->data.clear();

	timer->start();
	while (timer->elapsedMillis() < timeout) {
		char tone = -1;
		while (tone == -1) {
			timer->start();
			tone = dtmf->listenTone(LISTEN_DURATION);
		}
		
		int tonei = 0;
		int headeri = -1;
		int preambleSize = sizeof(PREAMBLE) / sizeof(PREAMBLE[0]);
		while (tone != -1) {
			//cout << "End" << timer->elapsedMillis() << endl;
			
			if (headeri == -1) // First tone in preamble
			{
				const char* found = find(PREAMBLE, PREAMBLE + preambleSize, tone);
				if (found != end(PREAMBLE)) 
				{
					headeri = distance(PREAMBLE, found) + 1;
				} 
				else 
				{
					break;
				}
			}
			else if (headeri < preambleSize) // Rest of preamble
			{
				if (PREAMBLE[headeri] == tone) {
					headeri++;
				} else {
					break;
				}
			}
			else if (headeri == preambleSize) // Type
			{
				transmissionType = TransmissionType(tone);
				headeri++;
			}
			else // Rest of tones
			{
				data.push_back(tone);
			}
			tonei++;
			tone = nextTone(timer, tonei);
		}
		if (headeri > preambleSize) {
			return true;
		}
	}
	return false;
}

char Frame::nextTone(Timer* timer, int toneNum) {
	//cout << "Begin" << timer->elapsedMillis() << endl;
	timer->sleepUntil(toneNum * TONE_DURATION + LISTEN_MARGIN);
	//cout << "Listen" <<timer->elapsedMillis() << endl;
	return dtmf->listenTone(TONE_DURATION - 2 * LISTEN_MARGIN);
	//timer->sleepUntil(toneNum * TONE_DURATION + TONE_DURATION);
	//cout << "End" << timer->elapsedMillis() << endl;
}
