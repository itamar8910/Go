#pragma once
#include "GoLogic.hpp"

#include <chrono>
using namespace std::chrono;


vector<Position> getAllMoves(const BoardState& state, char player);
Position getRandMove(const BoardState& state, char player);
milliseconds get_time_ms();


