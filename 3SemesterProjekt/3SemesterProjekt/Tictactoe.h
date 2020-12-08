#pragma once
#include<iostream>
#include"DTMF.h"
#include"FrameHandler.h"
using namespace std;


class TicTacToe {
public:
	TicTacToe();
	int game();
	int checkwin();
	void board();
	void data(vector<char> data);
	void tokenpass();
	void end();


private:
	char square[10] = { 'o','1','2','3','4','5','6','7','8','9' };
	int local = 0;// controls which player is local
	int choicef = 0; // choice of foreign player


};