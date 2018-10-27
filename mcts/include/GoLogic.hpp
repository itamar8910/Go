
#pragma once
#include <vector>
#include <unordered_set>
#include <queue>
#include <map>
#include <algorithm>
#include <deque>
#include <unordered_map>
#include <iostream>

using namespace std;

// TODO: impl. operator== and hash on Position in order to put it in unordered_set inside BoardState::get_captured_pieces
// relevant SO thread (but he imlemented a bad hash): https://stackoverflow.com/questions/42359988/setting-a-custom-class-to-be-used-with-unordered-set-element-cant-be-found-in
struct Position{
    int row;
    int col;
    Position(int _row, int _col): row(_row), col(_col) {}
    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;
};

#define INVALID_POSITION Position(-1, -1)

// define hash of Position
namespace std
{
    template<> struct hash<Position>{
        size_t operator()(const Position& obj) const{
            return (53 + hash<int>()(obj.row)) * 53 + hash<int>()(obj.col);
        }
    };
}

struct Move{
    char player;
    Position pos;
    Move(char _player, Position _pos) : player(_player), pos(_pos) {}
    static vector<Move> get_moves(const string& sgf_path);
};


struct IllegalMove : public exception
{
    string msg;
    IllegalMove(string _msg): msg(_msg){}
	const char * what () const throw ()
    {
    	return (string("Illegal move: ") + msg).c_str();
    }
};

struct Group
{
    char color;
    unordered_set<Position> stones;
    unordered_set<Position> liberties;
};

class BoardState{
    public:
        vector<vector<char>> board;
        int num_turns;
        map<char, int> player_to_captures;
        unordered_map<Position, Group*> pos_to_group; 
        Position ko_pos;

        static int BOARD_SIZE;
        static char other_player(char player){
            return (player == 'W') ? 'B' : 'W';
        }
        static bool validPos(const Position& pos){
            return pos.row >= 0 && pos.row < BoardState::BOARD_SIZE
                    && pos.col >= 0 and pos.col < BoardState::BOARD_SIZE;
        }
        static vector<Position> get_surrounding_valid_positions(const Position& pos){
            vector<Position> deltas = {Position(1, 0), Position(-1, 0), Position(0, 1), Position(0, -1)};
            auto options =  vector<Position>();
            for(const auto& delta : deltas){
                Position option = Position(pos.row + delta.row, pos.col + delta.col);
                if(BoardState::validPos(option)){
                    options.push_back(Position(option));
                }
            }
            return options;
        }
        

        BoardState() : board(BoardState::BOARD_SIZE, vector<char>(BoardState::BOARD_SIZE, ' ')),
                                num_turns(0),
                                player_to_captures({{'W', 0}, {'B', 0}}),
                                 ko_pos(-1, -1){};



        tuple<unordered_set<Position>, bool> get_group_and_is_captured(const Position& pos) const;
        void assert_move_legality(char player, const Position& pos) const;
        BoardState(const vector<vector<char>>& _board): player_to_captures({{'W', 0}, {'B', 0}}), ko_pos(-1, -1){
            board = _board;
            for(int row = 0; row < BOARD_SIZE; row++){
                for(int col = 0;  col < BOARD_SIZE; col++){
                    if(board[row][col] != ' '){
                        num_turns += 1;
                        if(pos_to_group.find(Position(row, col)) == pos_to_group.end()){
                            auto group_stones = get<0>(get_group_and_is_captured(Position(row, col)));
                            Group* group = new Group();
                            group->stones.insert(Position(row, col));
                            group->color = board[row][col];
                            pos_to_group[Position(row, col)] = group;
                            for(auto& stone : group_stones){
                                pos_to_group[stone] = group;
                                group->stones.insert(stone);
                                for(auto& neigh : BoardState::get_surrounding_valid_positions(stone)){
                                    if(board[neigh.row][neigh.col] == ' '){
                                        group->liberties.insert(neigh);
                                    }
                                }
                            }
                        }
                        
                        // update_groups(Position(row, col), board[row][col]);
                    }
                }
            }
        }
        void move(char player, const Position& pos);
        unordered_set<Position> get_captured_pieces(char player, const Position& pos) const;
        BoardState(const BoardState& other) : board(other.board), num_turns(other.num_turns), player_to_captures(other.player_to_captures),
                                                ko_pos(other.ko_pos){
            // deep copy pos_to_group
           for(auto it : other.pos_to_group){
               if(this->pos_to_group.find(it.first) == this->pos_to_group.end()){
                   Group* group = it.second; 
                   Group* group_cpy = new Group(*group);
                   for(auto pos : group->stones){
                       this->pos_to_group[pos] = group_cpy;
                   }
               }
           }
        }

        ~BoardState(){
            unordered_set<Group*> deleted;
            for(auto it : pos_to_group){
            // in order not to delete a group ptr twice

                if(deleted.find(it.second) == deleted.end()){
                    delete it.second;
                    deleted.insert(it.second);
                }
            }
        }
    /**
    returns 2D board, board[row][col] = <player> if (row, col) is in
    <player>'s territory, else ' '

    A territory belongs to <player> if it's surrounded only by <players>'s stones
    */
    vector<vector<char>> getTerritory() const;

    /**
     * Returns: (White score, Black score)
     * 
     * Using Area scoring: https://senseis.xmp.net/?Scoring
     */
    pair<float, float> getScore() const;

    private:
    // returns (captured pieces, my group)
    tuple<vector<Position>, Group*> update_groups(const Position& pos, char color);

};



ostream& operator<<(ostream& os, const BoardState& board);
ostream& operator<<(ostream& os, const Position& move);
ostream& operator<<(ostream& os, const Move& move);