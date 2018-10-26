#include "mcts.hpp"
#include "utils.hpp"

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
    // use c=0 to not account for exploration
    return root->bestChild(0)->movePos; 
}

void MCTSNode::rollOut(BoardState currentBoardState){
    int currentPlayer = this->player;
    Position move = getRandMove(currentBoardState, currentPlayer);
    int num_pass = 0;
    while(true){
        if(move == INVALID_POSITION){
            num_pass++;
            if(num_pass == 2){
                break;
            }
        }else{
            currentBoardState.move(currentPlayer, move);
            num_pass = 0;
        }
        currentPlayer = BoardState::other_player(currentPlayer);
        move = getRandMove(currentBoardState, currentPlayer); 
    }


    auto score = currentBoardState.getScore();
    char winner = (score.first > score.second) ? 'W' : 'B';

    //propagate result to acnestors

    MCTSNode* current = this;
    while(current != nullptr){
        current->totalGames++;
        current->wins += (current->player == winner) ? 1 : 0;
        current = current->parent;
    }

    
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