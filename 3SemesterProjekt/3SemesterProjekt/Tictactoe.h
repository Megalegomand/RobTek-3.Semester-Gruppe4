#pragma once
#include<iostream>
#include"DTMF.h"
#include"FrameHandler.h"
using namespace std;


class TicTacToe {
public:
	TicTacToe();
	int game(); // runs the game
	int checkwin(); // determines if the game have been won
	void board(); // makes the board
	void data(vector<char> data); // holder for data that needs to be send
	void tokenpass(); // used to pass the turn
	void end(); //used to End the session


private:
	char square[10] = { 'o','1','2','3','4','5','6','7','8','9' }; //holds the pieces on the board 
	int local = 0;// controls which player is local
	int choicef = 0; // choice of foreign player


};