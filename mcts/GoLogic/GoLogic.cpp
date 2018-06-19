#include <iostream>
#include <string>
#ifndef GoLogic_H
#define GoLogic_H 
#include "GoLogic.hpp"
#endif
using namespace std;

int BoardState::BOARD_SIZE = 13;

void BoardState::move(char player, Position pos){
    // TODO: assert move is legal
    board[pos.row][pos.col] = player;
    // TODO: capture
    
}

ostream& operator<<(ostream& os, const BoardState& board){
    for(int row = 0; row < board.board.size(); row++){
        for(int col = 0; col < board.board[row].size(); col++){
            string end = (col == BoardState::BOARD_SIZE - 1) ? "" : " , ";
            os << board.board[row][col] << end;
        }
        os << endl;

    }
    return os;

}

// int main(){
//     cout << "GoLogic main" << endl;
//     auto board = BoardState();
//     board.move('W', Position(1, 2));
//     cout << board << endl;
//     cout << "done printing" << endl;
//     // return 0;
// }