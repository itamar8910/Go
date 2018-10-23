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

void fillTerritory(const BoardState& state, vector<vector<char>>& territoryBoard, Position initPos){
    bool owns_territory = true;
    auto visited = unordered_set<Position>();
    auto q = queue<Position>();
    q.push(initPos);
    visited.insert(initPos);
    char player = 'X'; // player is X while we haven't found some player's stones in the edges of the territory
    while(owns_territory && !q.empty()){
        Position current = q.front();
        q.pop();
        for(auto neighPos : BoardState::get_surrounding_valid_positions(current)){
            char neigh = state.board[neighPos.row][neighPos.col];
            if(player == 'X' && neigh != ' '){
                player = neigh;
            }
            if(neigh == player){
                continue;
            } else if(player != 'X' && neigh == BoardState::other_player(player)){ // if both player share the borders of the territory - this territory is undecided
                owns_territory = false;
                break;
            }else if(neigh == ' ' && visited.find(neighPos) == visited.end()){ // add empty spot to potential territory
                q.push(neighPos);
                visited.insert(neighPos);
            }
        }
    }
    if(player != 'X' && owns_territory){ // mark territory
        for(auto pos : visited){
            territoryBoard[pos.row][pos.col] = player;
        }
    }
}

vector<vector<char>> getTerritory(const BoardState& state){
    vector<vector<char>> territoryBoard = vector<vector<char>>(BoardState::BOARD_SIZE, vector<char>(BoardState::BOARD_SIZE, ' '));
    for(int row = 0; row < BoardState::BOARD_SIZE; row++){
        for(int col = 0; col < BoardState::BOARD_SIZE; col++){
            if(state.board[row][col] != ' ' || territoryBoard[row][col] != ' '){
                continue;
            }
            fillTerritory(state, territoryBoard, Position(row, col));
        }
    }
    return territoryBoard;
}