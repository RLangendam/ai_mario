import numpy as np
import tensorflow as tf
import random

from RandomAgent import RandomAgent
from StateTransform import transform_state

first_layer_neuron_count = 64


class SequentialAgent(RandomAgent):
    def __init__(self, layers):
        super().__init__()
        self.fitness = 0
        self.model = tf.keras.Sequential()
        self.build_model(layers)

    @staticmethod
    def create_random_initialized_agent():
        return SequentialAgent([{"weights": np.random.rand(first_layer_neuron_count, 50), "biases": np.random.rand(50)},
                                {"weights": np.random.rand(50, 2), "biases": np.random.rand(2)}])

    def build_model(self, layers):
        self.model.add(tf.keras.layers.Dense(50,
                                             input_shape=(first_layer_neuron_count,),
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
        action_prediction = self.get_action_prediction(transform_state(state).reshape(1, first_layer_neuron_count))
        return self.actions[np.argmax(action_prediction)]
