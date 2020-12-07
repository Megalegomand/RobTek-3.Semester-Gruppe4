#pragma once
#include<vector>

using namespace std;
class RollingBuffer
{
public:
	RollingBuffer();
	RollingBuffer(int size);

private:
	vector<double> array;
	int pos = 0; // Current position, for rolling buffer

};

