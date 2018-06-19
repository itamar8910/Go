#include <iostream>

#ifndef GoLogic_H
#define GoLogic_H 
#include "GoLogic.hpp"
#endif

using namespace std;

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"


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

TEST_CASE( "BoardState move is registered", "[board_state2]" ) {
    auto board = BoardState();
    auto pos1 = Position(2, 2);
    auto pos2 = Position(2, 3);
    board.move('W', pos1);
    board.move('B', pos2);
    REQUIRE(board.board[2][2] == 'W');
    REQUIRE(board.board[2][3] == 'B');
    REQUIRE(board.board[3][3] == ' ');
    
}