
#include <stdio.h>
#include <iostream>

#include "mcts.hpp"
#include "GoLogic.hpp"
#include "zobrist.hpp"

using namespace std;

int main(){
    BoardState::BOARD_SIZE=13;
    auto board = BoardState();
    // board.move('B', Position(5, 5));
    // board.move('W', Position(6, 5));

    // cout << ZobristHashing::getInstance().hashBoard(board) << endl;
    auto pos = run_mcts(board, 'B');
    cout << pos << endl;
}