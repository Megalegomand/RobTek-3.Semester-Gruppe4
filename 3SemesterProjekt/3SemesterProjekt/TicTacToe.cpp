// Code from http://www.cppforschool.com/project/tic-tac-toe-project.html
// But modified for use with this project
#include "Tictactoe.h"

TicTacToe::TicTacToe() {}

void TicTacToe::data(vector<char> data) {
    // Get choice
    if (data.size() == 1) {
        // Choice from 1-9, so char can manage
        choicef = data[0];
    }
}

void TicTacToe::tokenpass() {}

void TicTacToe::end() {
    cout << "Connection closed" << endl;
    exit(0);
}

int TicTacToe::game()
{
    // Setup data for new game
    FrameHandler* dl = new FrameHandler(std::bind(&TicTacToe::data, this, _1), std::bind(&TicTacToe::tokenpass, this), std::bind(&TicTacToe::end, this));
    int player = 1; // Which player have turn
    int i; // Controls game state
    int choice; // Move the local player have made
    int p1p = 3; // Number of pieces not on the board for player one
    int p2p = 3; // Number of pieces not on the board for player two
    int pp; // Number of pieces not on the board for the current player
    int error = 0; // Controls errors on placement of pieces
    int errorLift = 0; // Controls errors on lifting of pieces
    char mark; // Determines what kind of piece a player puts down
    char a; // Used to convert int to char array, for sending data
    vector<char> out;

    // Tries to connect. ends the game if it fails
    cout << "Connecting..." << endl; 
    if (!dl->bind(10)) {
        cout << "Connection failed" << endl;
        return -1;
    }
    
    // Determines if the local player is player 1 or 2
    if (local == 0 && dl->getState() == TransmissionState::Primary) { 
        local = 1;
    }
    else if(local == 0) {
        local = 2;
    }

    // Run game
    do
    {
        board(); // Loads the board, in its newest configuration
        player = (player % 2) ? 1 : 2; // Determines whos turn it is
        pp = (player == 1) ? p1p : p2p; // Ensures that pieces are taken from the right pool of pieces
        cout << "Pieces left:" << pp << endl; // Displays the number of pieces

        mark = (player == 1) ? 'X' : 'O';
        if (pp == 0) { // Remove one of your pieces, so you have a new piece to put down
            // Get the piece that needs to be moved
            if (player == local) {
                cout << "Player " << player << ", remove a number:  ";
                cin >> choice;

                vector<char> data = vector<char>();
                data.push_back(choice);
                dl->sendData(data); // Send data using protocol
            }
            else {
                cout << "Waiting for player " << player << ", remove a number.";
                choicef = -1;
                while (choicef == -1);
                choice = choicef;
            }

            // Pick up a piece
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
            else // Invalid move, try again
            {
                cout << "Invalid move ";
                errorLift = 1;
                if (dl->getState() == TransmissionState::Primary) {
                    cin.ignore();
                    cin.get();
                }
            }
             

            // Returns a piece, if no error
            if (player == 1 && errorLift == 0) {
                p1p++;
            }
            else if(errorLift == 0) {
                p2p++;
            }
            else {
                errorLift = 0;
            }

            //ensures the player gets a turn to lay down a piece after lifting a piece
            player--;
        } else {
            if (player == local) { // If the player have a piece, lets them put it down
                cout << "Player " << player << ", enter a number:  ";
                cin >> choice;

                vector<char> data = vector<char>();
                data.push_back(choice);
                dl->sendData(data); // Send move over protocol
            }
            else {
                cout << "Waiting for player " << player << ", enter a number.";
                choicef = -1;
                while (choicef == -1);
                choice = choicef;
            }

            // Put the piece down if the move is legal
            if (choice == 1 && square[1] == '1')

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
            else // Invalid move, try again
            {
                cout << "Invalid move ";
                error = 1;
                player--;
                if (dl->getState() == TransmissionState::Primary) {
                    cin.ignore();
                    cin.get();
                }
            }

            // This function will lower the amount of free pieces
            if (player == 1 && error == 0) { 
                p1p--;
            }
            else if (player == 2 && error == 0) {
                p2p--;
            }
            else {
                error = 0;
            }
        }

        // Checks if the game have been won
        i = checkwin();

        // Next player
        player++;

        if (player != local && dl->getState() == TransmissionState::Primary) {
            dl->passToken(); // Pass token
            this_thread::sleep_for(chrono::milliseconds(1000));
        }
        
    } while (i == -1);

    // Win board
    board();

    // Announce winner or draw
    if (i == 1) 
        cout << "==>\aPlayer " << --player << " win ";
    else
        cout << "==>\aGame draw";

    // Test information
    cout << "Send" << dl->send << endl;
    cout << "Resend " << dl->resend << endl;

    cin.ignore();
    cin.get();

    // Ends the connection
    dl->close();
    
    return 0;
}

/*FUNCTION TO RETURN GAME STATUS
1 FOR GAME IS OVER WITH RESULT
-1 FOR GAME IS IN PROGRESS
O GAME IS OVER AND NO RESULT*/
int TicTacToe::checkwin()
{
    // Checks if theres a winner, or if the game is tied
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
{
    // Draws the board, and displays constant information
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


