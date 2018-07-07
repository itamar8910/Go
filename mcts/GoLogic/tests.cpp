#include <iostream>

#ifndef GoLogic_H
#define GoLogic_H 
#include "GoLogic.hpp"
#endif

using namespace std;

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_FAST_COMPILE
#include "catch.hpp"

//command to build & run: g++ -std=c++11 -o build/tests.o tests.cpp GoLogic.cpp && build/tests.o

TEST_CASE( "Positions are instantiated well", "[position]" ) {
    auto pos = Position(1,2);

    REQUIRE( pos.row == 1 );
    REQUIRE( pos.col == 2 );
}

TEST_CASE( "Moves are instantiated well", "[position]" ) {
    auto move = Move('W', Position(3,4));

    REQUIRE( move.pos.row == 3 );
    REQUIRE( move.pos.col == 4 );
    REQUIRE( move.player == 'W' );
}

TEST_CASE( "BoardState are instantiated well", "[board_state]" ) {
    auto board = BoardState();
    REQUIRE(board.board.size() == 13);
    REQUIRE(board.board[0].size() == 13);
    REQUIRE(board.board[0][0] == ' ');
    REQUIRE(board.board[7][3] == ' ');
    REQUIRE(board.board[12][12] == ' ');
}

TEST_CASE( "BoardState move is registered", "[board_state]" ) {
    auto board = BoardState();
    auto pos1 = Position(2, 2);
    auto pos2 = Position(2, 3);
    board.move('W', pos1);
    board.move('B', pos2);
    REQUIRE(board.board[2][2] == 'W');
    REQUIRE(board.board[2][3] == 'B');
    REQUIRE(board.board[3][3] == ' ');
    
}

TEST_CASE( "Other player funciton", "[other_player]" ) {
    REQUIRE(BoardState::other_player('W') == 'B');
    REQUIRE(BoardState::other_player('B') == 'W');
}

TEST_CASE( "valid position funciton", "[valid_pos]" ) {
    REQUIRE(BoardState::validPos(Position(1, 3)));
    REQUIRE(!BoardState::validPos(Position(-1, 3)));
    REQUIRE(!BoardState::validPos(Position(1,13)));
    REQUIRE(!BoardState::validPos(Position(5, 15)));
    REQUIRE(!BoardState::validPos(Position(133, -1)));
    REQUIRE(!BoardState::validPos(Position(4, -1)));
    REQUIRE(!BoardState::validPos(Position(-4, -5)));
}

TEST_CASE("Testing Position operator==", "[position_operator==]"){
    REQUIRE(Position(1,5) == Position(1,5));
    REQUIRE(Position(1,5) != Position(1,6));
    REQUIRE(Position(8,5) != Position(1,5));
}

TEST_CASE("Testing surrounding points", "[surrounding_points]"){
    auto surrounding = BoardState::get_surrounding_valid_positions(Position(5, 5));
    REQUIRE(find(surrounding.begin(), surrounding.end(), Position(4, 5)) != surrounding.end());
    REQUIRE(find(surrounding.begin(), surrounding.end(), Position(6, 5)) != surrounding.end());
    REQUIRE(find(surrounding.begin(), surrounding.end(), Position(5, 4)) != surrounding.end());
    REQUIRE(find(surrounding.begin(), surrounding.end(), Position(5, 6)) != surrounding.end());
    REQUIRE(find(surrounding.begin(), surrounding.end(), Position(5, 5)) == surrounding.end());
    REQUIRE(find(surrounding.begin(), surrounding.end(), Position(4, 7)) == surrounding.end());
}

TEST_CASE("Testing move", "[move]"){
    vector<vector<char>> board =  { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},            
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},           
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},          
                                    {' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' '},      
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},    
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},   
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}};

    vector<vector<char>> tar =      {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},            
                                    {' ', ' ', 'W', ' ', ' ', ' ', ' ', ' ', ' '},           
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},          
                                    {' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' '},      
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},    
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},   
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}};

    auto boardState = BoardState();
    boardState.board = board;
    boardState.move('W', Position(2, 2));
    REQUIRE(boardState.board == tar);
}

TEST_CASE("Testing liberties", "[liberties]"){
    BoardState::BOARD_SIZE = 9;
    vector<vector<char>> board =  { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},            
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},           
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},          
                                    {' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' '},      
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},    
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},   
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}};

    auto boardState = BoardState();
    boardState.board = board;
    
    unordered_set<Position> group;
    bool captured;
    tie(group, captured) = boardState.get_group_and_is_captured(Position(4, 4));

    REQUIRE(group.size() == 1);
    REQUIRE(group.find(Position(4, 4)) != group.end());
    REQUIRE(!captured);

}

TEST_CASE("Testing liberties2", "[liberties]"){
    vector<vector<char>> board =  { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},            
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},           
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},          
                                    {' ', ' ', ' ', 'B', 'B', 'B', ' ', ' ', ' '},      
                                    {' ', ' ', ' ', 'B', 'W', 'B', ' ', ' ', ' '},    
                                    {' ', ' ', ' ', ' ', ' ', 'B', ' ', ' ', ' '},   
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}};
    BoardState::BOARD_SIZE = 9;
    auto boardState = BoardState();
    boardState.board = board;
    
    unordered_set<Position> group;
    bool captured;
    tie(group, captured) = boardState.get_group_and_is_captured(Position(4, 4));
    cout << group.size() << endl;
    REQUIRE(group.size() == 6);
    REQUIRE(group.find(Position(6, 5)) != group.end());
    REQUIRE(!captured);
}

