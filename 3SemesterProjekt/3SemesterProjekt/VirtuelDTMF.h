#pragma once
#include "DTMF.h"
#include<mutex>
#include <chrono>
#include <thread>

using namespace std;
class VirtuelDTMF : public DTMF
{
public:
	void sendSequence(vector<char>& sequence);
	void sendTone(char tone);
	char listenTone();

	void outputMedium();

	static char medium;
	static mutex medium_mutex;
private:
	
};

