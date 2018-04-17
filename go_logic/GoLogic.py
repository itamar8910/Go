from typing import Tuple, List
from collections import namedtuple
from queue import Queue
from collections import deque
from go_logic.Exceptions import KoException, SuicideException, SpotOccupiedException, InvalidMoveException
from go_logic.primitives import Position, Move

class BoardState:
    BOARD_SIZE = 13
    
    @staticmethod
    def valid_pos(pos : Position) -> bool:
        return pos.row >= 0 and pos.row < BoardState.BOARD_SIZE and pos.col >= 0 and pos.col < BoardState.BOARD_SIZE 

    @staticmethod
    def get_surrounding_valid_positios(pos : Position) -> List[Position]:
        possible_positions = [Position(pos.row + delta_r, pos.col + delta_c) for delta_r, delta_c in [(1, 0), (-1, 0), (0, 1), (0, -1)]]
        return [pos for pos in possible_positions if BoardState.valid_pos(pos)]

    @staticmethod
    def other_player(player : int):
        assert player == 'B' or player == 'W'
        return 'B' if player == 'W' else 'W'

    @staticmethod
    def same_boards(board1, board2):

        return len(board1) == len(board2) and len(board1[0]) == len(board2[0]) and \
                all([x1 == x2 for r1, r2 in zip(board1, board2) for x1, x2 in zip(r1, r2)])

    @staticmethod
    def copy_board(board):
        return [[x for x in r] for r in board]

    @staticmethod
    def init_from_moves(moves : List[Move]):
        state = BoardState()
        for move in moves:
            state.move(move.player, move.pos)
        return state

    def __init__(self, board = None):
        if board:
            assert len(board) == BoardState.BOARD_SIZE
            self.board = [[c for c in r] for r in board]
        else:
            self.board = [[' ' for c in range(BoardState.BOARD_SIZE)] for r in range(BoardState.BOARD_SIZE)]
        self.player_to_captures = {'B' : 0, 'W' : 0}
        self.past_2_boards = deque(maxlen=2) # to enforce the KO rule
        self.num_turns = 0

    def move(self, player : int, pos : Position):
        
        if not BoardState.valid_pos(pos):
            raise InvalidMoveException
        if self.board[pos.row][pos.col] != ' ':
            raise SpotOccupiedException

        current_state_save = self.clone()

        self.num_turns += 1
        self.board[pos.row][pos.col] = player
        captured_pieces = self.get_captured_pieces(player, pos)
        self.player_to_captures[player] += len(captured_pieces)
        for x in captured_pieces:
            self.board[x.row][x.col] = ' '

        # check for suicide
        if self.get_group_and_is_captured(pos)[1]:
            # rollback board
            BoardState.shallow_copy(current_state_save, self)
            raise SuicideException
        
        # check for KO
        if len(self.past_2_boards) == 2 and BoardState.same_boards(self.board, self.past_2_boards[0]):
            # rollback board
            BoardState.shallow_copy(current_state_save, self)
            raise KoException

        self.past_2_boards.append(BoardState.copy_board(self.board))

        

    def get_captured_pieces(self, player : int, pos : Position):
        all_captured = set()
        for pos in BoardState.get_surrounding_valid_positios(pos):
            if self.board[pos.row][pos.col] == BoardState.other_player(player):
                enemy_group, captured = self.get_group_and_is_captured(pos)
                if captured:
                    all_captured.update(enemy_group)
        return all_captured

    def get_group_and_is_captured(self, pos : Position) -> Tuple[List[Position], bool]:
        player = self.board[pos.row][pos.col]
        assert player != ' '
        
        visited = set()
        captured = True
        # bfs
        q = Queue()
        q.put(pos)
        visited.add(pos)
        while not q.empty():
            current_pos = q.get()
            visited.add(current_pos)
            valid_surrounding = BoardState.get_surrounding_valid_positios(current_pos)
            if len([x for x in valid_surrounding if self.board[x.row][x.col] == ' ']) > 0:
                captured = False

            player_neighbors_unvisited = [x for x in valid_surrounding if self.board[x.row][x.col] == player and x not in visited]
            for x in player_neighbors_unvisited:
                q.put(x)
                visited.add(x)
        
            
        return visited, captured

    def clone(self):
        new_state = BoardState()
        new_state.board = BoardState.copy_board(self.board)
        new_state.num_turns = self.num_turns
        new_state.past_2_boards = deque(list(self.past_2_boards), maxlen = 2) # shallow copy of boards should suffice
        new_state.player_to_captures = {p:c for p, c in self.player_to_captures.items()}
        return new_state

    @staticmethod
    def shallow_copy(src, dst):
        dst.board = src.board
        dst.num_turns = src.num_turns
        dst.past_2_boards = src.past_2_boards
        dst.player_to_captures = src.player_to_captures

    def __str__(self):
        def to_char(x):
            return " " if x == ' ' else ("B" if x == 1 else "W")
        return "\n".join([" ".join([to_char(x) for x in r]) for r in self.board])

