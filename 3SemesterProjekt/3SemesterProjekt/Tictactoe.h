#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include<iostream>
#include "Goertzel.h"
#include"DTMF.h"
#include"DataLink.h"
#include<thread>
#include<fstream>
#include <functional>
using namespace std;


class Tictactoe {
public:
	Tictactoe();
	int game();
	int checkwin();
	void board();
	void data(vector<char> data);
	void tokenpass();


private:
	char square[10] = { 'o','1','2','3','4','5','6','7','8','9' };


};