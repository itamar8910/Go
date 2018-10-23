#include "utils.hpp"


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
    vector<Position> validMoves = getAllMoves(state, player);
    if(validMoves.size() == 0){
        return INVALID_POSITION;
    }
    return validMoves[rand() % (int)validMoves.size()]; 
}