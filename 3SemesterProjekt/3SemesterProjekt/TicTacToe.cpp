// Store dele af koden kommer fra http://www.cppforschool.com/project/tic-tac-toe-project.html

#include "Tictactoe.h"

using namespace std;
using namespace std::placeholders;

TicTacToe::TicTacToe() {
}


void TicTacToe::data(vector<char> data) {
    if (data.size() == 1) {
        choicef = data[0];
    }
}

void TicTacToe::tokenpass() {
    
}

void TicTacToe::end() {//end the game, tells the protokol 
    cout << "Connection closed" << endl;
    exit(0);
}

int TicTacToe::game()
{// below, setup for the game follows
    FrameHandler* dl = new FrameHandler(std::bind(&TicTacToe::data, this, _1), std::bind(&TicTacToe::tokenpass, this), std::bind(&TicTacToe::end, this));
    int player = 1;//which player have turn
    int i;//controls game state
    int choice;//the move the local player have made
    int p1p = 3;//number of pieces not on the board for player one
    int p2p = 3;//number of pieces not on the board for player two
    int pp;//number of pieces not on the board for the current player
    int error = 0;//controls errors on placement of pieces
    int errorLift = 0;//controls errors on lifting of pieces
    char mark;//determines what kind of piece a player puts down
    char a;//used to convert int to char array, for sending data
    vector<char> out;

    cout << "Connecting..." << endl; //tries to connect. ends the game if it fails
    if (!dl->bind(10)) {
        cout << "Connection failed" << endl;
        return -1;
    }
    

    if (local == 0 && dl->getState() == TransmissionState::Primary) {//determines if the local player is player 1 or 2
        local = 1;
    }
    else if(local == 0) {
        local = 2;
    }

    do
    {
        board();//loads the board, in its newest configuration
        player = (player % 2) ? 1 : 2;//determines whos turn it is
        pp = (player == 1) ? p1p : p2p;//ensures that pieces are taken from the right pool of pieces
        cout << "Pieces left:" << pp << endl;//displays the number of pieces

        mark = (player == 1) ? 'X' : 'O';
        if (pp == 0) { //this function will remove one of your pieces, so you have a new piece to put down
            
            
            if (player == local) {//gets the piece that needs to be moved
                cout << "Player " << player << ", remove a number:  ";
                cin >> choice;

                vector<char> data = vector<char>();
                data.push_back(choice);
                dl->sendData(data);
            }
            else {
                cout << "Waiting for player " << player << ", remove a number.";
                choicef = -1;
                while (choicef == -1);
                choice = choicef;
            }
            // picks up a piece
            if (choice == 1 && square[1] == mark)

                square[1] = '1';
            else if (choice == 2 && square[2] == mark)

                square[2] = '2';
            else if (choice == 3 && square[3] == mark)

                square[3] = '3';
            else if (choice == 4 && square[4] == mark)

                square[4] = '4';
            else if (choice == 5 && square[5] == mark)

                square[5] = '5';
            else if (choice == 6 && square[6] == mark)

                square[6] = '6';
            else if (choice == 7 && square[7] == mark)

                square[7] = '7';
            else if (choice == 8 && square[8] == mark)

                square[8] = '8';
            else if (choice == 9 && square[9] == mark)

                square[9] = '9';
            else//gives error for illegal move, and lets the player try again 
            {
                cout << "Invalid move ";
                errorLift = 1;
                if (dl->getState() == TransmissionState::Primary) {
                    cin.ignore();
                    cin.get();
                }
            }
             


            if (player == 1 && errorLift == 0) {//gives back a piece, if there havent been an error
                p1p++;
            }
            else if(errorLift == 0) {
                p2p++;
            }
            else {
                errorLift = 0;
            }
            player--;//ensures the player gets a turn to lay down a piece after lifting a piece
        } else {
            if (player == local) {// if the player have a piece, lets them put it down
                cout << "Player " << player << ", enter a number:  ";
                cin >> choice;

                vector<char> data = vector<char>();
                data.push_back(choice);
                dl->sendData(data);
            }
            else {
                cout << "Waiting for player " << player << ", enter a number.";
                choicef = -1;
                while (choicef == -1);
                choice = choicef;
            }

            if (choice == 1 && square[1] == '1')// puts the piece down if the move is legal

                square[1] = mark;
            else if (choice == 2 && square[2] == '2')

                square[2] = mark;
            else if (choice == 3 && square[3] == '3')

                square[3] = mark;
            else if (choice == 4 && square[4] == '4')

                square[4] = mark;
            else if (choice == 5 && square[5] == '5')

                square[5] = mark;
            else if (choice == 6 && square[6] == '6')

                square[6] = mark;
            else if (choice == 7 && square[7] == '7')

                square[7] = mark;
            else if (choice == 8 && square[8] == '8')

                square[8] = mark;
            else if (choice == 9 && square[9] == '9')

                square[9] = mark;
            else//tells the player the move was illigal, and let them try again
            {
                cout << "Invalid move ";
                error = 1;
                player--;
                if (dl->getState() == TransmissionState::Primary) {
                    cin.ignore();
                    cin.get();
                }
            }

            if (player == 1 && error == 0) { // this function will lower the amount of free pieces
                p1p--;
            }
            else if (player == 2 && error == 0) {
                p2p--;
            }
            else {
                error = 0;
            }
        }
        i = checkwin();//checks if the game have been won

        player++;//changes the player

        if (player != local && dl->getState() == TransmissionState::Primary) {
            dl->passToken();
            this_thread::sleep_for(chrono::milliseconds(1000));
        }
        
    } while (i == -1);
    board();

    if (i == 1)// if the game have ended, determine if the player won, and annouce winner or tie

        cout << "==>\aPlayer " << --player << " win ";
    else
        cout << "==>\aGame draw";

    cout << "Send" << dl->send << endl;
    cout << "Resend " << dl->resend << endl;

    cin.ignore();
    cin.get();

    dl->close();//ends the connection
    
    return 0;
}

