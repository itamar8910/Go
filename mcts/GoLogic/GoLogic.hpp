#include <vector>
using namespace std;
struct Position{
    int row;
    int col;
    Position(int _row, int _col): row(_row), col(_col) {}
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
        // BoardState() : board(BoardState::BOARD_SIZE, vector<char>(BoardState::BOARD_SIZE) )
        BoardState() : board(BoardState::BOARD_SIZE, vector<char>(BoardState::BOARD_SIZE, ' ')){};
        // BoardState(){
        //     board = vector<vector<char>>(13, vector<char>(13, ' '));
        // };
        void move(char player, Position pos);
        
};

