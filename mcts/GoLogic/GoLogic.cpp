#include <iostream>
#include <string>
#ifndef GoLogic_H
#define GoLogic_H 
#include "GoLogic.hpp"
#endif
using namespace std;

int BoardState::BOARD_SIZE = 13;

bool Position::operator==(const Position& other) const{
    return row == other.row && col == other.col;
}

bool Position::operator!=(const Position& other) const{
    return !(*this == other);
}
namespace std
{
    template<> struct hash<Position>{
        size_t operator()(const Position& obj) const{
            return (53 + hash<int>()(obj.row)) * 53 + hash<int>()(obj.col);
        }
    };
}

void BoardState::move(char player, const Position& pos){
    if(!BoardState::validPos(pos)){
        throw "Invalid move: invalid position";
    }
    if(board[pos.row][pos.col] != ' '){
        throw "Invalid move: spot is occupied";
    }

    BoardState current_state_save = *this;  // this calls copy constructor
    num_turns += 1;
    
    board[pos.row][pos.col] = player;
    unordered_set<Position> captured_pieces = get_captured_pieces(player, pos);
    // TODO: make captured pieces blank & add to player's score
    // TODO: check for suicide rule & ko rule
}


unordered_set<Position> BoardState::get_captured_pieces(char player, const Position& position) const{
    auto all_captured = unordered_set<Position>();
    for(auto pos : BoardState::get_surrounding_valid_positions(position)){
        unordered_set<Position> group;
        bool captured;
        tie(group, captured) = get_group_and_is_captured(pos);

        if(captured){
            all_captured.insert(group.begin(),group.end());
        }
    }
    return all_captured;
}

tuple<unordered_set<Position>, bool> BoardState::get_group_and_is_captured(Position& pos) const{
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

// //g++ -Wall --std=c++11 GoLogic.cpp -o GoLogic && ./GoLogic

// int main(){
//     cout << "GoLogic main" << endl;
//     auto board = BoardState();
//     cout << "initialized BoardState" << endl;
//     board.move('W', Position(1, 2));
//     board.move('W', Position(8, 5));

//     cout << board << endl;
//     cout << "done printing" << endl;
//     // return 0;
// }