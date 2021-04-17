import numpy as np
import tensorflow as tf
import random

from RandomAgent import RandomAgent
from StateTransform import transform_state


class SequentialAgent(RandomAgent):
    def __init__(self, layers):
        super().__init__()
        self.fitness = 0
        self.model = tf.keras.Sequential()
        self.build_model(layers)

    def build_model(self, layers):
        self.model.add(tf.keras.layers.Dense(50,
                                             input_shape=(64,),
                                             activation="relu",
                                             kernel_initializer=tf.constant_initializer(layers[0]['weights']),
                                             bias_initializer=tf.constant_initializer(layers[0]['biases'])))
        self.model.add(tf.keras.layers.Dense(self.action_size,
                                             activation="sigmoid",
                                             kernel_initializer=tf.constant_initializer(layers[1]['weights']),
                                             bias_initializer=tf.constant_initializer(layers[1]['biases'])))

        optimizer = tf.keras.optimizers.Adam(learning_rate=0.01)

        self.model.compile(loss='mse',
                           metrics=[],
                           optimizer=optimizer)

    @tf.function()
    def get_action_prediction(self, state):
        return self.model(state)

    def get_action(self, state):
        action_prediction = self.get_action_prediction(transform_state(state))
        return self.actions[np.argmax(action_prediction)]