/*FUNCTION TO RETURN GAME STATUS
1 FOR GAME IS OVER WITH RESULT
-1 FOR GAME IS IN PROGRESS
O GAME IS OVER AND NO RESULT*/

int TicTacToe::checkwin()
{//checks if theres a winner, or if the game is tied
    if (square[1] == square[2] && square[2] == square[3])

        return 1;
    else if (square[4] == square[5] && square[5] == square[6])

        return 1;
    else if (square[7] == square[8] && square[8] == square[9])

        return 1;
    else if (square[1] == square[4] && square[4] == square[7])

        return 1;
    else if (square[2] == square[5] && square[5] == square[8])

        return 1;
    else if (square[3] == square[6] && square[6] == square[9])

        return 1;
    else if (square[1] == square[5] && square[5] == square[9])

        return 1;
    else if (square[3] == square[5] && square[5] == square[7])

        return 1;
    else if (square[1] != '1' && square[2] != '2' && square[3] != '3'
        && square[4] != '4' && square[5] != '5' && square[6] != '6'
        && square[7] != '7' && square[8] != '8' && square[9] != '9')
        return 0;
    else
        return -1;
}




void TicTacToe::board()
{//draws the board, and displays constant information
    system("cls");
    cout << "\n\n\tTic Tac Toe\n\n";

    cout << "Player 1 (X)  -  Player 2 (O)" << endl << endl;
    cout << "You are " << local << endl;
    cout << endl;

    cout << "     |     |     " << endl;
    cout << "  " << square[1] << "  |  " << square[2] << "  |  " << square[3] << endl;

    cout << "_____|_____|_____" << endl;
    cout << "     |     |     " << endl;

    cout << "  " << square[4] << "  |  " << square[5] << "  |  " << square[6] << endl;

    cout << "_____|_____|_____" << endl;
    cout << "     |     |     " << endl;

    cout << "  " << square[7] << "  |  " << square[8] << "  |  " << square[9] << endl;

    cout << "     |     |     " << endl << endl;
}


