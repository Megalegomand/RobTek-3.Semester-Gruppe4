#pragma once
#include<vector>

enum TransmissionType {
	BIND = 0x0,
	CONNECT = 0x1,
	ACK = 0x2,
	NACK = 0x3,
	TOKEN_PASS = 0x4,
	DATA = 0xF,
};

using namespace std;
class Frame
{
public:
	Frame();
	Frame(TransmissionType transmissionType, vector<char> data);
	Frame(TransmissionType transmissionType);

	TransmissionType getType();
	vector<char> getData();

private:
	TransmissionType transmissionType;
	vector<char> data;
};

