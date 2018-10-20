#ifndef GoLogic_H
#define GoLogic_H 
#include "GoLogic.hpp"
#endif
#include <dirent.h> 
#include <stdio.h> 
#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;
milliseconds get_time_ms(){
    return duration_cast< milliseconds >(
    system_clock::now().time_since_epoch()
);
}
// from here: https://stackoverflow.com/questions/23212000/get-the-list-of-files-in-a-directory-with-in-a-directory
vector<string> list_dir(const string& dir_path){
    DIR *dirp;
    struct dirent *directory;
    vector<string> files;
    dirp = opendir(dir_path.c_str());
    if (dirp){
        while ((directory = readdir(dirp)) != NULL){
            if(string(directory->d_name) != "." && string(directory->d_name) != ".."){
                files.push_back(directory->d_name);
            }
        }
        closedir(dirp);
    }
    return files;
}

/*
for 1K games:
NOTE: without -O3 optimization flag:
    - before opt: time on 1k games: 2418ms
    - after 'storing groups' opt: 1451ms
    - after invalid move opt: 840ms
With -O3 optimization flag:
    - 161ms
*/

//g++ -g -Wall --std=c++11 test_performance.cpp GoLogic.cpp -o build/test_performance.o && build/test_performance.o
// with optimization: g++ -Wall -O3 --std=c++11 test_performance.cpp GoLogic.cpp -o build/test_performance.o && build/test_performance.o
int main(void){
    BoardState::BOARD_SIZE = 13;
    string games_dir_path = "tests_data/1000_games/";
    int NUM_GAMES = 1000;
    auto games_sgfs = list_dir("../../" + games_dir_path);
    vector<vector<Move>> games_moves;
  
    int total_moves = 0;
    int i = 0;
    
    for(auto& sgf : games_sgfs){
        cout << "parsing moves of game:" << i++ << endl;
        if(i > NUM_GAMES){
            break;
        }
        auto moves = Move::get_moves(games_dir_path + sgf);
        total_moves += moves.size();
        games_moves.push_back(moves);
    }
    cout << "num of games:" << games_moves.size() << endl;
    
    milliseconds t1 = get_time_ms();
    for(auto& game_moves : games_moves){
        BoardState init_state;
        for(auto& move : game_moves){
            init_state.move(move.player, move.pos);
        }
    }
    long time = (get_time_ms() - t1).count();
    cout << "Time for 1K games:" << time << "ms, Time/Games:" << (time/float(NUM_GAMES)) << "ms, Time/move:" << (time/float(total_moves)) << "ms" << endl;
    return 0;
}