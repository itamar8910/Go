import numpy as np
from go_logic.GoLogic import BoardState
from go_logic.primitives import Move
from go_logic.sgf_parser import SGFParser
from os import path, listdir
import pickle
from random import shuffle

def concat_planes(*planes):
    board_size = len(planes[0])
    return np.array([[[p[r][c] for p in planes] for c in range(board_size)] for r in range(board_size)])
    # return np.concatenate([x[None, :, :] for x in planes], axis = 0)

def simple_X_representation(board_state, turn):
    """
    plane 1 - 1 if player's stone
    plane 2 - 1 if opponent's stone
    plane 3 - 1 if empty spot
    """
    plane_player = np.array([[1 if board_state.board[r][c] == turn else 0 for c in range(BoardState.BOARD_SIZE)] for r in range(BoardState.BOARD_SIZE)])
    plane_opponent = np.array([[1 if board_state.board[r][c] == BoardState.other_player(turn) else 0 for c in range(BoardState.BOARD_SIZE)] for r in range(BoardState.BOARD_SIZE)])
    plane_empty = np.array([[1 if board_state.board[r][c] == ' ' else 0 for c in range(BoardState.BOARD_SIZE)] for r in range(BoardState.BOARD_SIZE)])
    return concat_planes(plane_player, plane_opponent, plane_empty)

def board_state_to_X_Y(board_state : BoardState, player_move : Move, X_representation = simple_X_representation):
    X = X_representation(board_state, player_move.player)
    tar_move_plane = np.array([[1 if r == player_move.pos.row and c == player_move.pos.col else 0 for c in range(BoardState.BOARD_SIZE)] for r in range(BoardState.BOARD_SIZE)])
    return X, tar_move_plane


def game_to_XYs(sgf_path):
    parser = SGFParser(sgf_path)
    Xs = []
    Ys = []
    moves = [m for m in parser.moves if m is not None]
    for state, move in zip(BoardState.get_all_intermittent_states_from_moves(moves), moves):
        X, Y = board_state_to_X_Y(state, move)

        Xs.append(X)
        Ys.append(Y)
        
    return Xs, Ys

def num_samples_in_game(sgf_path):
    parser = SGFParser(sgf_path)
    return len(parser.moves)

def generate_games_XY(sgf_dir, batch_size, shuffle_games = True, shuffle_sampels = True):
    """
    yields batches of X, Y data, each batch of 'batch_size' size.
    if batch_Size = -1, batch size = data size
    """
    batch_Xs = []
    batch_Ys = []
    games = list(listdir(sgf_dir))
    if shuffle_games:
        shuffle(games)
    # print("# of games in data: ", len(games))
    for f_i, f in enumerate(games):
        f_Xs, f_Ys = game_to_XYs(path.join(sgf_dir, f))
        game_XYs = list(zip(f_Xs, f_Ys))
        shuffle(game_XYs)
        f_Xs, f_Ys = list(zip(*game_XYs))
        batch_Xs.extend(f_Xs)
        batch_Ys.extend(f_Ys)
        if batch_size != -1 and len(batch_Xs) >= batch_size:
            leftover_X = batch_Xs[batch_size:]
            leftover_Y = batch_Ys[batch_size:]
            yield batch_Xs[:batch_size], batch_Ys[:batch_size]
            batch_Xs = leftover_X
            batch_Ys = leftover_Y
    yield batch_Xs, batch_Ys


def main():
    # Xs, Ys = game_to_XYs('data/13/go13/2015-03-06T16:25:13.507Z_k5m7o9gtv63k.sgf')
    XY_generator = generate_games_XY('data/13/go13', 5000) 
    batch_X, batch_Y = XY_generator.__next__()
    print(len(batch_X), len(batch_Y))