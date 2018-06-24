#include <vector>
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
    public:
        static int BOARD_SIZE;
        static char other_player(char player){
            return (player == 'W') ? 'B' : 'W';
        }
        static bool validPos(const Position& pos){
            return pos.row >= 0 && pos.row < BoardState::BOARD_SIZE
                    && pos.col >= 0 and pos.col < BoardState::BOARD_SIZE;
        }
        // BoardState() : board(BoardState::BOARD_SIZE, vector<char>(BoardState::BOARD_SIZE) )
        BoardState() : board(BoardState::BOARD_SIZE, vector<char>(BoardState::BOARD_SIZE, ' ')){};
        BoardState(const BoardState& other){
            board = other.board;
            num_turns = other.num_turns;
        }
        void move(char player, const Position& pos);
        vector<Position> get_captured_pieces(char player, const Position& pos) const;
        
};

