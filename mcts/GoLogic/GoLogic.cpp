#include <iostream>
#include <string>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <sstream>
#include <iterator>
#ifndef GoLogic_H
#define GoLogic_H 
#include "GoLogic.hpp"
#endif
#define PROJECT_ROOT string("../../")
using namespace std;

int BoardState::BOARD_SIZE = 13;

bool Position::operator==(const Position& other) const{
    return row == other.row && col == other.col;
}

bool Position::operator!=(const Position& other) const{
    return !(*this == other);
}

ostream& operator<<(ostream& os, const Position& pos){
    return os << "[" << pos.row << "," << pos.col << "]";
}

ostream& operator<<(ostream& os, const Move& move){
    return os << "[" << move.player << "," << move.pos << "]";
}

void BoardState::assert_move_legality(char player, const Position& pos) const{
    if(!BoardState::validPos(pos)){
        throw IllegalMove("invalid position");
    }
    if(board[pos.row][pos.col] != ' '){
        throw IllegalMove("spot is occupied");
    }
    if(pos == ko_pos){  // check for ko
        throw IllegalMove("KO");
    }
    // check for suicide
    bool is_suicide = true;
    for(auto& neigh : BoardState::get_surrounding_valid_positions(pos)){
        if(board[neigh.row][neigh.col] == ' '){ // has a neighbor that's an empty spot
            is_suicide = false;
            break;
        }else {
            auto itr = pos_to_group.find(neigh);
            if(itr == pos_to_group.end()){ // TODO: this is only for debug
                throw "ERROR: checking suicide: neighbor stone without group";
            }
            Group& neigh_group = *(itr->second);
            if(neigh_group.liberties.size() == 0){ // TODO: this is only for debug
                throw "ERROR: checking suicide: group with zero liberties is on the board";
            }
            if(neigh_group.liberties.find(pos) == neigh_group.liberties.end()){
                    throw "ERROR: checking suicide: previously empty spot is not in enemy group's liberties";
            }
            // if enemy group but capturing it - not a suicide
            if(board[neigh.row][neigh.col] == BoardState::other_player(player) && neigh_group.liberties.size() == 1){
                is_suicide = false;
                break;
            }
            // if friendly group but has more than one liberty - not a suicide
            if(board[neigh.row][neigh.col] == player && neigh_group.liberties.size() > 1){
                is_suicide = false;
                break;
            }
        }
    }
    if(is_suicide){
        throw IllegalMove("Suicide");
    }
    
}

void BoardState::move(char player, const Position& pos){
    if(pos.row == -1 && pos.col == -1){ // pass move
        ko_pos = Position(-1, -1);  // reset ko position
        // TODO: handle game end after 2 consecutive passes
        return;
    }

    assert_move_legality(player, pos);
 
    num_turns += 1;
    board[pos.row][pos.col] = player;

    auto udpate_groups_res = update_groups(pos, player);
    auto& captured_pieces = get<0>(udpate_groups_res);
    player_to_captures[player] += captured_pieces.size();
    for(auto& pos : captured_pieces){
        board[pos.row][pos.col] = ' ';
    }

    //update the "KO point"
    // count num of enemy neighbors
    unsigned int num_enemy_neighbors = 0;
    auto surrounding_valid = BoardState::get_surrounding_valid_positions(pos);
    for(auto& neigh_pos : surrounding_valid){
        if(board[neigh_pos.row][neigh_pos.col] == BoardState::other_player(player)){
            num_enemy_neighbors += 1;
        }
    }
    if(num_enemy_neighbors == surrounding_valid.size()){ // TODO: this is for dbg, remove laterd
        throw "Error: should have detected suicide";
    }
    // if has 3 enemy neighbors, and has captured the previously fourth, then the position of the captured enemy is the new ko point
    if(num_enemy_neighbors == surrounding_valid.size() - 1 && captured_pieces.size() == 1){
        ko_pos = captured_pieces[0];
    }else{
        ko_pos = Position(-1, -1);
    }

}


unordered_set<Position> BoardState::get_captured_pieces(char player, const Position& position) const{
    auto all_captured = unordered_set<Position>();
    for(auto pos : BoardState::get_surrounding_valid_positions(position)){
        if(! (board[pos.row][pos.col] == BoardState::other_player(player))){
            continue;
        }
        unordered_set<Position> group;
        bool captured;
        // NOTE: we can have optimized by saving groups we have already found & checking if neighboring Position's group has already been computed
        // but in performance testing on 1K games, it actually slows the run down abit (2450ms -> 2550ms)
        tie(group, captured) = get_group_and_is_captured(pos);

        if(captured){
            all_captured.insert(group.begin(),group.end());
        }
    }
    return all_captured;
}

tuple<unordered_set<Position>, bool> BoardState::get_group_and_is_captured(const Position& pos) const{
    char player = board[pos.row][pos.col];
    auto visited = unordered_set<Position>();
    bool captured = true;

    //bfs
    auto q = queue<Position>();
    q.push(pos);
    visited.insert(pos);

    while(!q.empty()){
        auto current_pos = q.front();
        q.pop();
        visited.insert(current_pos);
        auto valid_surrounding = BoardState::get_surrounding_valid_positions(current_pos);
        for(auto surrounding : valid_surrounding){
            if(board[surrounding.row][surrounding.col] == ' '){
                captured = false;
            }
        }
        auto player_neighbors_unvisited = vector<Position>();
        for(auto sur : valid_surrounding){
            if(board[sur.row][sur.col] == player && visited.find(sur) == visited.end()){
                q.push(sur);
                visited.insert(sur);
            }
        }
    }
    return make_tuple(visited, captured);
}

