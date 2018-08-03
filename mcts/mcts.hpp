
#include "GoLogic/GoLogic.hpp"
#include <iostream>
#include <vector>
using namespace std;

Position run_mcts(const BoardState& state, char player);

class MCTSNode{
public:
    MCTSNode* parent;
    const Position& movePos;
    char player;
    int wins, totalGames;
    vector<MCTSNode*> children;

    MCTSNode(MCTSNode* _parent, const Position& _movePos, char _player);
    void rollOut(BoardState parentBoardState);
};