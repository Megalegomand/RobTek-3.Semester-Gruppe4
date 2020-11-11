#include <iostream>
using namespace std;

char square[10] = { 'o','1','2','3','4','5','6','7','8','9' };

int checkwin();
void board();

int main()
{
    int player = 1, i, choice;
    int p1p = 3;//number of pieces not on the board for player one
    int p2p = 3;//number of pieces not on the board for player two
    int pp;//number of pieces not on the board for the current player
    int error = 0;//controls errors on placement of pieces
    int errorLift = 0;

    char mark;
    do
    {
        board();
        player = (player % 2) ? 1 : 2;
        pp = (player == 1) ? p1p : p2p;
        cout << pp << endl;

        //cout << "Player " << player << ", enter a number:  ";
        //cin >> choice;

        mark = (player == 1) ? 'X' : 'O';
        if (pp == 0) { //this function will remove one of your pieces, so you have a new piece to put down
            cout << "Player " << player << ", remove a number:  ";
            cin >> choice;
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
            else
            {
                cout << "Invalid move ";
                errorLift = 1;
                cin.ignore();
                cin.get();
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
            cout << "Player " << player << ", enter a number:  ";
            cin >> choice;
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
            else
            {
                cout << "Invalid move ";
                error = 1;
                player--;
                cin.ignore();
                cin.get();
            }
            if (player == 1 && error == 0) { // this function will lower the amount of free pieces, if you had free pieces when your turn started
                p1p--;
            }
            else if (player == 2 && error == 0) {
                p2p--;
            }
            else {
                error = 0;
            }
        }
        i = checkwin();

        
        player++;
        
    } while (i == -1);
    board();
    if (i == 1)

        cout << "==>\aPlayer " << --player << " win ";
    else
        cout << "==>\aGame draw";

    cin.ignore();
    cin.get();
    return 0;
}

/*FUNCTION TO RETURN GAME STATUS
1 FOR GAME IS OVER WITH RESULT
-1 FOR GAME IS IN PROGRESS
O GAME IS OVER AND NO RESULT*/

int checkwin()
{
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


//FUNCTION TO DRAW BOARD OF TIC TAC TOE WITH PLAYERS MARK

void board()
{
    system("cls");
    cout << "\n\n\tTic Tac Toe\n\n";

    cout << "Player 1 (X)  -  Player 2 (O)" << endl << endl;
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