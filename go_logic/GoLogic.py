from typing import Tuple, List
from collections import namedtuple
from queue import Queue

Position = namedtuple('Position', ['row', 'col'])

class BoardState:
    BOARD_SIZE = 9
    
    @staticmethod
    def valid_pos(pos : Position) -> bool:
        return pos.row >= 0 and pos.row < BoardState.BOARD_SIZE and pos.col >= 0 and pos.col < BoardState.BOARD_SIZE 

    @staticmethod
    def get_surrounding_valid_positios(pos : Position) -> List[Position]:
        possible_positions = [Position(pos.row + delta_r, pos.col + delta_c) for delta_r, delta_c in [(1, 0), (-1, 0), (0, 1), (0, -1)]]
        return [pos for pos in possible_positions if BoardState.valid_pos(pos)]

    @staticmethod
    def other_player(player : int):
        assert player == 1 or player == 2
        return (3 - player)

    @staticmethod
    def same_boards(board1, board2):

        return len(board1) == len(board2) and len(board1[0]) == len(board2[0]) and \
                all([x1 == x2 for r1, r2 in zip(board1, board2) for x1, x2 in zip(r1, r2)])


    def __init__(self, board = None):
        if board:
            self.board = [[c for c in r] for r in board]
        else:
            self.board = [[0 for c in range(BoardState.BOARD_SIZE)] for r in range(BoardState.BOARD_SIZE)]
        self.player_to_captures = {1 : 0, 2 : 0}

    def move(self, player : int, pos : Position):
        
        assert BoardState.valid_pos(pos)
        assert self.board[pos.row][pos.col] == 0
        # TODO: check if move is valid (suicide rule, ko rule)

        self.board[pos.row][pos.col] = player
        captured_pieces = self.get_captured_pieces(player, pos)
        self.player_to_captures[player] += len(captured_pieces)
        for x in captured_pieces:
            self.board[x.row][x.col] = 0

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
        assert player != 0
        
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
            if len([x for x in valid_surrounding if self.board[x.row][x.col] == 0]) > 0:
                captured = False

            player_neighbors_unvisited = [x for x in valid_surrounding if self.board[x.row][x.col] == player and x not in visited]
            for x in player_neighbors_unvisited:
                q.put(x)
                visited.add(x)
        
            
        return visited, captured


    def __str__(self):
        def to_char(x):
            return " " if x == 0 else ("X" if x == 1 else "O")
        return "\n".join([" ".join([to_char(x) for x in r]) for r in self.board])

