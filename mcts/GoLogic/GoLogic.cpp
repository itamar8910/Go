#include <iostream>
#include <string>
#ifndef GoLogic_H
#define GoLogic_H 
#include "GoLogic.hpp"
#endif
using namespace std;

int BoardState::BOARD_SIZE = 13;

bool Position::operator==(const Position& other) const{
    return row == other.row && col == other.col;
}

bool Position::operator!=(const Position& other) const{
    return !(*this == other);
}
namespace std
{
    template<> struct hash<Position>{
        size_t operator()(const Position& obj) const{
            return (53 + hash<int>()(obj.row)) * 53 + hash<int>()(obj.col);
        }
    };
}

void BoardState::move(char player, const Position& pos){
    if(!BoardState::validPos(pos)){
        throw "Invalid move: invalid position";
    }
    if(board[pos.row][pos.col] != ' '){
        throw "Invalid move: spot is occupied";
    }

    //THIS LINE IS PROBABLY THE PROBELM THAT CAUSES ABORT
    BoardState current_state_save = *this;  // this calls copy constructor
    num_turns += 1;
    
    board[pos.row][pos.col] = player;
    vector<Position> captured_pieces = get_captured_pieces(player, pos);
    
}



vector<Position> BoardState::get_captured_pieces(char player, const Position& pos) const{
    return vector<Position>();
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

// //g++ -Wall --std=c++11 GoLogic.cpp -o GoLogic && ./GoLogic
// int main(){
//     cout << "GoLogic main" << endl;
//     auto board = BoardState();
//     cout << "initialized BoardState" << endl;
//     board.move('W', Position(1, 2));

//     cout << board << endl;
//     cout << "done printing" << endl;
//     // return 0;
// }