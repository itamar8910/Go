from go_logic.GoLogic import BoardState
from go_logic.Exceptions import KoException, SuicideException
from go_logic.primitives import Position
from go_logic.sgf_parser import SGFParser

import unittest

class TestBoardLogic(unittest.TestCase):

    def test_liberties1(self):
        BoardState.BOARD_SIZE = 9
        board = [
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', 'B', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
        ]
        
        state = BoardState(board)
        group, captured = state.get_group_and_is_captured(Position(4, 4))
    
        assert len(group) == 1 and list(group)[0] == Position(4, 4) and not captured

    def test_liberties2(self):
        BoardState.BOARD_SIZE = 9

        board =  [
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', 'B', 'B', 'B', ' ', ' '],
            [' ', ' ', ' ', ' ', 'B', ' ', 'B', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', 'B', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
        ]
        
        state = BoardState(board)
        group, captured = state.get_group_and_is_captured(Position(4, 4))
    
        assert len(group) == 6 and not captured

    def test_liberties3(self):
        BoardState.BOARD_SIZE = 9

        board =  [
            ['B', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            ['W', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
        ]
        
        state = BoardState(board)
        group, captured = state.get_group_and_is_captured(Position(0, 0))
    
        assert len(group) == 1 and captured
    
    def sameboards_test(self):
        BoardState.BOARD_SIZE = 9

        board1 = [
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', 'W', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', 'W', 'B', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', 'W', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
        ]

        board2 = [
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', 'W', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', 'W', 'B', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', 'W', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
        ]

        board3 =   [
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', 'W', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', 'W', 'B', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', 'W', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
        ]

        assert BoardState.same_boards(board1, board2) and not BoardState.same_boards(board1, board3)

    def test_capture1(self):
        BoardState.BOARD_SIZE = 9

        board = [
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', 'W', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', 'W', 'B', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', 'W', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
        ]

        tar_board = [
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', 'W', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', 'W', ' ', 'W', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', 'W', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
        ]
        state = BoardState(board)
        state.move('W', Position(3, 3)) 
        # print(state)  
        assert BoardState.same_boards(state.board, tar_board) 
        
    def test_capture2(self):
        BoardState.BOARD_SIZE = 9

        board = [
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', 'W', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', 'W', 'B', ' ', ' ', ' ', ' ', ' '],
            [' ', 'W', 'B', 'B', 'W', ' ', ' ', ' ', ' '],
            [' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
        ]
        tar_board = [
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', 'W', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', 'W', ' ', 'W', ' ', ' ', ' ', ' '],
            [' ', 'W', ' ', ' ', 'W', ' ', ' ', ' ', ' '],
            [' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
        ]
        state = BoardState(board)
        state.move('W', Position(2, 4))   
        assert BoardState.same_boards(state.board, tar_board) 
        
    def test_suicide_rule1(self):
        BoardState.BOARD_SIZE = 9

        board = [
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', 'W', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' '],
            [' ', 'W', 'B', ' ', 'W', ' ', ' ', ' ', ' '],
            [' ', ' ', 'W', 'W', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
        ]
       
        state = BoardState(board)
        threw_correct = False

        
        try:
            state.move('B', Position(3, 3))   
        except Exception as e:
            if isinstance(e, SuicideException):
                threw_correct = True

        assert threw_correct
        
    def test_ko_rule1(self):
        BoardState.BOARD_SIZE = 9

        board = [
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            ['B', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            ['W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
        ]
        ko_board = [
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            ['B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            ['W', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', 'W', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            ['W', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
            [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
        ]
       
        state = BoardState(board)
        state.num_turns = 10 
        state.move('B', Position(3, 0))
        state.move('W', Position(4, 0))
        assert BoardState.same_boards(state.board, ko_board)
        threw_correct = False

        
        try:
            state.move('B', Position(5, 0))
        except Exception as e:
            if isinstance(e, KoException):
                threw_correct = True

        assert threw_correct
    
    def test_game1(self):
        BoardState.BOARD_SIZE = 13
        init_state = BoardState()
        parser = SGFParser('tests_data/game1.sgf')
        moves = parser.moves
        good_end_board = [[' ', 'W', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '],
                        [' ', 'W', 'W', 'B', ' ', ' ', 'B', 'B', ' ', ' ', ' ', 'W', ' '],
                        [' ', ' ', 'W', 'B', 'B', 'B', 'W', 'B', ' ', ' ', 'B', 'B', 'B'],
                        [' ', ' ', ' ', 'W', 'W', 'B', 'W', 'W', 'B', 'B', 'B', 'W', ' '],
                            [' ', ' ', ' ', 'W', ' ', 'B', 'W', 'W', 'W', 'W', 'B', 'W', ' '],
                            [' ', ' ', ' ', ' ', ' ', 'B', 'B', 'W', 'W', 'B', 'W', 'W', ' '],
                            [' ', ' ', ' ', 'W', 'B', 'B', 'W', 'W', ' ', ' ', 'B', 'W', ' '],
                            [' ', 'B', ' ', 'W', ' ', ' ', 'B', 'W', 'W', 'W', ' ', ' ', ' '],
                                ['W', 'W', ' ', ' ', 'B', ' ', ' ', 'W', 'W', 'B', 'W', 'W', ' '],
                                ['B', 'W', 'W', 'W', 'W', 'W', 'B', 'W', 'B', 'B', 'B', 'W', ' '],
                                ['B', 'B', 'B', 'B', 'W', 'B', 'B', 'B', ' ', 'B', ' ', 'B', ' '],
                                [' ', ' ', 'B', ' ', 'B', 'W', ' ', ' ', ' ', ' ', 'B', ' ', ' '],
                                    [' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ']]
        for move in moves:
            init_state.move(move.player, move.pos)
        
        assert BoardState.same_boards(init_state.board, good_end_board)

if __name__ == "__main__":
    unittest.main()