TEST_CASE("Testing liberties3", "[liberties]"){
    vector<vector<char>> board =  { {'B', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {'W', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' '},            
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},           
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},          
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},      
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},    
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},   
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}};
    BoardState::BOARD_SIZE = 9;
    auto boardState = BoardState();
    boardState.board = board;
    
    unordered_set<Position> group;
    bool captured;
    tie(group, captured) = boardState.get_group_and_is_captured(Position(0, 0));
    cout << group.size() << endl;
    REQUIRE(group.size() == 1);
    REQUIRE(group.find(Position(0, 0)) != group.end());
    REQUIRE(captured);

     tie(group, captured) = boardState.get_group_and_is_captured(Position(0, 1));
    cout << group.size() << endl;
    REQUIRE(group.size() == 3);
    REQUIRE(group.find(Position(1, 0)) != group.end());
    REQUIRE(!captured);
}

TEST_CASE("Test capture 1", "[capture]"){
    vector<vector<char>> board =  { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},            
                                    {' ', ' ', 'W', ' ', ' ', ' ', ' ', ' ', ' '},           
                                    {' ', 'W', 'B', ' ', ' ', ' ', ' ', ' ', ' '},          
                                    {' ', ' ', 'W', ' ', ' ', ' ', ' ', ' ', ' '},      
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},    
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},   
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}};
    
    vector<vector<char>> tar =  {   {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},            
                                    {' ', ' ', 'W', ' ', ' ', ' ', ' ', ' ', ' '},           
                                    {' ', 'W', ' ', 'W', ' ', ' ', ' ', ' ', ' '},          
                                    {' ', ' ', 'W', ' ', ' ', ' ', ' ', ' ', ' '},      
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},    
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},   
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}};

    BoardState::BOARD_SIZE = 9;
    auto boardState = BoardState();
    boardState.board = board;
    
    boardState.move('W', Position(3, 3));

    REQUIRE(boardState.board == tar);
}

TEST_CASE("Test capture 2", "[capture]"){
    vector<vector<char>> board =  { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', 'W', ' ', ' ', ' ', ' ', ' '},            
                                    {' ', ' ', 'W', 'B', ' ', ' ', ' ', ' ', ' '},           
                                    {' ', 'W', 'B', 'B', 'W', ' ', ' ', ' ', ' '},          
                                    {' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' '},      
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},    
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},   
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}};
    
    vector<vector<char>> tar =  {   {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', 'W', ' ', ' ', ' ', ' ', ' '},            
                                    {' ', ' ', 'W', ' ', 'W', ' ', ' ', ' ', ' '},           
                                    {' ', 'W', ' ', ' ', 'W', ' ', ' ', ' ', ' '},          
                                    {' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' '},      
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},    
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},   
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}};

    BoardState::BOARD_SIZE = 9;
    auto boardState = BoardState();
    boardState.board = board;
    
    boardState.move('W', Position(2, 4));

    REQUIRE(boardState.board == tar);
}

TEST_CASE("Test suicide rule", "[suicide_rule]"){
    BoardState::BOARD_SIZE = 9;

    vector<vector<char>> board =  {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', 'W', ' ', ' ', ' ', ' ', ' '},            
                                    {' ', ' ', 'W', 'W', 'W', ' ', ' ', ' ', ' '},           
                                    {' ', 'W', 'B', ' ', 'W', ' ', ' ', ' ', ' '},          
                                    {' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' '},      
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},    
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},   
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}};
    
    auto boardState = BoardState();
    boardState.board = board;
    
    bool threw_correct = false;
    try{
        boardState.move('B', Position(3, 3));
    }catch(IllegalMove& e){
        if(e.msg == "Suicide"){
            threw_correct = true;
        } 
    }
    REQUIRE(threw_correct);
}


TEST_CASE("Test KO rule", "[ko_rule]"){
    BoardState::BOARD_SIZE = 9;
    vector<vector<char>> board =  { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},            
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},           
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},          
                                    {' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' '},      
                                    {'B', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' '},    
                                    {'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},   
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}};
    vector<vector<char>> ko_board =
                                  { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},            
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},           
                                    {'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},          
                                    {'W', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' '},      
                                    {' ', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' '},    
                                    {'W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},   
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
                                    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}};
    auto boardState = BoardState();
    boardState.board = board;
    boardState.num_turns = 10;
    boardState.move('B', Position(3, 0));
    boardState.move('W', Position(4, 0));
    REQUIRE(boardState.board == ko_board);

    bool threw_correct = false;
    try{
        boardState.move('B', Position(5, 0));
    }catch(IllegalMove& e){
        if(e.msg == "KO"){
            threw_correct = true;
        }
    }
    REQUIRE(threw_correct);
}


