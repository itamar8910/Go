import numpy as np
import keras
from os import listdir, path
from random import shuffle
from train.make_training_data import game_to_XYs, num_samples_in_game
from go_logic.GoLogic import BoardState
from keras.models import Sequential
from keras.layers import Activation, Flatten
from keras.layers import Conv2D
import json

class GamesDataGenerator(keras.utils.Sequence):
    'Generates data for Keras'
    def __init__(self, games_dir, board_size, _nX_planes, batch_size=32, shuffle=True):
        'Initialization'
        self.batch_size = batch_size
        self.shuffle = shuffle
        self.games_dir = games_dir
        self.games_sgfs = list(listdir(games_dir))
        self.board_size = board_size
        self.n_X_planes = _nX_planes
        self.total_num_samples = self.__calc_total_num_samples()
        self.current_game_index = 0
        self.current_sample_index = 0
        self.leftover_X = []
        self.leftover_Y = []
        self.on_epoch_end()

    def __len__(self):
        'Denotes the number of batches per epoch'
        return int(np.floor(self.total_num_samples / self.batch_size))

    def __getitem__(self, index):
        'Generate one batch of data'
        # print('GamesDataGenerator __getitem__, n_games:{}, n_samples:{}'.format(self.current_game_index, self.current_sample_index))
        while len(self.leftover_X) < self.batch_size and self.current_game_index < len(self.games_sgfs):
            game_Xs, game_Ys = game_to_XYs(path.join(self.games_dir, self.games_sgfs[self.current_game_index]))
            self.current_game_index += 1
            self.current_sample_index += len(game_Xs)
            self.leftover_X.extend(game_Xs)
            self.leftover_Y.extend(game_Ys)

        to_return =  (np.array([x for x in self.leftover_X[:self.batch_size]]), np.array([y for y in self.leftover_Y[:self.batch_size]]).reshape([-1, self.board_size*self.board_size]))
        self.leftover_X = self.leftover_X[self.batch_size: ]
        self.leftover_Yleftover_Y = self.leftover_Y[self.batch_size: ]
        # print(len(to_return[0]), len(to_return[1]))
        return to_return


    def on_epoch_end(self):
        'Updates indexes after each epoch'
        self.current_game_index = 0
        self.current_sample_index = 0
        if self.shuffle == True:
            shuffle(self.games_sgfs)
    
    def __calc_total_num_samples(self):
        parent_dir, games_dirname = path.split(self.games_dir)
        meta_file = path.join(parent_dir, 'META.json')
        if path.isfile(path.join(meta_file)):
            print('GamesDataGenerator loading num_samples from data')
            with open(meta_file, 'r') as f:
                metadata = json.load(f)
                return metadata['n_samples'][games_dirname]
        print('GamesDataGenerator calculation n samples')
        n_samples = 0
        for f_i, f in enumerate(self.games_sgfs):
            print(f_i, n_samples)
            n_samples += num_samples_in_game(path.join(self.games_dir, f))
        print('GamesDataGenerator total n samples:{}'.format(n_samples))
        return n_samples

SGF_TRAIN_DIR = 'data/13/collection_1/train'
SGF_VAL_DIR = 'data/13/collection_1/val'
SGF_TEST_DIR = 'data/13/collection_1/test'
N_X_PLANES = 3
N_FILTERS = 64
KERNEL_SIZE = (3, 3)
BOARD_SIZE = BoardState.BOARD_SIZE
INPUT_SHAPE = [BOARD_SIZE, BOARD_SIZE, N_X_PLANES]

def main():
    # Generators
    training_generator = GamesDataGenerator(SGF_TRAIN_DIR, BOARD_SIZE, N_X_PLANES, batch_size=32)
    validation_generator = GamesDataGenerator(SGF_VAL_DIR, BOARD_SIZE, N_X_PLANES, batch_size=32)

    # Design model
    model = Sequential()
    model.add(Conv2D(N_FILTERS, KERNEL_SIZE, padding='same',
                 input_shape=INPUT_SHAPE, activation='relu')) # conv1
    model.add(Conv2D(N_FILTERS, KERNEL_SIZE, padding='same', activation='relu'))# conv2
    model.add(Conv2D(N_FILTERS, KERNEL_SIZE, padding='same', activation='relu'))# conv3
    model.add(Conv2D(N_FILTERS, KERNEL_SIZE, padding='same', activation='relu'))# conv4
    # model.add(Conv2D(N_FILTERS, KERNEL_SIZE, padding='same', activation='relu'))# conv5
    model.add(Conv2D(1, [1, 1], padding='same')) #conv5, 1x1 convolution to get final predictions plane, no activation
    model.add(Activation('softmax'))
    model.add(Flatten())
    model.compile(loss='categorical_crossentropy',
                optimizer=keras.optimizers.adam(),
                metrics=['accuracy'])

    # Train model on dataset
    model.fit_generator(generator=training_generator,
                         validation_data=validation_generator,
                         use_multiprocessing=True,
                         workers=1,
                         steps_per_epoch = training_generator.__len__(),
                         validation_steps = validation_generator.__len__())
    #model.fit_generator(generator=training_generator,
    #                    validation_data=validation_generator,
    #                    )
if __name__ == "__main__":
   main()
