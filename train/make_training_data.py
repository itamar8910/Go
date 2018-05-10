import numpy as np
from go_logic.GoLogic import BoardState
from go_logic.primitives import Move
from go_logic.sgf_parser import SGFParser
from os import path, listdir
import pickle
from random import shuffle
import multiprocessing
import os

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

def generate_games_XY(sgf_dir, batch_size, shuffle_games = True, shuffle_sampels = True, verbose=False):
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
        if verbose:
            print("progress: {:.2f}, game: {}/{}".format((f_i/len(games)), f_i+1, len(games)))
        f_Xs, f_Ys = game_to_XYs(path.join(sgf_dir, f))
        if len(f_Xs) == 0 or len(f_Ys) == 0:
            print("Game with 0 samples:{}".format(f))
            continue
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

def get_games_XY(sgf_dir, shuffle_games = True, shuffle_sampels = True, verbose=False):
    Xs = []
    Ys = []
    games = list(listdir(sgf_dir))
    if shuffle_games:
        shuffle(games)
    for f_i, f in enumerate(games):
        if verbose:
            print("progress: {:.2f}, game: {}/{}".format((f_i/len(games)), f_i+1, len(games)))
        f_Xs, f_Ys = game_to_XYs(path.join(sgf_dir, f))
        if len(f_Xs) == 0 or len(f_Ys) == 0:
            print("Game with 0 samples:{}".format(f))
            continue
        game_XYs = list(zip(f_Xs, f_Ys))
        shuffle(game_XYs)
        f_Xs, f_Ys = list(zip(*game_XYs))
        Xs.extend(f_Xs)
        Ys.extend(f_Ys)
    return Xs, Ys

def worker_main(games_dir, games, queue, done_queue, n_workers):
    print("process: {} working".format(os.getpid()))
    for game_i, game in enumerate(games):
        game_Xs, game_Ys = game_to_XYs(path.join(games_dir, game))
        if len(game_Xs) == 0 or len(game_Ys) == 0:
            print("Game with 0 samples:{}".format(game))
            continue
        queue.put((game_Xs, game_Ys))
    print("process: {} done".format(os.getpid()))
    done_queue.put(os.getpid())
    if done_queue.qsize() == n_workers:
        print('all workers are done, putting None in queue')
        queue.put(None)

def generate_games_XY_multiprocessing(sgf_dir, batch_size, shuffle_games = True, shuffle_sampels = True, verbose=False):
    N_WORKES = 3
    m = multiprocessing.Manager()
    data_queue = m.Queue() # workers will put Xs, Ys in this queue
    worker_done_queue = m.Queue() # workers will put their PID in this queue once they're done
    
    all_games = list(listdir(sgf_dir))
    if shuffle_games:
        shuffle(all_games)
    games_per_worker = len(all_games) // N_WORKES
    worker_to_games = [all_games[worker_i * games_per_worker : (worker_i + 1) * games_per_worker] for worker_i in range(N_WORKES)]
    pool = multiprocessing.Pool(processes = N_WORKES)
    pool.starmap_async(worker_main, [(sgf_dir, worker_to_games[worker_i], data_queue, worker_done_queue, N_WORKES) for worker_i in range(N_WORKES)])
    i = 0
    for game_Xs, game_Ys in iter(data_queue.get, None):
        print('games: {} {}'.format(len(game_Xs), len(game_Ys)))
        print(i / len(all_games))
        i += 1
    
    print('DONE')


def main():
    generate_games_XY_multiprocessing('data/13/collection_1/val', 64)
    # Xs, Ys = game_to_XYs('data/13/go13/2015-03-06T16:25:13.507Z_k5m7o9gtv63k.sgf')
    # X, Y = generate_games_XY('data/13/go13/collection_1/val', -1, verbose=True) 
    # print(len(X))
    # print(len(Y))