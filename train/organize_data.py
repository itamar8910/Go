from os import path, listdir, mkdir
from random import shuffle
from shutil import copy

def main():
    def write_to_dir(src_dir, files, tar_dir):
            for f in files:
                copy(path.join(src_dir, f), path.join(tar_dir, f))
    SOURCES = ['data/13/sources/go13', 'data/13/sources/go13-large']
    TAR_DIR = 'data/13/collection_4'
    dirs_to_make = [TAR_DIR, path.join(TAR_DIR, 'train'), path.join(TAR_DIR, 'test'), path.join(TAR_DIR, 'val')]
    for dir_to_make in dirs_to_make:
        if not path.isdir(dir_to_make):
            mkdir(dir_to_make)
    TEST_SIZE = 0.1
    VAL_SIZE = 0.02
    for source in SOURCES:
        games = list(listdir(source))
        shuffle(games)
        test_split = int(len(games) * TEST_SIZE)
        val_split = int(test_split + len(games) * VAL_SIZE)
        test_games = games[:test_split]
        val_games = games[test_split:val_split]
        train_games = games[val_split:]
        print(test_split, val_split)
        print("# test:", len(test_games))
        print("# val:", len(val_games))
        print("# train:", len(train_games))
        print('writing test')
        write_to_dir(source, test_games, path.join(TAR_DIR, 'test'))
        print('writing val')
        write_to_dir(source, val_games, path.join(TAR_DIR, 'val'))
        print('writing train')
        write_to_dir(source, train_games, path.join(TAR_DIR, 'train'))


if __name__ == "__main__":
    main()