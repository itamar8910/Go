#include "mcts.hpp"

MCTSNode::MCTSNode(MCTSNode* _parent, const Position& _movePos, char _player): 
                                parent(_parent), movePos(_movePos),
                                player(_player), wins(0), totalGames(0){}

vector<Position> getAllMoves(const BoardState& state, char player){
    vector<Position> moves;
    for(int row = 0; row < (int)state.board.size(); row++){
        for(int col = 0; col < (int)state.board[row].size(); col++){
            try{
                state.assert_move_legality(player, Position(row, col));
                moves.push_back(Position(row, col));
            }catch(IllegalMove& e){}
        }
    }
    return moves;
}

Position run_mcts(const BoardState& state, char player){

    MCTSNode* root = new MCTSNode(nullptr, Position(-1, -1), player);
    MCTSNode* currentNode = root;

    // selection
    BoardState currentState(state);
    while(currentNode->children.size() != 0){ // until we are in a leaft node
        // TODO
        // update current board state as we select nodes
    }

    // expansion
    vector<Position> moves = getAllMoves(currentState, player);
    vector<MCTSNode*> childNodes;
    for(auto& move : moves){
        MCTSNode* childNode = new MCTSNode(currentNode, move, BoardState::other_player(player));
        childNode->rollOut(currentState);
        childNodes.push_back(childNode);
    }
    currentNode->children = childNodes;

}

void MCTSNode::rollOut(BoardState currentBoardState){
    int currentPlayer = player;
    while(true){
        vector<Position> validMoves = getAllMoves(currentBoardState, currentPlayer);
        if(validMoves.size() == 0){
            break;
        }
        Position movePos = validMoves[rand() % (int)validMoves.size()]; 
        currentBoardState.move(currentPlayer, movePos);
        currentPlayer = BoardState::other_player(currentPlayer);
    }

    //TODO: impl. teritorry counting
    //TODO: check who won
    //TODO: propagate result to acnestors

    
}