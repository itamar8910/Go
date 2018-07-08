
import sys
project_root = '.'
sys.path.append(project_root)
from go_logic.GoLogic import BoardState
from go_logic.sgf_parser import SGFParser
from os import path, listdir
import time


if __name__ == "__main__":
    BoardState.BOARD_SIZE = 13
    games_dir = path.join(project_root, 'tests_data/1000_games/')
    t1 = time.time()
    games_sgfs = listdir(games_dir)
    print('parsing')
    games_moves = [SGFParser(path.join(games_dir, game_sgf)).moves for game_sgf in games_sgfs]
    print('done parsing')
    for game_i, game_moves in enumerate(games_moves):
        print(game_i)
        init_state = BoardState()
        for move in game_moves:
            if move:
                init_state.move(move.player, move.pos)
               
        print(init_state)
    print(time.time() - t1)