import numpy as np
import tensorflow as tf
from go_logic.GoLogic import BoardState
from train.make_training_data import generate_games_XY
from random import shuffle
import time

# Parameters
learning_rate = 0.001
training_epochs = 100
batch_size = 64
SGF_TRAIN_DIR = 'data/13/collection_1/train'
SGF_VAL_DIR = 'data/13/collection_1/val'
SGF_TEST_DIR = 'data/13/collection_1/test'
board_size = BoardState.BOARD_SIZE
N_X_PLANES = 3
CHECKPOINT_INTERVAL = 5
MODEL_NAME = "simple_convnet"



# Create model
def get_model(x):
    kernel_size = [3, 3]
    n_filters = 64
    # we only do conv layers and relu to maintain dimention
    l1_conv = tf.layers.conv2d(inputs = x, filters = n_filters, kernel_size = kernel_size, padding='SAME', activation=tf.nn.relu)
    l2_conv = tf.layers.conv2d(inputs = l1_conv, filters = n_filters, kernel_size = kernel_size, padding='SAME', activation=tf.nn.relu)
    l3_conv = tf.layers.conv2d(inputs = l2_conv, filters = n_filters, kernel_size = kernel_size, padding='SAME', activation=tf.nn.relu)
    l4_conv = tf.layers.conv2d(inputs = l3_conv, filters = n_filters, kernel_size = kernel_size, padding='SAME', activation=tf.nn.relu)
    l5_conv = tf.layers.conv2d(inputs = l4_conv, filters = 1, kernel_size = [1, 1], padding='SAME', activation=tf.identity, name='pred')
    print(l5_conv.shape)
    return l5_conv


def calc_acc(pred, x, y, VAL_SIZE = 500):
    correct_prediction = tf.equal(tf.argmax(pred, 1), tf.argmax(y, 1))
    # Calculate accuracy
    accuracy = tf.reduce_mean(tf.cast(correct_prediction, "float"))
    for test_x, test_y in generate_games_XY(SGF_VAL_DIR, batch_size=VAL_SIZE):
        test_y = np.array(test_y)[: , :, :, np.newaxis]
        val_acc = accuracy.eval({x: test_x, y: test_y})
        print("Validation Accuracy:", val_acc)
        return val_acc

def main():

    LOAD_CHECKPOINT = True
    CHECKPOINT_PATH = 'train/save/simple_convnet_batch:5_valAcc:0.0283.ckpt'

    # tf Graph input
    x = tf.placeholder("float", [None, board_size, board_size, N_X_PLANES], name = 'x')
    y = tf.placeholder("float", [None, board_size, board_size, 1], name = 'y')

    # Construct model
    pred = get_model(x)


    # Launch the graph
    with tf.Session() as sess:
        
        if LOAD_CHECKPOINT:
            saver = tf.train.Saver()
            saver.restore(sess, CHECKPOINT_PATH)
            x = tf.get_default_graph().get_tensor_by_name('x:0')
            y = tf.get_default_graph().get_tensor_by_name('y:0')
            pred = tf.get_default_graph().get_tensor_by_name('pred:0')
        else:
            saver = tf.train.Saver()
            
        # Define loss and optimizer
        flattened_pred = tf.reshape(pred, [-1, board_size * board_size])
        flattented_y = tf.reshape(y, [-1, board_size * board_size])
        cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=flattened_pred, labels=flattented_y))
        optimizer = tf.train.AdamOptimizer(learning_rate=learning_rate).minimize(cost)
        if not LOAD_CHECKPOINT:
            # Initializing the variables
            init = tf.global_variables_initializer()
            sess.run(init)
        # Training cycle
        for epoch in range(training_epochs):
            avg_cost = 0.
            # Loop over all batches
            n_batches = 0
            for batch_i, (batch_x, batch_y) in enumerate(generate_games_XY(SGF_TRAIN_DIR, batch_size)):
                batch_y = np.array(batch_y)[: , :, :, np.newaxis]
                batch_x_y = list(zip(batch_x, batch_y))
                shuffle(batch_x_y)
                batch_x, batch_y = list(zip(*batch_x_y))
                
                _, c = sess.run([optimizer, cost], feed_dict={x: batch_x,
                                                            y: batch_y})
                # Compute average loss
                avg_cost += c
                n_batches += 1
                if batch_i % CHECKPOINT_INTERVAL == 0:
                    print('batch:', batch_i)
                    val_acc = calc_acc(pred, x, y)
                    save_path = saver.save(sess, 'train/save/{}_batch:{}_valAcc:{:.4f}.ckpt'.format(MODEL_NAME, batch_i, val_acc))
                    print('model saved at: {}'.format(save_path))
            avg_cost = avg_cost / n_batches
            # Display logs per epoch step
            print("Epoch:", '%04d' % (epoch+1), "cost=", \
                "{:.9f}".format(avg_cost))
        print("Optimization Finished!")

        # Test model
        calc_acc(pred, x, y, -1)
