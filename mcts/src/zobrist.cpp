
#include "zobrist.hpp"
#include "GoLogic.hpp"
#include <vector>


ZobristHashing* ZobristHashing::instance = nullptr;

ZobristHashing::ZobristHashing(){
    // TODO: check if below intialization is correct. Might be causing a segfault.
    boardRands = vector<vector<vector<int > > >(BoardState::BOARD_SIZE, vector<vector<int > >(BoardState::BOARD_SIZE, vector<int>(2, 0)));
    for(int row = 0; row < BoardState::BOARD_SIZE; row++){
        for(int col = 0; col < BoardState::BOARD_SIZE; col++){
            for(int val = 0; val < 2; val++){
                boardRands[row][col][val] = rand();
            }
        }
    }
}

int ZobristHashing::hashBoard(const BoardState& boardState) const{
    int h = 0;
    for(int row = 0; row < BoardState::BOARD_SIZE; row++){
        for(int col = 0; col < BoardState::BOARD_SIZE; col++){
            if(boardState.board[row][col] != ' '){
                h ^= (boardState.board[row][col] == 'W') ? boardRands[row][col][0] : boardRands[row][col][1];
            } 
        }
    }
    return h;
}