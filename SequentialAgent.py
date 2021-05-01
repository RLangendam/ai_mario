import numpy as np
import tensorflow as tf
import random

from RandomAgent import RandomAgent
from StateTransform import transform_state

first_layer_neuron_count = 128
second_layer_neuron_count = 100


class SequentialAgent(RandomAgent):
    def __init__(self, layers=None):
        super().__init__()
        self.fitness = 0
        self.model = tf.keras.Sequential()
        self.build_model(layers)

    # @staticmethod
    # def create_random_initialized_agent():
    #     return SequentialAgent([{"weights": np.random.rand(first_layer_neuron_count, second_layer_neuron_count),
    #                              "biases": np.random.rand(second_layer_neuron_count)},
    #                             {"weights": np.random.rand(second_layer_neuron_count, 2), "biases": np.random.rand(2)}])

    def build_model(self, layers=None):
        self.model.add(tf.keras.layers.Dense(second_layer_neuron_count,
                                             input_shape=(first_layer_neuron_count,),
                                             activation="relu",
                                             kernel_initializer=tf.keras.initializers.RandomUniform() if layers is None else tf.constant_initializer(
                                                 layers[0]['weights']),
                                             bias_initializer=tf.keras.initializers.Zeros() if layers is None else tf.constant_initializer(
                                                 layers[0]['biases'])))
        self.model.add(tf.keras.layers.Dense(self.action_size,
                                             activation="softmax",
                                             kernel_initializer=tf.keras.initializers.Identity() if layers is None else tf.constant_initializer(
                                                 layers[1]['weights']),
                                             bias_initializer=tf.keras.initializers.Zeros() if layers is None else tf.constant_initializer(
                                                 layers[1]['biases'])))

        optimizer = tf.keras.optimizers.Adam(learning_rate=0.01)

        self.model.compile(loss='mse',
                           metrics=[],
                           optimizer=optimizer)

    @tf.function()
    def get_action_prediction(self, state):
        return self.model(state)

    def get_action(self, state):
        action_prediction = self.get_action_prediction(transform_state(state).reshape(1, first_layer_neuron_count))
        action_model = self.actions[np.argmax(action_prediction)]
        # action_random = super().get_action(state)
        # print(action_prediction)
        return action_model
        # return action_random if random.random() < 1 else action_model
