#include "DataLink.h"

DataLink::DataLink() {
	frame = new Frame();
}

bool DataLink::listen(int timeout)
{
	while (true) {
		bool t = frame->wait(timeout);
		cout << "Listen: " << t << endl;
		cout << "Type: " << frame->getType() << endl;
		cout << "Data: ";
		for (char c : frame->getData()) {
			cout << int(c) << ", ";
		}
		cout << endl;
		//frame->send();
	}

	return false;
}

bool DataLink::bind(int attempts)
{
	vector<char> data;
	//data.push_back(0x0);
	while (true) {
		//data.push_back(0x0);
		frame->setFrame(BIND, data);
		frame->send();
		//frame->wait(100);
		//data = frame->getData();
	}
	return false;
}

bool DataLink::sendData(vector<char> &data)
{
	//sendFrame(data);
	return false;
}


