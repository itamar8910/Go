import numpy as np
import tensorflow as tf
from go_logic.GoLogic import BoardState
from train.make_training_data import generate_games_XY
from random import shuffle

# Parameters
learning_rate = 0.001
training_epochs = 100
batch_size = 64
SGF_TRAIN_DIR = 'data/13/go13_dummy'
SGF_TEST_DIR = 'data/13/go13_test'
n_filters = 64
board_size = BoardState.BOARD_SIZE
n_X_planes = 3
kernel_size = [3, 3]


# Create model
def get_model(x):
    
    # we only do conv layers and relu to maintain dimention
    l1_conv = tf.layers.conv2d(inputs = x, filters = n_filters, kernel_size = kernel_size, padding='SAME', activation=tf.nn.relu)
    l2_conv = tf.layers.conv2d(inputs = l1_conv, filters = n_filters, kernel_size = kernel_size, padding='SAME', activation=tf.nn.relu)
    l3_conv = tf.layers.conv2d(inputs = l2_conv, filters = n_filters, kernel_size = kernel_size, padding='SAME', activation=tf.nn.relu)
    l4_conv = tf.layers.conv2d(inputs = l3_conv, filters = n_filters, kernel_size = kernel_size, padding='SAME', activation=tf.nn.relu)
    l5_conv = tf.layers.conv2d(inputs = l4_conv, filters = 1, kernel_size = [1, 1], padding='SAME', activation=tf.identity)
    print(l5_conv.shape)
    return l5_conv

def main():  
    # tf Graph input
    x = tf.placeholder("float", [None, board_size, board_size, n_X_planes])
    y = tf.placeholder("float", [None, board_size, board_size, 1])

    # Construct model
    pred = get_model(x)
    print(pred.shape)
    print(y.shape)
    # Define loss and optimizer
    flattened_pred = tf.reshape(pred, [-1, board_size * board_size])
    flattented_y = tf.reshape(y, [-1, board_size * board_size])
    cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(logits=flattened_pred, labels=flattented_y))
    optimizer = tf.train.AdamOptimizer(learning_rate=learning_rate).minimize(cost)

    # Initializing the variables
    init = tf.global_variables_initializer()

    # Launch the graph
    with tf.Session() as sess:
        sess.run(init)

        # Training cycle
        for epoch in range(training_epochs):
            avg_cost = 0.
            # Loop over all batches
            n_batches = 0
            for batch_i, (batch_x, batch_y) in enumerate(generate_games_XY(SGF_TRAIN_DIR, batch_size)):
                print('batch:', batch_i)

                batch_y = np.array(batch_y)[: , :, :, np.newaxis]
                batch_x_y = list(zip(batch_x, batch_y))
                shuffle(batch_x_y)
                batch_x, batch_y = list(zip(*batch_x_y))
                
                _, c = sess.run([optimizer, cost], feed_dict={x: batch_x,
                                                            y: batch_y})
                # Compute average loss
                avg_cost += c
                n_batches += 1
            avg_cost = avg_cost / n_batches
            # Display logs per epoch step
            print("Epoch:", '%04d' % (epoch+1), "cost=", \
                "{:.9f}".format(avg_cost))
        print("Optimization Finished!")

        # Test model
        correct_prediction = tf.equal(tf.argmax(pred, 1), tf.argmax(y, 1))
        # Calculate accuracy
        accuracy = tf.reduce_mean(tf.cast(correct_prediction, "float"))
        for test_x, test_y in generate_games_XY(SGF_TRAIN_DIR, batch_size=-1):
            test_y = np.array(test_y)[: , :, :, np.newaxis]
            print("Accuracy:", accuracy.eval({x: test_x, y: test_y}))
