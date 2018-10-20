
#include "GoLogic.hpp"
#include <iostream>
#include <vector>
#include <algorithm>    // std::random_shuffle

using namespace std;

Position run_mcts(const BoardState& state, char player);

class MCTSNode{
public:
    MCTSNode* parent;
    const Position movePos;
    char player;
    int wins, totalGames;
    vector<MCTSNode*> children;
    vector<Position> unexplored;

    MCTSNode(MCTSNode* _parent, const Position& _movePos, char _player, const BoardState& currentState);
    void rollOut(BoardState parentBoardState);
    void expand(const BoardState& currentState);
    MCTSNode* bestChild(float c = 1.41) const;
};