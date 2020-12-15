#pragma once
#include<iostream>
#include"DTMF.h"
#include"FrameHandler.h"

using namespace std;
using namespace std::placeholders;
class TicTacToe {
public:
	TicTacToe();
	int game(); // Run game
	int checkwin(); // Determines if the game have been won
	void board(); // Draw board on screen

	void data(vector<char> data); // Data event from FrameHandler
	void tokenpass(); // Token event from FrameHandler
	void end(); // Closed event from FrameHandler


private:
	char square[10] = { 'o','1','2','3','4','5','6','7','8','9' }; // Pieces on the board
	int local = 0; // Controls which player is local
	int choicef = 0; // Choice of foreign player
};