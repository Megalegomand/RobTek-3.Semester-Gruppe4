#pragma once
using namespace std;


class Tictactoe {
public:
	Tictactoe();
	
	int game();
	int checkwin();
	void board();


private:
	char square[10] = { 'o','1','2','3','4','5','6','7','8','9' };


};