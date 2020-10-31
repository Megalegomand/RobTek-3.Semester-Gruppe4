#pragma once
class DataLink
{
public:
	DataLink();
	

	bool listen(int timeout); // Timeout in ms
	bool bind(int attempts);
private:
	bool gotToken;

	// Data transmission types
	const unsigned char BIND = 0x0;
	const unsigned char CONNECT = 0x1;
	const unsigned char ACK = 0x2;
	const unsigned char NACK = 0x3;
	const unsigned char TOKEN_PASS = 0x4;
	const unsigned char DATA = 0xF;

	const unsigned int PREAMBLE = 0xFA50FA50;
	const unsigned char SFD = 0xF;
};

/*
Preamble        SFD (1tone) (xbytes)
FA50FA50 F   TYPE     LENGTH DATA Checksum


tone = 100ms


1: Bind
2: ACK

1: Connect
2: ACK
1: Data
2: ACK
1: Token til 2
2: ACK
Time 100000 (
	2: Connect
	1: ACK
)
2: Connect
1: ACK
2: Data
1: ACK
2: Token til 1
1: ACK

*/