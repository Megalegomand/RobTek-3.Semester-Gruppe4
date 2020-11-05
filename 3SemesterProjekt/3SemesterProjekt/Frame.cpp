#include "Frame.h"

Frame::Frame()
{
}

Frame::Frame(TransmissionType transmissionType, vector<char> data)
{
	this->transmissionType = transmissionType;
	this->data = data;
}

Frame::Frame(TransmissionType transmissionType)
{
	this->transmissionType = transmissionType;
}

TransmissionType Frame::getType()
{
	return transmissionType;
}

vector<char> Frame::getData()
{
	return data;
}
