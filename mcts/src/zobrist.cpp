
#include "zobrist.hpp"
#include "GoLogic.hpp"
#include <vector>

#define ZOBRIST_DEPTH 2
#define THREE_D_INDEX(row, col, i) row * BoardState::BOARD_SIZE*ZOBRIST_DEPTH + col*ZOBRIST_DEPTH + i
ZobristHashing* ZobristHashing::instance = nullptr;

ZobristHashing::ZobristHashing(){
    // TODO: check if below intialization is correct. Might be causing a segfault.
    boardRands = new int[BoardState::BOARD_SIZE * BoardState::BOARD_SIZE * ZOBRIST_DEPTH];
    for(int row = 0; row < BoardState::BOARD_SIZE; row++){
        for(int col = 0; col < BoardState::BOARD_SIZE; col++){
            for(int val = 0; val < ZOBRIST_DEPTH; val++){
                boardRands[THREE_D_INDEX(row, col, val)] = rand();
            }
        }
    }
}

int ZobristHashing::hashBoard(const BoardState& boardState) const{
    int h = 0;
    for(int row = 0; row < BoardState::BOARD_SIZE; row++){
        for(int col = 0; col < BoardState::BOARD_SIZE; col++){
            if(boardState.board[row][col] != ' '){
                int depthIndex = (boardState.board[row][col] == 'W') ? 0 : 1;
                h ^= boardRands[THREE_D_INDEX(row, col, depthIndex)];
            }  
        }
    }
       return h;
}