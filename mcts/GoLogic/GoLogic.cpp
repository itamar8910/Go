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

void BoardState::move(char player, const Position& pos){
    if(!BoardState::validPos(pos)){
        throw IllegalMove("invalid position");
    }
    if(board[pos.row][pos.col] != ' '){
        throw IllegalMove("spot is occupied");
    }


    auto board_save = this->board;  // this calls copy constructor
    
    num_turns += 1;

    board[pos.row][pos.col] = player;
    auto udpate_groups_res = update_groups(pos, player);
    auto& captured_pieces = get<0>(udpate_groups_res);
    Group& my_group = *(get<1>(udpate_groups_res));
    player_to_captures[player] += captured_pieces.size();
    for(auto& pos : captured_pieces){
        board[pos.row][pos.col] = ' ';
    }

    // check for suicide rule violation
    if (my_group.liberties.size() == 0){
        board = board_save; // TODO: we also need to rollback groups, but leaving it here for performance measurement
        player_to_captures[player] -= captured_pieces.size();
        throw IllegalMove("Suicide");
    }
    // check for KO rule violation
    // TODO: check for KO more efficiently by keeping track of a "KO point"
    if(past_two_boards.size() == 2 && board == past_two_boards.get(0)){
        board = board_save; // TODO: we also need to rollback groups, but leaving it here for performance measurement
        player_to_captures[player] -= captured_pieces.size();
        throw IllegalMove("KO");
    }
    past_two_boards.push_back(board);
    // unordered_set<Position> captured_pieces = get_captured_pieces(player, pos);
    
    // // add to player's score
    // player_to_captures[player] += captured_pieces.size();
    // // make captured pieces blank
    // for(auto& pos : captured_pieces){
    //     board[pos.row][pos.col] = ' ';
    // }

    // // check for suicide rule violation
    // if(get<1>(get_group_and_is_captured(pos))){
    //     // rollback board & score
    //     board = board_save;
    //     player_to_captures[player] -= captured_pieces.size();
    //     throw IllegalMove("Suicide");
    // }

    // // check for KO rule violation
    // if(past_two_boards.size() == 2 && board == past_two_boards.get(0)){
    //     // rollback board & score
    //     board = board_save;
    //     player_to_captures[player] -= captured_pieces.size();
    //     throw IllegalMove("KO");
    // }

    // past_two_boards.push_back(board);
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
    my_group->stones.push_back(pos);
    my_group->color = color;
    for(auto& neigh : neighbors){
        if(board[neigh.row][neigh.col] == BoardState::other_player(color)){ // enemy neighbor
            auto itr = pos_to_group.find(neigh);
            if(itr == pos_to_group.end()){ // TODO: this is only for debug
                throw "ERROR: enemy stone without group";
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
                }
                delete &enemy_group;
            }
        } else if(board[neigh.row][neigh.col] == color){ // friendly neighbor
            auto itr = pos_to_group.find(neigh);
            if(itr == pos_to_group.end()){ // TODO: this is only for debug
                throw "ERROR: friendly neighbor stone without group";
            }
            Group* neigh_group = itr->second;
            // merge my_group in neigh_group
            // & switch everything that pointed into my_group to point into neigh_group
            for(auto& stone : my_group->stones){
                neigh_group->stones.push_back(stone);
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
            string end = (val_itr == --row_itr->end()) ? "" : " , ";
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
