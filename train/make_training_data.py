from typing import List

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

def get_3_plane_X_representation(board_state, turn):
    """
    plane 1 - 1 if player's stone
    plane 2 - 1 if opponent's stone
    plane 3 - 1 if empty spot
    """
    plane_player = np.array([[1 if board_state.board[r][c] == turn else 0 for c in range(BoardState.BOARD_SIZE)] for r in range(BoardState.BOARD_SIZE)])
    plane_opponent = np.array([[1 if board_state.board[r][c] == BoardState.other_player(turn) else 0 for c in range(BoardState.BOARD_SIZE)] for r in range(BoardState.BOARD_SIZE)])
    plane_empty = np.array([[1 if board_state.board[r][c] == ' ' else 0 for c in range(BoardState.BOARD_SIZE)] for r in range(BoardState.BOARD_SIZE)])
    # return concat_planes(plane_player, plane_opponent, plane_empty)
    return {'player': plane_player, 'opponent': plane_opponent, 'empty': plane_empty}

#deprecated because doesn't support move history
def board_state_to_X_Y(board_state : BoardState, player_move : Move, X_representation = get_3_plane_X_representation):
    X = X_representation(board_state, player_move.player)
    tar_move_plane = np.array([[1 if r == player_move.pos.row and c == player_move.pos.col else 0 for c in range(BoardState.BOARD_SIZE)] for r in range(BoardState.BOARD_SIZE)])
    return X, tar_move_plane

def get_board_Y_repr(board_state : BoardState, player_move : Move):
    return np.array([[1 if r == player_move.pos.row and c == player_move.pos.col else 0 for c in range(BoardState.BOARD_SIZE)] for r in range(BoardState.BOARD_SIZE)])


def game_to_XYs_3_planes(sgf_path):
    parser = SGFParser(sgf_path)
    Xs = []
    Ys = []
    moves = [m for m in parser.moves if m is not None]
    for state, move in zip(BoardState.get_all_intermittent_states_from_moves(moves), moves):
        # X, Y = board_state_to_X_Y(state, move)
        Y = get_board_Y_repr(state, move)
        three_planes = get_3_plane_X_representation(state, move.player)
        X = concat_planes(three_planes['player'], three_planes['opponent'], three_planes['empty'])
        Xs.append(X)
        Ys.append(Y)
        
    return Xs, Ys


def get_history_planes(spots_last_played : List[List[int or None]], current_turn : int, HISTORY_LEN = 5):
    """
    returns planes indicating how many turns since a move was played in a position

    :param spots_last_played: for each spot in the board, the turn it was last played at or None if it wasn't played yet
    :param current_turn: the current turn index
    :param HISTORY_LEN: number of moves to remember in history (= number of returned planes)
    :return: 3d ndarray, num of planes = HISTORY_LEN. results[i][r][c] = 1 if move r,c was played i turns ago, else 0
    """
    history_planes = [[[0 for c in range(BoardState.BOARD_SIZE)] for r in range(BoardState.BOARD_SIZE)]
                      for history_i in range(HISTORY_LEN)]
    for r in range(BoardState.BOARD_SIZE):
        for c in range(BoardState.BOARD_SIZE):
            if spots_last_played[r][c] is None:
                continue # spot was'nt played yet
            turns_since_played = current_turn - 1 - spots_last_played[r][c]
            if  turns_since_played < HISTORY_LEN:
                history_planes[turns_since_played][r][c] = 1
    return np.array(history_planes)

def game_to_XYs_with_history(sgf_path):
    """
    returns 8 planes: player, opponent, empty, 5 history planes
    :param sgf_path:
    :return:
    """
    parser = SGFParser(sgf_path)
    Xs = []
    Ys = []
    moves = [m for m in parser.moves if m is not None]
    "spots_last_played[r][c] = turn num in which this spot was last played or None if it wasn't played yet"
    spots_last_played = [[None for c in range(BoardState.BOARD_SIZE)] for r in range(BoardState.BOARD_SIZE)]
    for move_i, (state, move) in enumerate(zip(BoardState.get_all_intermittent_states_from_moves(moves), moves)):
        # X, Y = board_state_to_X_Y(state, move)
        Y = get_board_Y_repr(state, move)
        three_planes = get_3_plane_X_representation(state, move.player)
        history_planes = get_history_planes(spots_last_played, current_turn=move_i)
        X = concat_planes(three_planes['player'], three_planes['opponent'], three_planes['empty'], *history_planes)
        Xs.append(X)
        Ys.append(Y)
        spots_last_played[move.pos.row][move.pos.col] = move_i
        move_i += 1
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
        f_Xs, f_Ys = game_to_XYs_3_planes(path.join(sgf_dir, f))
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

