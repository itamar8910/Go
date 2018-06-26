#include <vector>
#include <unordered_set>
#include <queue>
#include <map>
#include <algorithm>

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

struct Move{
    char player;
    Position pos;
    Move(char _player, Position _pos) : player(_player), pos(_pos) {}
};

class BoardState{
    public:
        vector<vector<char>> board;
        int num_turns;
        map<char, int> player_to_captures;
        // TODO: add past_2_boards for ko check
    public:
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

        // BoardState() : board(BoardState::BOARD_SIZE, vector<char>(BoardState::BOARD_SIZE) )
        BoardState() : board(BoardState::BOARD_SIZE, vector<char>(BoardState::BOARD_SIZE, ' ')),
                                num_turns(0),
                                player_to_captures({{'W', 0}, {'B', 0}}){};
        BoardState(const BoardState& other){
            board = other.board;
            num_turns = other.num_turns;
        }
        void move(char player, const Position& pos);
        unordered_set<Position> get_captured_pieces(char player, const Position& pos) const;
        tuple<unordered_set<Position>, bool> get_group_and_is_captured(const Position& pos) const;
};

// define hash of Position
namespace std
{
    template<> struct hash<Position>{
        size_t operator()(const Position& obj) const{
            return (53 + hash<int>()(obj.row)) * 53 + hash<int>()(obj.col);
        }
    };
}