import numpy as np
import tensorflow as tf
from go_logic.GoLogic import BoardState
from train.make_training_data import get_games_XY, generate_games_XY_multiprocessing, game_to_XYs_3_planes, game_to_XYs_with_history
from random import shuffle
import time
import pickle
from os import path

# Parameters
learning_rate = 0.001
training_epochs = 100
batch_size = 64
SGF_TRAIN_DIR = 'data/13/collection_2/train'
SGF_VAL_DIR = 'data/13/collection_2/val'
SGF_TEST_DIR = 'data/13/collection_2/test'
SGF_VAL_PICKLE = 'data/13/collection_2/val_pickle.p'
board_size = BoardState.BOARD_SIZE
BATCHES_CHECKPOINT_INTERVAL = 5
N_DATAGEN_WORKERS = 1
MODEL_NAME = "simple_convnet"
N_X_PLANES = 8
GAME_TO_XY_FUNC = game_to_XYs_with_history
LOAD_CHECKPOINT = False
CHECKPOINT_PATH = 'train/save/simple_convnet_batch:10_valAcc:0.0233.ckpt'


# Create model
def get_model(x):
    kernel_size = [3, 3]
    n_filters = 64
    # we only do conv layers and relu to maintain dimention
    l1_conv = tf.layers.conv2d(inputs = x, filters = n_filters, kernel_size = kernel_size, padding='SAME', activation=tf.nn.relu)
    l2_conv = tf.layers.conv2d(inputs = l1_conv, filters = n_filters, kernel_size = kernel_size, padding='SAME', activation=tf.nn.relu)
    l3_conv = tf.layers.conv2d(inputs = l2_conv, filters = n_filters, kernel_size = kernel_size, padding='SAME', activation=tf.nn.relu)
    l4_conv = tf.layers.conv2d(inputs = l3_conv, filters = n_filters, kernel_size = kernel_size, padding='SAME', activation=tf.nn.relu)
    l5_conv = tf.layers.conv2d(inputs = l4_conv, filters = 1, kernel_size = [1, 1], padding='SAME', activation=tf.identity)
    print(l5_conv.shape)
    return l5_conv


def calc_acc(sess, pred, x, y, accuracy_op, cost):

    val_accs = []
    val_costs = []
    if path.isfile(SGF_VAL_PICKLE):
        print('loading validation data from pickle')
        with open(SGF_VAL_PICKLE, 'rb') as f:
            all_val_x, all_val_y = pickle.load(f)
    else:
        print('generating validation data')
        all_val_x, all_val_y = get_games_XY(SGF_VAL_DIR, game_to_XY_func=GAME_TO_XY_FUNC)
        with open(SGF_VAL_PICKLE, 'wb') as f:
            pickle.dump((all_val_x, all_val_y), f)
    for batch_i in range(0, len(all_val_x) // batch_size):
        print('validation batch:{} / {}'.format(batch_i, len(all_val_x) // batch_size))
        val_x = all_val_x[batch_i * batch_size : (batch_i+1) * batch_size]
        val_y = all_val_y[batch_i * batch_size : (batch_i+1) * batch_size]
        val_y = np.array(val_y)[: , :, :, np.newaxis]
        # flattened_pred = tf.reshape(pred, [-1, board_size * board_size])
        # flattented_y = tf.reshape(y, [-1, board_size * board_size])
        val_cost, val_acc = sess.run([cost, accuracy_op], feed_dict={x: val_x,
                                                            y: val_y})
        val_accs.append(val_acc)
        val_costs.append(val_cost)
    def avg(l):
        return sum(l) / len(l)
    # val_acc = accuracy.eval({x: test_x, y: test_y})
    print("Validation Accuracy:", avg(val_accs))
    print("Validation Cost:", avg(val_costs))
    return avg(val_accs)


def main():


    # tf Graph input
    x = tf.placeholder("float", [None, board_size, board_size, N_X_PLANES], name = 'x')
    y = tf.placeholder("float", [None, board_size, board_size, 1], name = 'y')

    # Construct model
    pred = get_model(x)
    pred = tf.identity(pred, name='pred')

    flattened_pred = tf.reshape(pred, [-1, board_size * board_size])
    flattented_y = tf.reshape(y, [-1, board_size * board_size])
    cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=flattened_pred, labels=flattented_y), name='cost')

    correct_prediction = tf.equal(tf.argmax(pred, 1), tf.argmax(y, 1))
    # Calculate accuracy
    accuracy_op = tf.reduce_mean(tf.cast(correct_prediction, "float"), name='accuracy')
    optimizer = tf.train.AdamOptimizer(learning_rate=learning_rate).minimize(cost)
    tf.add_to_collection("optimizer", optimizer)

    if LOAD_CHECKPOINT:
        saver = tf.train.import_meta_graph(CHECKPOINT_PATH + '.meta')
    else:
        saver = tf.train.Saver()

    # Launch the graph
    with tf.Session() as sess:
        # Initializing the variables
        init = tf.global_variables_initializer()
        sess.run(init)
        if LOAD_CHECKPOINT:
            saver.restore(sess, CHECKPOINT_PATH)
        tf.get_default_graph().finalize()
        # Training cycle
        for epoch in range(training_epochs):
            avg_cost = 0.
            # Loop over all batches
            n_batches = 0
            for batch_i, (batch_x, batch_y) in enumerate(
                    generate_games_XY_multiprocessing(SGF_TRAIN_DIR, batch_size,
                                                      N_WORKES = N_DATAGEN_WORKERS, game_to_XY_func=GAME_TO_XY_FUNC)
            ):
                batch_y = np.array(batch_y)[: , :, :, np.newaxis]
                batch_x_y = list(zip(batch_x, batch_y))
                shuffle(batch_x_y)
                batch_x, batch_y = list(zip(*batch_x_y))
                
                _, c = sess.run([optimizer, cost], feed_dict={x: batch_x,
                                                            y: batch_y})
                print('batch:{}, cost:{}'.format(batch_i, c))
                # Compute average loss
                avg_cost += c
                n_batches += 1
                if batch_i >= BATCHES_CHECKPOINT_INTERVAL and batch_i % BATCHES_CHECKPOINT_INTERVAL == 0:
                    print('batch:', batch_i)
                    val_acc = calc_acc(sess, pred, x, y, accuracy_op, cost)
                    save_path = saver.save(sess, 'train/save/{}_batch:{}_valAcc:{:.4f}.ckpt'.format(MODEL_NAME, batch_i, val_acc))
                    print('model saved at: {}'.format(save_path))
            avg_cost = avg_cost / n_batches
            # Display logs per epoch step
            print("Epoch:", '%04d' % (epoch+1), "train cost=", \
                "{:.9f}".format(avg_cost))
            val_acc = calc_acc(sess, pred, x, y, accuracy_op, cost)
            save_path = saver.save(sess,
                                   'train/save/{}_batch:{}_valAcc:{:.4f}.ckpt'.format(MODEL_NAME, batch_i, val_acc))
            print('model saved at: {}'.format(save_path))

        print("Optimization Finished!")
        # val_acc = calc_acc(sess, pred, x, y)
        save_path = saver.save(sess, 'train/save/{}_final.ckpt'.format(MODEL_NAME))
        print('model saved at: {}'.format(save_path))
        # # Test model
        # calc_acc(pred, x, y, -1)