def get_games_XY(sgf_dir, shuffle_games = True, shuffle_sampels = True, verbose=False, game_to_XY_func=game_to_XYs_3_planes):
    Xs = []
    Ys = []
    games = list(listdir(sgf_dir))
    if shuffle_games:
        shuffle(games)
    for f_i, f in enumerate(games):
        if verbose:
            print("progress: {:.2f}, game: {}/{}".format((f_i/len(games)), f_i+1, len(games)))
        f_Xs, f_Ys = game_to_XY_func(path.join(sgf_dir, f))
        if len(f_Xs) == 0 or len(f_Ys) == 0:
            print("Game with 0 samples:{}".format(f))
            continue
        game_XYs = list(zip(f_Xs, f_Ys))
        shuffle(game_XYs)
        f_Xs, f_Ys = list(zip(*game_XYs))
        Xs.extend(f_Xs)
        Ys.extend(f_Ys)
    return Xs, Ys


def generate_games_XY_worker_main(games_dir, games, queue, done_queue, n_workers, game_to_XY_func=game_to_XYs_3_planes):
    print("process: {} working".format(os.getpid()))
    print('worker in generating games with func:', game_to_XY_func)
    for game_i, game in enumerate(games):
        game_Xs, game_Ys = game_to_XY_func(path.join(games_dir, game))
        if len(game_Xs) == 0 or len(game_Ys) == 0:
            print("Game with 0 samples:{}".format(game))
            continue
        queue.put((game_Xs, game_Ys))
    print("process: {} done".format(os.getpid()))
    done_queue.put(os.getpid())
    if done_queue.qsize() == n_workers:
        print('all workers are done, putting None in queue')
        queue.put(None)

def generate_games_XY_multiprocessing(sgf_dir, batch_size, shuffle_games = True,
                                      shuffle_sampels = True, verbose=False,
                                      N_WORKES = 2, game_to_XY_func=game_to_XYs_3_planes):

    m = multiprocessing.Manager()
    data_queue = m.Queue() # workers will put Xs, Ys in this queue
    worker_done_queue = m.Queue() # workers will put their PID in this queue once they're done
    
    all_games = list(listdir(sgf_dir))
    if shuffle_games:
        shuffle(all_games)
    games_per_worker = len(all_games) // N_WORKES
    worker_to_games = [all_games[worker_i * games_per_worker : (worker_i + 1) * games_per_worker] for worker_i in range(N_WORKES)]
    pool = multiprocessing.Pool(processes = N_WORKES)
    pool.starmap_async(generate_games_XY_worker_main, [(sgf_dir, worker_to_games[worker_i], data_queue, worker_done_queue, N_WORKES, game_to_XY_func) for worker_i in range(N_WORKES)])
    i = 0
    batch_Xs, batch_Ys = [], []
    for game_Xs, game_Ys in iter(data_queue.get, None):
        if verbose:
            print('games: {} {}'.format(len(game_Xs), len(game_Ys)))
            print(i / len(all_games))
        
        if shuffle_sampels:
            game_XYs = list(zip(game_Xs, game_Ys))
            shuffle(game_XYs)
            game_Xs, game_Ys = list(zip(*game_XYs))

        batch_Xs.extend(game_Xs)
        batch_Ys.extend(game_Ys)
        if len(batch_Xs) >= batch_size:
            leftover_Xs = batch_Xs[batch_size:]
            leftover_Ys = batch_Ys[batch_size:]
            yield batch_Xs[:batch_size], batch_Ys[:batch_size]
            batch_Xs = leftover_Xs
            batch_Ys = leftover_Ys
        i += 1

    if len(batch_Xs) > 0:
        yield batch_Xs, batch_Ys
    


def main():

    # generate_games_XY_multiprocessing('data/13/collection_1/val', 64)
    import time
    t1 = time.time()
    Xs, Ys = game_to_XYs_with_history('data/13/sources/go13/2015-03-06T16:25:13.507Z_k5m7o9gtv63k.sgf')
    print(time.time() - t1)
    print(np.array(Xs).shape, np.array(Ys).shape)
    # X, Y = generate_games_XY('data/13/go13/collection_1/val', -1, verbose=True) 
    # print(len(X))
    # print(len(Y))

if __name__ == "__main__":
    main()