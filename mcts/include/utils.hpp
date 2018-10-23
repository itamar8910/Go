#pragma once
#include "GoLogic.hpp"



vector<Position> getAllMoves(const BoardState& state, char player);
Position getRandMove(const BoardState& state, char player);

/*
returns 2D board, board[row][col] = <player> if (row, col) is in
<player>'s territory, else ' '
*/
vector<vector<char>> getTerritory(const BoardState& state);