tuple<vector<Position>, Group*> BoardState::update_groups(const Position& pos, char color){
    vector<Position> captured;
    auto neighbors = BoardState::get_surrounding_valid_positions(pos);
    Group* my_group = new Group();
    my_group->stones.insert(pos);
    my_group->color = color;
    unordered_set<Group*> deleted_groups;

    for(auto& neigh : neighbors){
        if(board[neigh.row][neigh.col] == BoardState::other_player(color)){ // enemy neighbor
            auto itr = pos_to_group.find(neigh);
            if(itr == pos_to_group.end()){ // this happens when this stone has already been captured (the capture happened in a previous iteration, on a diff neighbor, in this function)
                continue;
            }
            Group& enemy_group = *(itr->second);
            auto pos_itr = enemy_group.liberties.find(pos);
            if(pos_itr != enemy_group.liberties.end()){
                enemy_group.liberties.erase(pos_itr); // remove played stone from liberties
            }
            if(enemy_group.liberties.size() == 0){ // if enemy group is captured
                for(auto& captured_enemy : enemy_group.stones){
                    captured.push_back(captured_enemy);
                    pos_to_group.erase(captured_enemy);
                    for(auto& neigh2 : BoardState::get_surrounding_valid_positions(captured_enemy)){ // restore liberties (because stone enemy is removed)
                        if(neigh2 == pos){
                            my_group->liberties.insert(captured_enemy);
                        }
                        else if(board[neigh2.row][neigh2.col] == color){
                            auto neigh2_itr = pos_to_group.find(neigh2);
                            if(neigh2_itr == pos_to_group.end()){ // TODO: this is only for debug
                                throw "ERROR: neigh2 stone without group";
                            }
                            Group& neigh2_group = *(neigh2_itr->second);
                            neigh2_group.liberties.insert(captured_enemy);
                        }

                    }
                }
                // in order not to delete a group ptr twice
                if(deleted_groups.find(&enemy_group) == deleted_groups.end()){
                    delete &enemy_group;
                    deleted_groups.insert(&enemy_group);
                }
            }
        } else if(board[neigh.row][neigh.col] == color){ // friendly neighbor
            auto itr = pos_to_group.find(neigh);
            if(itr == pos_to_group.end()){ // TODO: this is only for debug
                throw "ERROR: friendly neighbor stone without group";
            }
            Group* neigh_group = itr->second;
            //don't merge if they are the same group
            if(neigh_group == my_group){
                continue;
            }
            // merge my_group in neigh_group
            // & switch everything that pointed into my_group to point into neigh_group
            for(auto& stone : my_group->stones){
                neigh_group->stones.insert(stone);
                pos_to_group[stone] = neigh_group;
            }
            // merge liberties into neigh_group
            for(auto& liberty : my_group->liberties){
                neigh_group->liberties.insert(liberty);
            }
            // delete my_group & swap it to point at neigh_group
            delete my_group;
            my_group = neigh_group;
        }else{ // empty neighboring spot
            my_group->liberties.insert(neigh);
        }
    }
    my_group->liberties.erase(pos); // remove this stone from its group's liberties
    
    // set stone to point to my group
    pos_to_group[pos] = my_group;
    return make_tuple(captured, my_group);
}



ostream& operator<<(ostream& os, const BoardState& board){
    for(auto row_itr = board.board.begin(); row_itr != board.board.end(); ++row_itr){
        for(auto val_itr = row_itr->begin(); val_itr != row_itr->end(); ++val_itr){
            string end = (val_itr == --row_itr->end()) ? " |" : "  ";
            os << *val_itr << end;
        }
        os << endl;

    }
    return os;

}
// from here: https://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c-using-posix
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}

// from here: https://stackoverflow.com/questions/236129/the-most-elegant-way-to-iterate-the-words-of-a-string
template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

vector<Move> Move::get_moves(const string& sgf_path){
    // calling python code to get the moves in string format
    string project_root = PROJECT_ROOT;
    string rel_python_path = "venv/bin/python ";
    string python_path = project_root + rel_python_path;
    string rel_parser_script_path = "go_logic/sgf_parser.py ";
    string parser_script_path = project_root + rel_parser_script_path;
    string command = python_path + parser_script_path + project_root + sgf_path;
    cout << command << endl;
    string res_str = exec(command.c_str());
    vector<Move> moves;
    vector<string> lines = split(res_str, '\n');
    for(auto& line : lines){
        if(line.length() == 0){
            continue;
        }
        vector<string> chars = split(line, ',');
        string player = chars[0];
        string row = chars[1];
        string col = chars[2];
        moves.push_back(Move(player[0], Position(stoi(row), stoi(col))));
    }
    return moves;
}


//g++ -Wall --std=c++11 GoLogic.cpp -o build/GoLogic.o && build/GoLogic.o

// int main(){
//     cout << "GoLogic main" << endl;
//     // cout << exec("pwd") << endl;
//     auto moves = Move::get_moves("tests_data/game1.sgf");
//     for(auto& move : moves){
//         cout << move << endl;
//     }
//     // auto board = BoardState();
//     // cout << "initialized BoardState" << endl;
//     // board.move('W', Position(1, 2));
//     // board.move('W', Position(8, 5));

//     // cout << board << endl;
//     // cout << "done printing" << endl;
//     // cout << board.player_to_captures['W'] << endl;
//     // cout << board.player_to_captures['B'] << endl;
//     // return 0;
// }
