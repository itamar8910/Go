#include "mcts.hpp"
#include "utils.hpp"
#include "zobrist.hpp"
 
#define START_ZOBRIST_THRESH 300 // num of moves after which we start saving board hashes to avoid infinite rollout

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
Position run_mcts(const BoardState& state, char player, int num_rollouts){

    MCTSNode* root = new MCTSNode(nullptr, Position(-1, -1), player, state);
    for(int rollout_i = 0; rollout_i < num_rollouts; rollout_i++){
        cout << "iteration:" << rollout_i << endl;
        MCTSNode* currentNode = root;
        BoardState currentState(state);
        // selection
        while(currentNode->children.size() == currentNode->unexplored.size()){ // until we are in a leaf node
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
    milliseconds t1 = get_time_ms();
    int currentPlayer = this->player;
    Position move = getRandMove(currentBoardState, currentPlayer);
    int num_pass = 0;
    int num_moves = 0;
    unordered_set<int> savedBoards;
    while(true){
        // cout << move << "," + string(1, currentPlayer) << endl;

        // // FOR DBG
        // if(move != INVALID_POSITION){
        //     currentBoardState.board[move.row][move.col] = 'X';
        //     cout << currentBoardState << endl;
        //     currentBoardState.board[move.row][move.col] = ' ';
        // }else{
        //     cout << currentBoardState << endl;
        // }
        // // END FOR DBG
        /*
            After a certain # of moves,
            we start saving visited board hashes
            in order to avoid inifintie rollouts
            this inforces the superko rule
            for example, a board with 3 ko sitautations
            can cause an inifinite rollout
            without any player passing
        */
        if(num_moves > START_ZOBRIST_THRESH){
            int board_hash = ZobristHashing::getInstance().hashBoard(currentBoardState);
            // int board_hash = 3; 
            if(savedBoards.find(board_hash) != savedBoards.end()){
                // cout << "passed same board twice! exiting" << endl;
                break;
            }else{
                savedBoards.insert(board_hash);
            }
        }

        if(move == INVALID_POSITION){
            num_pass++;
            if(num_pass == 2){
                break;
            }
        }else{
            currentBoardState.move(currentPlayer, move);
            num_pass = 0;
        }
        num_moves++;
        currentPlayer = BoardState::other_player(currentPlayer);
        move = getRandMove(currentBoardState, currentPlayer); 
    }
    double time = ((get_time_ms() - t1).count()) / 1000.0;

    cout << "# moves in rollout:" << num_moves << endl;
    cout << "time:" << time << endl;
    cout << "time/move:" << (time / num_moves) << endl;

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
    float bestScore = -1;
    MCTSNode* bestChild = nullptr;
    for(MCTSNode* child : children){
        float childScore = (child->wins / child->totalGames) + c * sqrt(log(totalGames) / child->totalGames);
        if(childScore > bestScore){
            bestScore = childScore;
            bestChild = child;
        }
    }
    // TODO assert(bestChild != nullptr);
    if(bestChild == nullptr){
        throw "bestChild is null";
    }
    return bestChild;
}

// int main(){
    
//     return 0;
// }