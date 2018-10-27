#include "utils.hpp"
#include <iostream>
using namespace std;

#include <chrono>
using namespace std::chrono;

bool doesMoveFillEye(Position pos, const BoardState& state, char player){
    for(auto& neigh_pos : BoardState::get_surrounding_valid_positions(pos)){
        if(state.board[neigh_pos.row][neigh_pos.col] != player){
            return false;
        }
    }
    return true;
}
vector<Position> getAllMoves(const BoardState& state, char player){
    vector<Position> moves;
    for(int row = 0; row < (int)state.board.size(); row++){
        for(int col = 0; col < (int)state.board[row].size(); col++){
            try{
                Position movePos = Position(row, col);
                state.assert_move_legality(player, movePos);
                if(!doesMoveFillEye(movePos, state, player)){
                    moves.push_back(Position(row, col));
                }
            }catch(IllegalMove& e){} // TODO: perhaps exceptions hurt performence
        }
    }
    return moves;
}

Position getRandMove(const BoardState& state, char player){
    vector<Position> possibleMoves;
    for(int row = 0; row < (int)state.board.size(); row++){
        for(int col = 0; col < (int)state.board[row].size(); col++){
            if(state.board[row][col] == ' '){
                possibleMoves.push_back(Position(row, col));
            }
        }
    }

    std::random_shuffle ( possibleMoves.begin(), possibleMoves.end() ); // shuffle possibleMoves
    while(possibleMoves.size() > 0){
        Position move = possibleMoves.back();
        possibleMoves.pop_back();
        try{
            state.assert_move_legality(player, move);
            if(!doesMoveFillEye(move, state, player)){
                return move;
            }
        } catch(IllegalMove& e){} // TODO: perhaps exceptions hurt performence
    }
    return INVALID_POSITION;
}

milliseconds get_time_ms(){
    return duration_cast< milliseconds >(
    system_clock::now().time_since_epoch()
    );
}