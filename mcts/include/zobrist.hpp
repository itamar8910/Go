#pragma once
#include "GoLogic.hpp"
#include <iostream>

using namespace std;

/**
 * Utility class for hashing boards with Zobrist hashing
 * https://en.wikipedia.org/wiki/Zobrist_hashing
 */
class ZobristHashing{

    private:
        static ZobristHashing* instance;
        vector<vector<vector<int> > > boardRands;
        ZobristHashing();
    public:
        static ZobristHashing& getInstance(){
            if(instance == nullptr){
                instance = new ZobristHashing();
            }
            return *instance;
        }
        int hashBoard(const BoardState& boardState) const;
};
// #ifndef zeroed_zobrist_instance
// #define zeroed_zobrist_instance 1
// #endif
