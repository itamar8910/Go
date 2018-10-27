
#pragma once
#include "GoLogic.hpp"
#include <iostream>
#include <vector>
#include <algorithm>    // std::random_shuffle

using namespace std;

#define DEFAULT_NUM_ROLLOUTS 100
Position run_mcts(const BoardState& state, char player, int num_rollouts=DEFAULT_NUM_ROLLOUTS);

class MCTSNode{
public:
    MCTSNode* parent;
    const Position movePos;
    const char player;
    int wins, totalGames;
    vector<MCTSNode*> children;
    vector<Position> unexplored;

    MCTSNode(MCTSNode* _parent, const Position& _movePos, char _player, const BoardState& currentState);
    void rollOut(BoardState parentBoardState);
    void expand(const BoardState& currentState);
    MCTSNode* bestChild(float c = 1.41) const;
    // TODO: write destructor
};