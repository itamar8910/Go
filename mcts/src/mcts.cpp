#include "mcts.hpp"


// void MCTSNode::expand(const BoardState& currentState){
//         // add a child node for every move
//         vector<Position> moves = getAllMoves(currentState, player);
//         vector<MCTSNode*> childNodes;
//         for(auto& move : moves){
//             MCTSNode* childNode = new MCTSNode(this, move, BoardState::other_player(player));
//             childNodes.push_back(childNode);
//         }
//         this->children = childNodes;
// }

vector<Position> getAllMoves(const BoardState& state, char player){
    vector<Position> moves;
    for(int row = 0; row < (int)state.board.size(); row++){
        for(int col = 0; col < (int)state.board[row].size(); col++){
            try{
                state.assert_move_legality(player, Position(row, col));
                moves.push_back(Position(row, col));
            }catch(IllegalMove& e){} // TODO: perhaps exceptions hurt performence
        }
    }
    return moves;
}

MCTSNode::MCTSNode(MCTSNode* _parent, const Position& _movePos, char _player, const BoardState& currentState): 
                                parent(_parent), movePos(_movePos),
                                player(_player), wins(0), totalGames(0){
    unexplored = getAllMoves(currentState, player);
    std::random_shuffle ( unexplored.begin(), unexplored.end() ); // shuffle unexplored moves
}
Position run_mcts(const BoardState& state, char player){

    MCTSNode* root = new MCTSNode(nullptr, Position(-1, -1), player, state);
    int NUM_ROLLOUTS = 100;
    for(int rollout_i = 0; rollout_i < NUM_ROLLOUTS; rollout_i++){
        MCTSNode* currentNode = root;
        BoardState currentState(state);
        // selection
        while(currentNode->unexplored.size() > 0){ // until we are in a leaf node
            // select a node out of children
            currentNode = currentNode->bestChild();
            // update current board state as we select nodes
            currentState.move(currentNode->player, currentNode->movePos);
        }


        // expand unexplored move
        Position unexploredPos = currentNode->unexplored.back();
        currentNode->unexplored.pop_back();
        MCTSNode* childNode = new MCTSNode(currentNode, unexploredPos, BoardState::other_player(currentNode->player), currentState);
        childNode->rollOut(currentState);
        currentNode->children.push_back(childNode);
    }

    // finally, choose child of root with highest win ratio
    return root->bestChild(0)->movePos; 

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


MCTSNode* MCTSNode::bestChild(float c) const{
    float bestScore = 0;
    MCTSNode* bestChild = nullptr;
    for(MCTSNode* child : children){
        float childScore = (child->wins / child->totalGames) + c * sqrt(log(totalGames) / child->totalGames);
        if(childScore > bestScore){
            bestScore = childScore;
            bestChild = child;
        }
    }
    // TODO: wrap in ifdef DBG
    // TODO assert(bestChild != nullptr);
    return bestChild;
}

// int main(){
    
//     return 0;
// }