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
    unordered_set<Position> captured_pieces = get_captured_pieces(player, pos);
    
    // add to player's score
    player_to_captures[player] += captured_pieces.size();
    // make captured pieces blank
    for(auto& pos : captured_pieces){
        board[pos.row][pos.col] = ' ';
    }

    // check for suicide rule violation
    if(get<1>(get_group_and_is_captured(pos))){
        // rollback board & score
        board = board_save;
        player_to_captures[player] -= captured_pieces.size();
        throw IllegalMove("Suicide");
    }

    // check for KO rule violation
    if(past_two_boards.size() == 2 && board == past_two_boards.get(0)){
        // rollback board & score
        board = board_save;
        player_to_captures[player] -= captured_pieces.size();
        throw IllegalMove("KO");
    }

    past_two_boards.push_back(board);
}


unordered_set<Position> BoardState::get_captured_pieces(char player, const Position& position) const{
    auto all_captured = unordered_set<Position>();
    for(auto pos : BoardState::get_surrounding_valid_positions(position)){
        if(! (board[pos.row][pos.col] == BoardState::other_player(player))){
            continue;
        }
        unordered_set<Position> group;
        bool captured;
        // TODO: we can optimize by saving groups we have already found & checking if neighboring Position's group has already been computed
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
