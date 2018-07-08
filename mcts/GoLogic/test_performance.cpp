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
time on 1k games: 2418ms
*/

//g++ -g -Wall --std=c++11 test_performance.cpp GoLogic.cpp -o build/test_performance.o && build/test_performance.o
int main(void){
    BoardState::BOARD_SIZE = 13;
    string games_dir_path = "tests_data/1000_games/";
    auto games_sgfs = list_dir("../../" + games_dir_path);
    vector<vector<Move>> games_moves;
    int i = 0;
    for(auto& sgf : games_sgfs){
        cout << i++ << endl;
        games_moves.push_back(Move::get_moves(games_dir_path + sgf));
    }
    cout << "name of game moves:" << games_moves.size() << endl;
    milliseconds t1 = get_time_ms();

    for(auto& game_moves : games_moves){
        // cout << "game" << endl;
        BoardState init_state;
        for(auto& move : game_moves){
            init_state.move(move.player, move.pos);
        }
        // cout << init_state << endl;
    }
    cout << "done" << endl;
    cout << (get_time_ms() - t1).count() << endl;
    return 0;
}